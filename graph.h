#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
using namespace std;

enum BUILDING_KIND
{
    visit,
    service1,
    service2,
    service3,
    service4,
    service5,
    service6,
    service7,
    service8,
    service9,
    service10,
    service11,
    service12,
    service13,
};

class building;
class road;
class graph;

class building
{
private:
    int x;
    int y;
    string building_id;
    BUILDING_KIND building_kind;

public:
    building() : x(rand() % 1500), y(rand() % 1500) { building_kind = static_cast<BUILDING_KIND>(rand() % 14); }

    double building_distance(const building &other) const
    {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }

    void building_name(int u)
    {
        building_id = "Building" + to_string(u);
    }

    void print() const
    {
        cout << x << endl;
        cout << y << endl;
    }

    building &operator=(const building &other)
    {
        if (this == &other)
        {
            return *this;
        }
        x = other.x;
        y = other.y;
        building_kind = other.building_kind;
        return *this;
    }

    bool operator==(const building &other) const
    {
       return x == other.x &&
            y == other.y &&
            building_kind == other.building_kind;
    }

    building(const building &other)
    {
        x = other.x;
        y = other.y;
        building_kind = other.building_kind;
    }

    int get_x() const { return x; }
    int get_y() const { return y; }
    string get_building_id() const { return building_id; }
    BUILDING_KIND get_building_kind() const { return building_kind; }

    void set_x(int _x) { x = _x; }
    void set_y(int _y) { y = _y; }
    void set_building_kind(int kind) { building_kind = static_cast<BUILDING_KIND>(kind); }
    void set_building_id(string ID) { building_id = ID; }

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

    road() {}

    road(building *s, building *e) : start(s), end(e)
    {
        road_distance = start->building_distance(*end);
        crowding = rand() % 10;
    }

    void road_name(int u)
    {
        road_id = "Road" + to_string(u);
    }

    bool intersect(const road &other) const
    {
        /* 快速排斥实验 */
        if ((max(start->x, end->x) < min(other.start->x, other.end->x)) ||
            (max(start->y, end->y) < min(other.start->y, other.end->y)) ||
            (min(start->y, end->y) > max(other.start->y, other.end->y)) ||
            (min(start->x, end->x) > max(other.start->x, other.end->x)))
        {
            return false;
        }

        /* 跨交实验 */
        if ((((start->x - other.start->x) * (other.end->y - other.start->y) - (start->y - other.start->y) * (other.end->x - other.start->x)) *
             ((end->x - other.start->x) * (other.end->y - other.start->y) - (end->y - other.start->y) * (other.end->x - other.start->x))) > 0 ||
            (((other.start->x - start->x) * (end->y - start->y) - (other.start->y - start->y) * (end->x - start->x)) *
             ((other.end->x - start->x) * (end->y - start->y) - (other.end->y - start->y) * (end->x - start->x))) > 0)
        {
            return false;
        }
        return true;
    }

    void print() const
    {
        // cout << "Road from (" << start->x << ", " << start->y << ") to ("
        //      << end->x << ", " << end->y << "), Distance: " << road_distance << endl;

        cout << road_id << " Distance: " << road_distance << endl;
    }

    double get_road_crowding() const { return crowding; }
    double get_road_distance() const { return road_distance; }
    string get_road_id() const { return road_id; }

    void set_road_start(building* _start) {start = _start; }
    void set_road_end(building* _end) {end = _end; }
    void set_road_crowding(double _crowding) { crowding = _crowding; }
    void set_road_distance(double _distance) { road_distance = _distance; }
    void set_road_id(string _id) { road_id = _id; }

    road &operator=(const road &other)
    {
        if (this == &other)
        {
            return *this;
        }
        start = other.start;
        end = other.end;
        crowding = other.crowding;
        road_distance = other.road_distance;
        road_id = other.road_id;
        return *this;
    }

    road(const road &other)
    {
        start = other.start;
        end = other.end;
        crowding = other.crowding;
        road_distance = other.road_distance;
        road_id = other.road_id;
    }

    friend class graph;
};

class graph
{
private:
    string graph_id;

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
            roads[i].road_name(i);
        }

        for (int i = 0; i < buildings.size(); i++)
        {
            buildings[i].building_name(i);
        }
    }

    void print() const
    {
        // for (const auto &b : buildings)
        // {
        //     b.print();
        // }
        cout << "Total roads: " << roads.size() << endl;
        for (const auto &r : roads)
        {
            r.print();
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

        return *this;
    }

    bool search_road(string target)
    {
        for (int i = 0; i < roads.size(); i++)
        {
            if (roads[i].get_road_id() == target)
                return true;
        }
        return false;
    }

    bool search_building(string target)
    {
        for (int i = 0; i < buildings.size(); i++)
        {
            if (buildings[i].get_building_id() == target)
                return true;
        }
        return false;
    }
};
