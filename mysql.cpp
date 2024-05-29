#include "graph.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

void storeBuildingToDatabase(const building &b)
{
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "root", "8911825zxt"));
        con->setSchema("buildings_db");

        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO buildings (x, y, building_id, building_kind) VALUES (?, ?, ?, ?)"));
        pstmt->setInt(1, b.get_x());
        pstmt->setInt(2, b.get_y());
        pstmt->setString(3, b.get_building_id());
        pstmt->setInt(4, static_cast<int>(b.get_building_kind()));
        pstmt->execute();
    }
    catch (sql::SQLException &e)
    {
        cerr << "SQLException: " << e.what() << endl;
    }
}

vector<building> loadBuildingsFromDatabase()
{
    vector<building> buildings;

    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "root", "8911825zxt"));
        con->setSchema("buildings_db");

        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT x, y, building_id, building_kind FROM buildings"));

        while (res->next())
        {
            building b;
            b.set_x(res->getInt("x"));
            b.set_y(res->getInt("y"));
            b.set_building_id(res->getString("building_id"));
            b.set_building_kind(static_cast<BUILDING_KIND>(res->getInt("building_kind")));
            buildings.push_back(b);
        }
    }
    catch (sql::SQLException &e)
    {
        cerr << "SQLException: " << e.what() << endl;
    }

    return buildings;
}

int main()
{
    srand(time(0)); // 初始化随机数种子

    vector<building> buildings(50); // 创建50个building对象

    for (const auto &b : buildings)
    {
        storeBuildingToDatabase(b);
    }

    return 0;
}
