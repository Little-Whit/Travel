#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <queue>
#include <array>
#include <random>
#include <cfloat>
#include <algorithm>
#include <climits>
#include <stdexcept>
#include <limits>
#include <optional>
#include <string_view>
#include <unordered_map>
using namespace std;

enum BUILDING_KIND // 编号 0 ~ 9 为服务设施，10 ~ 13 为建筑物
{
    Hotel,
    Canteen,
    Restaurant,
    Drugstore,
    Hospital,
    Toilet,
    Trainstation,
    Busstation,
    Shop,
    Workshop,
    Museum,
    Landmark,
    Monument,
    Garden,
};

string building_kind_to_string(BUILDING_KIND kind)
{
    switch (kind)
    {
    case Hotel:
        return "Hotel";
    case Canteen:
        return "Canteen";
    case Restaurant:
        return "Restaurant";
    case Drugstore:
        return "Drugstore";
    case Hospital:
        return "Hospital";
    case Toilet:
        return "Toilet";
    case Trainstation:
        return "Trainstation";
    case Busstation:
        return "Busstation";
    case Shop:
        return "Shop";
    case Workshop:
        return "Workshop";
    case Museum:
        return "Museum";
    case Landmark:
        return "Landmark";
    case Monument:
        return "Monument";
    case Garden:
        return "Garden";
    default:
        return "Unknown";
    }
}

optional<BUILDING_KIND> string_to_building_kind(const string &kind_str)
{
    static const unordered_map<string, BUILDING_KIND> kind_map = {
        {"Hotel", Hotel},
        {"Canteen", Canteen},
        {"Restaurant", Restaurant},
        {"Drugstore", Drugstore},
        {"Hospital", Hospital},
        {"Toilet", Toilet},
        {"Trainstation", Trainstation},
        {"Busstation", Busstation},
        {"Shop", Shop},
        {"Workshop", Workshop},
        {"Museum", Museum},
        {"Landmark", Landmark},
        {"Monument", Monument},
        {"Garden", Garden}};

    auto it = kind_map.find(kind_str);
    if (it != kind_map.end())
    {
        return it->second;
    }
    else
    {
        return nullopt;
    }
}

class building
{
private:
    int x;
    int y;
    BUILDING_KIND building_kind;
    string building_name;

public:
    string building_id;
    building() : x(rand() % 1500), y(rand() % 1500)
    {
        building_kind = static_cast<BUILDING_KIND>(rand() % 14);
    }

    double building_distance(const building &other) const
    {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }

    void initiate_building_id(int u)
    {
        building_id = "Building" + to_string(u);
    }

    void print() const
    {
        cout << building_id << ": " << endl;
        cout << "location: (" << x << "," << y << ")" << endl;
        cout << building_name << " - " << building_kind_to_string(building_kind) << endl;
        cout << endl;
    }

    building(const building &other)
        : x(other.x), y(other.y), building_name(other.building_name),
          building_id(other.building_id), building_kind(other.building_kind) {}

    building &operator=(const building &other)
    {
        if (this == &other)
        {
            return *this;
        }

        x = other.x;
        y = other.y;
        building_name = other.building_name;
        building_id = other.building_id;
        building_kind = other.building_kind;

        return *this;
    }

    bool operator==(const building &other) const
    {
        return x == other.x &&
               y == other.y &&
               building_kind == other.building_kind &&
               building_id == other.building_id &&
               building_name == other.building_name;
    }

    int get_x() const { return x; }
    int get_y() const { return y; }
    string get_building_id() const { return building_id; }
    string get_building_name() const { return building_name; }
    BUILDING_KIND get_building_kind() const { return building_kind; }

    void set_x(int _x) { x = _x; }
    void set_y(int _y) { y = _y; }
    void set_building_id(const string &ID) { building_id = ID; }
    void set_building_name(const string &NAME) { building_name = NAME; }
    void set_building_kind(int kind) { building_kind = static_cast<BUILDING_KIND>(kind); }

    friend class road;
    friend class graph;
};

class road
{
private:
    double crowding;
    double road_distance;
    string road_id;

public:
    building *start;
    building *end;

    road() : crowding(0.0), road_distance(0.0), start(nullptr), end(nullptr) {}

    // Parameterized constructor
    road(double crowding, double distance, const string &id, building *start, building *end)
        : crowding(crowding), road_distance(distance), road_id(id), start(start), end(end) {}

    road(building *start, building *end) : start(start), end(end) {}

    void initiate_road_id(int u)
    {
        road_id = "Road" + to_string(u);
    }

    void print() const
    {
        // cout << road_id << " Distance: " << road_distance << endl;
        start->print();
        end->print();
    }

    double get_road_crowding() const { return crowding; }
    double get_road_distance() const { return road_distance; }
    string get_road_id() const { return road_id; }

    void set_road_start(building *_start) { start = _start; }
    void set_road_end(building *_end) { end = _end; }
    void set_road_crowding(double _crowding) { crowding = _crowding; }
    void set_road_distance(double _distance) { road_distance = _distance; }
    void set_road_id(string _id) { road_id = _id; }

    road &operator=(const road &other)
    {
        if (this == &other)
        {
            return *this; // Prevent self-assignment
        }

        crowding = other.crowding;
        road_distance = other.road_distance;
        road_id = other.road_id;

        delete start;
        delete end;

        if (other.start)
        {
            start = new building(*other.start);
        }
        else
        {
            start = nullptr;
        }

        if (other.end)
        {
            end = new building(*other.end);
        }
        else
        {
            end = nullptr;
        }

        return *this;
    }

    road(const road &other)
        : crowding(other.crowding), road_distance(other.road_distance), road_id(other.road_id)
    {
        if (other.start)
        {
            start = new building(*other.start);
        }
        else
        {
            start = nullptr;
        }

        if (other.end)
        {
            end = new building(*other.end);
        }
        else
        {
            end = nullptr;
        }
    }

    friend class graph;
};

class graph
{
private:
    int graph_id;
    string graph_name;
    string graph_kind;
    string description;
    double graph_heat;
    double graph_score;

public:
    vector<building> buildings;
    vector<road> roads;

    graph() {}

    graph(int building_num, int road_num)
    {
        for (int i = 0; i < building_num; i++)
        {
            buildings.push_back(building());
        }

        vector<bool> visited(building_num, false);
        vector<int> stack; // 记录道路起点
        stack.push_back(0);
        visited[0] = true;

        /* 生成连通图 */
        while (!stack.empty())
        {
            int u = stack.back();
            stack.pop_back(); // start

            vector<int> unvisited_neighbors;

            for (int v = 0; v < building_num; v++)
            {
                if (!visited[v])
                {
                    unvisited_neighbors.push_back(v);
                }
            }

            if (!unvisited_neighbors.empty())
            {
                int v = unvisited_neighbors[rand() % unvisited_neighbors.size()]; // end
                roads.push_back(road(&buildings[u], &buildings[v]));
                stack.push_back(v);
                visited[v] = true;
            }
        }

        /* 生成其他道路 */
        while (roads.size() < road_num)
        {
            int u = rand() % building_num;
            int v = rand() % building_num;
            if (u != v && find_if(roads.begin(), roads.end(), [&](road &r)
                                  { return (r.start == &buildings[u] && r.end == &buildings[v]) || (r.start == &buildings[v] && r.end == &buildings[u]); }) == roads.end())
            {
                roads.push_back(road(&buildings[u], &buildings[v]));
            }
        }

        for (int i = 0; i < roads.size(); i++)
        {
            roads[i].initiate_road_id(i);
        }

        for (int i = 0; i < buildings.size(); i++)
        {
            buildings[i].initiate_building_id(i);
        }
    }

    void print_graph() const
    {
        cout << "graph_heat: " << graph_heat << endl;
        cout << "graph_name: " << graph_name << endl;
        cout << "graph_score: " << graph_score << endl;
        cout << "description: " << description << endl;
    }

    void print_building() const
    {
        cout << "Total buildings: " << buildings.size() << endl;
        for (const auto &b : buildings)
        {
            b.print();
        }
    }

    void print_road() const
    {
        cout << "Total roads: " << roads.size() << endl;
        for (const auto &r : roads)
        {
            r.print();
        }
    }

    void get_closest_buildings(const string &building_id)
    {
        const building *target_building = nullptr;

        for (const auto &building : buildings)
        {
            if (building.get_building_id() == building_id)
            {
                target_building = &building;
                break;
            }
        }

        if (!target_building)
        {
            cout << "Building not found: " << building_id << endl;
            return;
        }

        vector<pair<double, string>> distances;
        for (const auto &building : buildings)
        {
            if (building.get_building_id() != building_id)
            {
                double dist = target_building->building_distance(building);
                distances.emplace_back(dist, building.get_building_id());
            }
        }

        sort(distances.begin(), distances.end(), [](const auto &lhs, const auto &rhs)
             { return lhs.first < rhs.first; });

        cout << "The 10 closest buildings to " << building_id << " are:" << endl;
        for (size_t i = 0; i < min(distances.size(), size_t(10)); ++i)
        {
            cout << distances[i].second << " at distance " << distances[i].first << endl;
        }
        cout << endl;
    }

    void get_closest_buildings_by_kinds(const string &building_id, const vector<BUILDING_KIND> &kinds)
    {
        vector<pair<double, string>> filteredBuildings;
        const building *target_building = nullptr;

        // 找到目标建筑
        for (const auto &b : buildings)
        {
            if (b.get_building_id() == building_id)
            {
                target_building = &b;
                break;
            }
        }

        if (!target_building)
        {
            cout << "Building not found: " << building_id << endl;
            return;
        }

        // 过滤和计算距离
        for (const auto &b : buildings)
        {
            if (find(kinds.begin(), kinds.end(), b.get_building_kind()) != kinds.end())
            {
                double dist = target_building->building_distance(b);
                filteredBuildings.emplace_back(dist, b.get_building_id());
            }
        }

        // 按距离排序
        sort(filteredBuildings.begin(), filteredBuildings.end());

        // 输出结果
        cout << "Buildings of specified kinds from building_id (" << target_building->get_x() << ", " << target_building->get_y() << ") are:" << endl;
        for (const auto &pair : filteredBuildings)
        {
            cout << pair.second << " at distance " << pair.first << endl;
        }
    }

    graph &operator=(const graph &other)
    {
        if (this == &other)
        {
            return *this;
        }
        buildings = other.buildings;
        roads = other.roads;
        graph_id = other.graph_id;
        graph_heat = other.graph_heat;
        graph_score = other.graph_score;
        graph_name = other.graph_name;
        description = other.description;

        return *this;
    }

    void set_graph_heat(double HEAT) { graph_heat = HEAT; }
    void set_graph_score(double SCORE) { graph_score = SCORE; }
    void set_graph_name(string NAME) { graph_name = NAME; }
    void set_graph_description(string DESCRIPTION) { description = DESCRIPTION; }
    void set_graph_kind(string KIND) { graph_kind = KIND; }
    void set_graph_id(int ID) { graph_id = ID; }

    int get_graph_id() const { return graph_id; }
};
