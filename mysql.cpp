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

/// @brief 对数据库中的数据进行设置
/// @param table_name 数据库中表的名称
/// @param column_name 表中列的名称
void set_data_to_database(const string &table_name, const string &column_name)
{
    try
    {
        // 创建MySQL连接
        sql::Driver *driver = get_driver_instance();
        sql::Connection *conn = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");
        conn->setSchema("Graphs");

        // 创建Statement对象
        sql::Statement *stmt = conn->createStatement();

        string table_name = "graphs";
        string column_name = "graph_kind";

        // 从数据库中查询所有行的ID
        string idQuery = "SELECT id FROM " + table_name;
        sql::ResultSet *rs = stmt->executeQuery(idQuery);

        // 遍历每个ID,并为其设置新值
        while (rs->next())
        {
            int id = rs->getInt("id");

            string newValue = to_string(rand() % 10); // 设置的新值，可以通过设置数组完成自动赋值
            string updateQuery = "UPDATE " + table_name + " SET " + column_name + " = ? WHERE id = ?";
            unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(updateQuery));
            pstmt->setString(1, newValue);
            pstmt->setInt(2, id);
            pstmt->executeUpdate();
        }

        delete rs;
        delete stmt;
        delete conn;
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

/// @brief 从数据库中获取数据
/// @param table_name 数据库中表的名称
/// @param column_name 表中列的名称
void get_data_from_database(const string &table_name, const string &column_name)
{
    try
    {
        sql::Driver *driver = get_driver_instance();
        sql::Connection *conn = driver->connect("tcp://127.0.0.1", "admin", "admin123");
        sql::PreparedStatement *prep_stmt = nullptr;
        sql::ResultSet *res = nullptr; // 遍历所有行
        sql::ResultSet *rs = nullptr;  // 遍历所有符合条件的行数
        conn->setSchema("Graphs");
        string update_query = "SELECT " + column_name + " FROM " + table_name + " LIMIT 100";
        prep_stmt = conn->prepareStatement(update_query);
        res = prep_stmt->executeQuery();

        while (res->next())
        {
            cout << res->getString(column_name) << endl;
        }
    }
    catch (sql::SQLException &e)
    {
        cerr << "SQLException: " << e.what() << endl;
        cerr << "MySQL error code: " << e.getErrorCode() << endl;
        cerr << "SQLState: " << e.getSQLState() << endl;
    }
}

void set_graph_to_database(const graph &g, const string &graph_name, const string &description)
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

graph get_graph_from_database(const string &graph_name)
{
    // Connect to the database
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "admin", "admin123"));
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

    // 创建MySQL连接
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

    // 连接到数据库
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

/// @brief 查询图的函数
/// @param user_choice
void order_graph(const int &user_choice)
{

    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;

    // 创建MySQL连接
    driver = sql::mysql::get_mysql_driver_instance();
    conn = driver->connect("tcp://127.0.0.1:3306", "admin", "admin123");

    // 连接到数据库
    conn->setSchema("Graphs");

    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = conn->createStatement();
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
        unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(updateQuery));
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
        unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(updateQuery));
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
    delete conn;
}
