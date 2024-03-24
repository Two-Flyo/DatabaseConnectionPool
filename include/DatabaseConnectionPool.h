#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>
#include <condition_variable>

#include "Connection.h"

/*实现连接池功能模块*/

class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool* getConnectionPool();
    // 给外部提供接口，可以从连接池中获取一个可用的空闲连接
    std::shared_ptr<Connection> getConnection();
private:
    ConnectionPool(); // 单例#1 构造函数私有化
    bool loadConfigFile(); // 从配置文件中加载配置项
    // 这个独立的线程，专门用来生产新连接
    void produceConnectionTask();

    // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，对其回收    
    void scannerConnectionTask();

    std::string _ip; // 数据库ip地址
    unsigned short _port; // 数据库端口号
    std::string _dbname; // 数据库名称
    std::string _username; // 数据库登录用户名
    std::string _password; // 数据库登录密码
    int _initSize; // 连接池初始连接量
    int _maxSize; // 连接池最大连接量
    int _maxIdletime; // 连接池最大空闲时间
    int _connectionTimeout; // 连接池获取连接的超时时间

    std::queue<Connection*> _connectionQueue; // 存储数据库连接的队列
    std::mutex _queueMutex; // 维护连接队列的线程安全互斥锁
    std::atomic<int> _connectionCount; // 用来记录所创建的connection连接的总数量
    std::condition_variable _queueCond; // 设置条件变量，用于连接生产线程和消费线程的通信
};