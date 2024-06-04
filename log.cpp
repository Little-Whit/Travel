#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include "graph.h"

using namespace std;

int main()
{
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;

    // 建立MySQL连接
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

    // 连接到数据库
    con->setSchema("Graphs");

    // 创建用户表
    sql::Statement *stmt;
    stmt = con->createStatement();
    stmt->execute("CREATE TABLE IF NOT EXISTS users ("
                  "id INT AUTO_INCREMENT PRIMARY KEY,"
                  "username VARCHAR(255),"
                  "password VARCHAR(255)"
                  ")");

    // 获取用户输入
    string username, password;
    cout << "please enter your username:";
    cin >> username;
    cout << "please enter your password:";
    cin >> password;

    // 执行插入语句，将用户数据插入到用户表中
    stmt->execute("INSERT INTO users (username, password) "
                  "VALUES ('" +
                  username + "', '" + password + "')");

    cout << "Successfully sign up" << endl;

    // 释放内存和关闭连接
    delete stmt;
    delete con;

    return 0;
}
