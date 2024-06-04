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

void store_graph_to_database(const graph &g, const string &graph_name, const string &description)
{
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "admin", "admin123"));
        con->setSchema("Graphs");

        // 插入图信息
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO graphs (graph_name, description) VALUES (?, ?)"));
        pstmt->setString(1, graph_name);
        pstmt->setString(2, description);
        pstmt->execute();

        // 获取插入的 graph_id
        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID() as graph_id"));
        int graph_id = 0;
        if (res->next())
        {
            graph_id = res->getInt("graph_id");
        }

        // 插入 buildings
        pstmt.reset(con->prepareStatement("INSERT INTO buildings (axis_x, axis_y, building_id, building_kind) VALUES (?, ?, ?, ?)"));
        for (const auto &b : g.buildings)
        {
            pstmt->setInt(1, b.get_x());
            pstmt->setInt(2, b.get_y());
            pstmt->setString(3, b.get_building_id());
            pstmt->setInt(4, b.get_building_kind());
            pstmt->execute();
        }

        // 获取所有 building 的 ID
        vector<int> building_ids;
        res.reset(stmt->executeQuery("SELECT id FROM buildings ORDER BY id DESC LIMIT " + to_string(g.buildings.size())));
        while (res->next())
        {
            building_ids.push_back(res->getInt("id"));
        }
        reverse(building_ids.begin(), building_ids.end());

        // 插入 roads
        pstmt.reset(con->prepareStatement("INSERT INTO roads (start_building_id, end_building_id, crowding,road_distance, road_id) VALUES (?, ?, ?, ?,?)"));
        for (const auto &r : g.roads)
        {
            int start_index = distance(g.buildings.begin(), find(g.buildings.begin(), g.buildings.end(), *r.start));
            int end_index = distance(g.buildings.begin(), find(g.buildings.begin(), g.buildings.end(), *r.end));
            pstmt->setInt(1, building_ids[start_index]);
            pstmt->setInt(2, building_ids[end_index]);
            pstmt->setDouble(3, r.get_road_crowding());
            pstmt->setDouble(4, r.get_road_distance());
            pstmt->setString(5, r.get_road_id());
            pstmt->execute();
        }

        // 获取所有 road 的 ID
        vector<int> road_ids;
        res.reset(stmt->executeQuery("SELECT id FROM roads ORDER BY id DESC LIMIT " + to_string(g.roads.size())));
        while (res->next())
        {
            road_ids.push_back(res->getInt("id"));
        }
        reverse(road_ids.begin(), road_ids.end());

        // 插入 graph_buildings
        pstmt.reset(con->prepareStatement("INSERT INTO graph_buildings (graph_id, building_id) VALUES (?, ?)"));
        for (const auto &building_id : building_ids)
        {
            pstmt->setInt(1, graph_id);
            pstmt->setInt(2, building_id);
            pstmt->execute();
        }

        // 插入 graph_roads
        pstmt.reset(con->prepareStatement("INSERT INTO graph_roads (graph_id, road_id) VALUES (?, ?)"));
        for (const auto &road_id : road_ids)
        {
            pstmt->setInt(1, graph_id);
            pstmt->setInt(2, road_id);
            pstmt->execute();
        }

        cout << "Graph stored successfully with graph_id: " << graph_id << endl;
    }
    catch (sql::SQLException &e)
    {
        cerr << "SQLException: " << e.what() << endl;
        cerr << "MySQL error code: " << e.getErrorCode() << endl;
        cerr << "SQLState: " << e.getSQLState() << endl;
    }
}

graph load_graph_from_database(const string &graph_name)
{
    // Connect to the database
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "admin", "admin123"));
        con->setSchema("Graphs");

        // Create a prepared statement to fetch graph_id
        unique_ptr<sql::PreparedStatement> pstmt_graph_id(con->prepareStatement("SELECT id FROM graphs WHERE graph_name = ?"));
        pstmt_graph_id->setString(1, graph_name);
        unique_ptr<sql::ResultSet> res_graph_id(pstmt_graph_id->executeQuery());
        int graph_id = 0;
        if (res_graph_id->next())
        {
            graph_id = res_graph_id->getInt("id");
        }
        else
        {
            cerr << "Graph with name '" << graph_name << "' not found in the database." << endl;
            return graph(); // Return an empty graph object
        }

        // Create a prepared statement to fetch buildings data
        unique_ptr<sql::PreparedStatement> pstmt_buildings(con->prepareStatement("SELECT * FROM buildings WHERE id IN (SELECT building_id FROM graph_buildings WHERE graph_id = ?)"));
        pstmt_buildings->setInt(1, graph_id);
        unique_ptr<sql::ResultSet> res_buildings(pstmt_buildings->executeQuery());

        // Create a prepared statement to fetch roads data
        unique_ptr<sql::PreparedStatement> pstmt_roads(con->prepareStatement("SELECT * FROM roads WHERE id IN (SELECT road_id FROM graph_roads WHERE graph_id = ?)"));
        pstmt_roads->setInt(1, graph_id);
        unique_ptr<sql::ResultSet> res_roads(pstmt_roads->executeQuery());

        // Create a new graph object
        graph g;

        // Fetch buildings data and populate the buildings vector
        while (res_buildings->next())
        {
            int x = res_buildings->getInt("axis_x");
            int y = res_buildings->getInt("axis_y");
            string building_id = res_buildings->getString("building_id");
            int building_kind = res_buildings->getInt("building_kind");
            building b;
            b.set_x(x);
            b.set_y(y);
            b.set_building_id(building_id);
            b.set_building_kind(building_kind);
            g.buildings.push_back(b);
        }

        // Fetch roads data and populate the roads vector
        while (res_roads->next())
        {
            int start_building_id = res_roads->getInt("start_building_id");
            int end_building_id = res_roads->getInt("end_building_id");
            double crowding = res_roads->getDouble("crowding");
            double distance = res_roads->getDouble("road_distance");
            string road_id = res_roads->getString("road_id");

            string start_id = "building" + to_string(start_building_id);
            string end_id = "building" + to_string(end_building_id);

            // Find start and end buildings in the graph
            building *start = nullptr;
            building *end = nullptr;
            for (auto &b : g.buildings)
            {
                if (b.get_building_id() == start_id)
                    start = &b;
                else if (b.get_building_id() == end_id)
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

    // 创建MySQL连接
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

    // 连接到数据库
    con->setSchema("Graphs");

    // 执行查询语句
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM graphs");

    // 打印查询结果
    while (res->next())
    {
        cout << "graph_name: " << res->getString("graph_name") << endl;
        cout << "description " << res->getString("description") << endl;
        cout << endl;
        // 继续打印其他字段
    }

    // 释放内存和关闭连接
    delete res;
    delete stmt;
    delete con;
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    // int building_num;
    // int road_num;
    // cin >> building_num;
    // cin >> road_num;
    // graph g(building_num,road_num);

    // graph g(30, 100);
    // store_graph_to_database(g, "Example Graph", "This is an example graph.");

    // graph g = load_graph_from_database("Differ");
    // g.print();

    print_graph_info();

    return 0;
}
