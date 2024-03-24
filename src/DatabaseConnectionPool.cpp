#include "../include/DatabaseConnectionPool.h"
#include "../include/public.h"

#include <memory>


// 线程安全的懒汉代理函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
    // 静态局部变量的初始化，编译器会帮我们保证其是线程安全的
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadConfigFile()
{
    FILE* pf = fopen("config/mysql.config", "r");
    if (nullptr == pf)
    {
        LOG("mysql.config file open failed");
        // perror("open file: ");
        return false;
    }

    while (!feof(pf))
    {
        char line[1024] = { 0 };
        fgets(line, 1024, pf);
        std::string str = line;
        int idx = str.find("=", 0);
        // 注释/无效配置项
        if (idx == -1)
        {
            continue;
        }

        int endidx = str.find('\n', idx);
        std::string key = str.substr(0, idx);
        std::string value = str.substr(idx + 1, endidx - idx - 1);

        if (key == "ip")
        {
            _ip = value;
        }
        else if (key == "port")
        {
            _port = static_cast<unsigned short>(stoi(value));
        }
        else if (key == "dbname")
        {
            _dbname = value;
        }
        else if (key == "username")
        {
            _username = value;
        }
        else if (key == "password")
        {
            _password = value;
        }
        else if (key == "initSize")
        {
            _initSize = stoi(value);
        }
        else if (key == "maxSize")
        {
            _maxSize = stoi(value);
        }
        else if (key == "maxIdleTime")
        {
            _maxIdletime = stoi(value);
        }
        else if (key == "connectionTimeout")
        {
            _connectionTimeout = stoi(value);
        }

    }
    return true;
}

ConnectionPool::ConnectionPool()
{
    // 加载配置项
    if (false == loadConfigFile())
    {
        return;
    }

    // 创建初始数量的连接
    for (int i = 0; i < _initSize; i++)
    {
        Connection* conn = new Connection();
        conn->connect(_ip, _port, _dbname, _username, _password);
        conn->refreshAliveTime();
        _connectionQueue.push(conn);
        _connectionCount++;
    }

    // 启动一个线程，作为连接的生产者
    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，对其回收
    std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        // 队列不空，生产线程进入等待状态
        while (!_connectionQueue.empty())
        {
            _queueCond.wait(lock);
        }

        // 连接数量没有到达上限，继续创建新的连接
        if (_connectionCount < _maxSize)
        {
            Connection* conn = new Connection();
            conn->connect(_ip, _port, _dbname, _username, _password);
            conn->refreshAliveTime(); // 刷新一下开始空闲的起始时间
            _connectionQueue.push(conn);
            _connectionCount++;
        }

        // 通知消费者线程可以消费连接了
        _queueCond.notify_all();

    }
}

// 给外部提供接口，可以从连接池中获取一个可用的空闲连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    while (_connectionQueue.empty())
    {
        if (std::cv_status::timeout == _queueCond.wait_for(lock, std::chrono::milliseconds(_connectionTimeout)))
        {
            if (_connectionQueue.empty())
            {
                LOG("获取空闲连接超时，获取连接失败！");
                return nullptr;
            }
        }
    }
    // shared_ptr析构时会把connection资源析构掉，相当于调用connection析构函数，connection就会被close掉了
    // 我们需要自定义shared_ptr的资源释放方式，把connection直接归还到queue中
    std::shared_ptr<Connection> sp(_connectionQueue.front(),
        [&](Connection* conn) {
            // 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全
            std::unique_lock<std::mutex> lock(_queueMutex);
            conn->refreshAliveTime();
            _connectionQueue.push(conn);
        });
    _connectionQueue.pop();

    // 消费完连接以后，通知生产者线程检查一下，队列为空赶紧生产
    _queueCond.notify_all();

    return sp;
}


void ConnectionPool::scannerConnectionTask()
{
    while (true)
    {
        // 通过sleep模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdletime));

        // 扫描整个队列，释放多余连接
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_connectionCount > _initSize)
        {
            Connection* conn = _connectionQueue.front();
            if (conn->getAliveTime() >= (_maxIdletime * 1000))
            {
                _connectionQueue.pop();
                _connectionCount--;
                delete conn;
            }
            else
            {
                // 队头的连接都没有超时，其他连接肯定不会超时
                break;
            }
        }
    }
}