#pragma once
#include <string>
#include <mysql/mysql.h>

#include <ctime>

#include "public.h"


/*实现数据库的增删改查操作*/

class Connection
{
public:
    // 初始化数据库连接
    Connection();

    ~Connection();

    // 连接数据库
    bool connect(std::string ip, unsigned short port, std::string dbname, std::string user,
        std::string password);


    // 更新操作 insert、delete、update
    bool update(std::string sql);

    // 查询操作 select
    MYSQL_RES* query(std::string sql);

    // 刷新一下连接起始的空闲时间
    void refreshAliveTime();
    clock_t getAliveTime()const;

private:
    MYSQL* _conn;
    clock_t _alivetime; // 记录进入空闲状态(也即进入队列)后的起始存活时间
};