#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <mysql/mysql.h>
#include <sstream>
#include "graph.h"

graph get_graph_from_database(const string &graph_name)
{
    try
    {
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

        con->setSchema("Graphs");

        // Create a prepared statement to fetch graph_id
        unique_ptr<sql::PreparedStatement> pstmt_graph_id(con->prepareStatement("SELECT * FROM graphs WHERE graph_name = ?"));
        pstmt_graph_id->setString(1, graph_name);
        unique_ptr<sql::ResultSet> res_graph_id(pstmt_graph_id->executeQuery());

        // 创建一张图
        graph g;
        int graph_id = 0;
        int correction = 0;
        double graph_heat = 0;
        double graph_score = 0;
        string description;
        string graph_name;
        string graph_kind;
        if (res_graph_id->next())
        {
            graph_id = res_graph_id->getInt("id");
            graph_heat = res_graph_id->getDouble("graph_heat");
            graph_score = res_graph_id->getDouble("graph_score");
            graph_name = res_graph_id->getString("graph_name");
            graph_kind = res_graph_id->getString("graph_kind");
            description = res_graph_id->getString("description");
            g.set_graph_heat(graph_heat);
            g.set_graph_score(graph_score);
            g.set_graph_id(graph_id);
            g.set_graph_name(graph_name);
            g.set_graph_description(description);
            g.set_graph_kind(graph_kind);
        }
        else
        {
            return graph(); // Return an empty graph object
        }

        sql::ResultSet *r;
        sql::PreparedStatement *p;
        std::string query = "SELECT b.id "
                            "FROM buildings b "
                            "JOIN graph_buildings gb ON b.id = gb.building_id "
                            "WHERE gb.graph_id = ? "
                            "ORDER BY b.id ASC "
                            "LIMIT 1;";
        p = con->prepareStatement(query);

        // Set the parameter value
        p->setInt(1, graph_id);
        r = p->executeQuery();
        if (r->next())
        {
            correction = r->getInt("id");
        }

        // Create a prepared statement to fetch buildings data
        unique_ptr<sql::PreparedStatement> pstmt_buildings(con->prepareStatement("SELECT * FROM buildings WHERE id IN (SELECT building_id FROM graph_buildings WHERE graph_id = ?)"));
        pstmt_buildings->setInt(1, graph_id);
        unique_ptr<sql::ResultSet> res_buildings(pstmt_buildings->executeQuery());

        // Create a prepared statement to fetch roads data
        unique_ptr<sql::PreparedStatement> pstmt_roads(con->prepareStatement("SELECT * FROM roads WHERE id IN (SELECT road_id FROM graph_roads WHERE graph_id = ?)"));
        pstmt_roads->setInt(1, graph_id);
        unique_ptr<sql::ResultSet> res_roads(pstmt_roads->executeQuery());

        // Fetch buildings data and populate the buildings vector
        while (res_buildings->next())
        {
            building b;
            int x = res_buildings->getInt("axis_x");
            int y = res_buildings->getInt("axis_y");
            string building_id = res_buildings->getString("building_id");
            string building_name = res_buildings->getString("building_name");
            int building_kind = res_buildings->getInt("building_kind");
            b.set_x(x);
            b.set_y(y);
            b.set_building_id(building_id);
            b.set_building_kind(building_kind);
            b.set_building_name(building_name);
            g.buildings.push_back(b);
        }

        // Fetch roads data and populate the roads vector
        while (res_roads->next())
        {
            int start_building_id = res_roads->getInt("start_building_id");
            start_building_id -= correction;
            int end_building_id = res_roads->getInt("end_building_id");
            end_building_id -= correction;
            double crowding = res_roads->getDouble("crowding");
            double distance = res_roads->getDouble("road_distance");
            string road_id = res_roads->getString("road_id");

            ostringstream start_id_stream;
            start_id_stream << "Building" << (start_building_id);
            string start_id = start_id_stream.str();

            ostringstream end_id_stream;
            end_id_stream << "Building" << (end_building_id);
            string end_id = end_id_stream.str();

            // Find start and end buildings in the graph
            building *start = nullptr;
            building *end = nullptr;
            for (auto &b : g.buildings)
            {
                if (start_id == b.get_building_id())
                    start = &b;

                if (end_id == b.get_building_id())
                    end = &b;
            }

            // Add road if start and end buildings are found
            if (start_building_id && end_building_id)
            {
                road r;
                r.set_road_start(start);
                r.set_road_end(end);
                r.set_road_crowding(crowding);
                r.set_road_distance(distance);
                r.set_road_id(road_id);
                g.roads.push_back(r);
            }
        }

        return g;
    }

    catch (sql::SQLException &e)
    {
        cerr << "SQLException: " << e.what() << endl;
        cerr << "MySQL error code: " << e.getErrorCode() << endl;
        cerr << "SQLState: " << e.getSQLState() << endl;
        // Return an empty graph object in case of error
        return graph();
    }
}

void print_graph_info()
{
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;

    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

    con->setSchema("Graphs");

    // 执行查询语句
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM graphs limit 100");

    graph g;
    // 打印查询结果
    while (res->next())
    {
        cout << res->getString("description") << endl;
    }

    // 释放内存和关闭连接
    delete res;
    delete stmt;
    delete con;
}

void order_graph(const int &user_choice)
{
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;

    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

    con->setSchema("Graphs");

    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    switch (user_choice)
    {
    case 1:
        res = stmt->executeQuery("SELECT graph_name, description FROM graphs ORDER BY graph_heat DESC LIMIT 10");
        break;
    case 2:
        res = stmt->executeQuery("SELECT graph_name, description FROM graphs ORDER BY graph_score DESC LIMIT 10");
        break;
    case 3:
        res = stmt->executeQuery("SELECT graph_name, description FROM graphs ORDER BY graph_score DESC, graph_heat DESC LIMIT 10");
        break;
    case 4:
    {
        string target;
        cout << "Cultural Attractions, Historical Sites, Modern Architecture, University, Natural Landmarks" << endl;
        cout << "Enter graph kind: ";
        cin.clear(); // 清除错误标志
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, target);
        string updateQuery = "SELECT graph_name, description FROM graphs WHERE graph_kind LIKE ? ORDER BY graph_score DESC, graph_heat DESC LIMIT 10";
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(updateQuery));
        pstmt->setString(1, "%" + target + "%");
        res = pstmt->executeQuery();
        break;
    }
    case 5:
    {
        string target;
        cout << "Enter graph name: ";
        cin.clear(); // 清除错误标志
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, target);
        string updateQuery = "SELECT graph_name, description FROM graphs WHERE graph_name LIKE ? ORDER BY graph_score DESC, graph_heat DESC LIMIT 10";
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(updateQuery));
        pstmt->setString(1, "%" + target + "%");
        res = pstmt->executeQuery();
        break;
    }
    default:
    {
        cout << "Invalid Input" << endl;
        cin.clear(); // 清除错误标志
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        break;
    }
    }

    while (res->next())
    {
        cout << "graph_name: " << res->getString("graph_name") << endl;
        cout << "description: " << res->getString("description") << endl;
        cout << endl;
    }

    // 释放内存和关闭连接
    delete res;
    delete stmt;
    delete con;
}
