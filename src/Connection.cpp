#include "../include/public.h" 
#include "../include/Connection.h"

// 初始化数据库连接
Connection::Connection()
{
    _conn = mysql_init(nullptr);
}

Connection::~Connection()
{
    if (_conn != nullptr)
    {
        mysql_close(_conn);
    }
}

// 连接数据库
bool Connection::connect(std::string ip, unsigned short port, std::string dbname, std::string user,
    std::string password)
{
    MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
        password.c_str(), dbname.c_str(), port, nullptr, 0);

    if (p != nullptr)
        return true;
    else
        return false;
}

// 更新数据库
bool Connection::update(std::string sql)
{
    if (mysql_query(_conn, sql.c_str()) != 0)
    {
        LOG(sql + "> 更新失败：" + mysql_error(_conn));
        return false;
    }
    return true;
}

// 查询数据库
MYSQL_RES* Connection::query(std::string sql)
{
    if (mysql_query(_conn, sql.c_str()) != 0)
    {
        LOG(sql + "> 查询失败：" + mysql_error(_conn));
        return nullptr;
    }

    return mysql_use_result(_conn);
}

void Connection::refreshAliveTime()
{
    _alivetime = clock();
}

clock_t Connection::getAliveTime() const
{
    return clock() - _alivetime;
}