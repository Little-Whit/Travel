#include "park.h"
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    int building_count = 70;
    int edge_count = 200;

    graph g(building_count, edge_count);
    g.print();

    return 0;
}
