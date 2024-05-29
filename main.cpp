#include "graph.h"
#include "mysql.h"
#include "diary.h"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    int usr_choic;

    cout << "please select a part:" << endl;

    cout << "1. travel part" << endl;
    cout << "2. diary part" << endl; 
    cin >> usr_choic;

    graph g = initiate_graph();

    g.print();
    return 0;
}
