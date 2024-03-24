#include <iostream>

#include "../include/Connection.h"
#include "../include/DatabaseConnectionPool.h"

int main()
{

    auto start = std::chrono::high_resolution_clock::now();

    std::thread t1([&]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; i++)
        {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s', %d, '%s')", "张三", 20, "male");
            std::shared_ptr<Connection> conn = cp->getConnection();
            conn->update(sql);
        }
        });

    std::thread t2([&]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; i++)
        {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s', %d, '%s')", "张三", 20, "male");
            std::shared_ptr<Connection> conn = cp->getConnection();
            conn->update(sql);
        }
        });

    std::thread t3([&]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; i++)
        {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s', %d, '%s')", "张三", 20, "male");
            std::shared_ptr<Connection> conn = cp->getConnection();
            conn->update(sql);
        }
        });

    std::thread t4([&]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 250; i++)
        {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s', %d, '%s')", "张三", 20, "male");
            std::shared_ptr<Connection> conn = cp->getConnection();
            conn->update(sql);
        }
        });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}

#if 0
int main()
{
    // Connection conn;
    // char sql[1024] = { 0 };
    // sprintf(sql, "insert into user(name,age,sex) values('%s', %d, '%s')", "张三", 20, "male");
    // conn.connect("127.0.0.1", 6877, "chat", "root", "123456");
    // conn.update(sql);
    // ConnectionPool* cp = ConnectionPool::getConnectionPool();
    // clock_t begin = clock();
    auto start = std::chrono::high_resolution_clock::now();
    ConnectionPool* cp = ConnectionPool::getConnectionPool();

    for (int i = 0; i < 1000; i++)
    {
        // Connection conn;
        char sql[1024] = { 0 };
        sprintf(sql, "insert into user(name,age,sex) values('%s', %d, '%s')", "张三", 20, "male");
        // conn.connect("127.0.0.1", 6877, "chat", "root", "123456");
        // conn.update(sql);
        std::shared_ptr<Connection> conn = cp->getConnection();
        conn->update(sql);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    // double time_spent = (double)(end - begin) * 1000 / CLOCKS_PER_SEC;
    // std::cout << time_spent << "ms" << std::endl;

    return 0;
}

#endif 