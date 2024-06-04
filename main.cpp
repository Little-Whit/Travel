#include "graph.h"
#include "diary.h"
// #include "mysql.cpp"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    vector<building> buildings;
    vector<road> roads;

    int building_num;
    int road_num;

    cin >> building_num;
    cin >> road_num;

    graph g(building_num,road_num);
    g.print();


    return 0;
}
