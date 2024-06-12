#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "graph.h"

void user_signup()
{
    try
    {
        // 连接 MySQL 数据库
        sql::Driver *driver = get_driver_instance();
        sql::Connection *con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");
        con->setSchema("Users");

        // 注册新用户
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        sql::PreparedStatement *pstmt = con->prepareStatement("INSERT INTO users (username, password) VALUES (?, ?)");
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        pstmt->execute();
        cout << "User registered successfully!" << endl;

        // 清理资源
        delete pstmt;
        delete con;
    }

    catch (sql::SQLException &e)
    {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;
    }
}

bool user_login()
{
    try
    {
        // 连接 MySQL 数据库
        sql::Driver *driver = get_driver_instance();
        sql::Connection *con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");
        con->setSchema("Users");

        // 注册新用户
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        sql::PreparedStatement *pstmt = con->prepareStatement("SELECT * FROM users WHERE username = ? AND password = ?");
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        sql::ResultSet *res = pstmt->executeQuery();

        if (res->next())
        {
            cout << "Login successful!" << endl;
            return true;
        }
        else
        {
            cout << "Invalid username or password." << endl;
        }

        // 清理资源
        delete res;
        delete pstmt;
        delete con;
        return false;
    }

    catch (sql::SQLException &e)
    {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;
        return false;
    }
}
