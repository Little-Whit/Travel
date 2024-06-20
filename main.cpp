#include "include/user.h"
#include "include/mysql.h"

void home_page()
{
    int user_choice;
    for (;;)
    {
        int user_choice;
        cout << "1.Sign Up" << endl;
        cout << "2.Log In" << endl;
        cin >> user_choice;

        switch (user_choice)
        {
        case 1:
            user_signup();
            break;
        case 2:
        {
            if (user_login())
                return;
            else
                break;
        }
        default:
        {
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid Input" << endl;
            break;
        }
        }
    }
}

void attraction_pattern()
{
    graph g;
    int user_choice;
    bool exitLoop = false;
    while (!exitLoop)
    {
        cout << "1.Sort By Heat" << endl;
        cout << "2.Sort By Score" << endl;
        cout << "3.Comprehensive Sort" << endl;
        cout << "4.Search By Kind" << endl;
        cout << "5.Search By Name" << endl;
        cout << "6.Choose Attraction" << endl;
        cout << "7.Exit" << endl;

        cin >> user_choice;
        string target;

        switch (user_choice)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            order_graph(user_choice);
            break;
        case 6:
        {
            cout << "Please Input The Graph Name:" << endl;
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, target);
            g = get_graph_from_database(target);
            if (g.get_graph_id() == 0)
            {
                cout << "This graph doesn't exit" << endl;
                break;
            }
            else
            {
                exitLoop = true;
                break;
            }
        }
        case 7:
            return;
        default:
        {
            cout << "Invalid Input" << endl;
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        }
    }

    for (;;)
    {
        cout << "1.Route Plan " << endl;
        cout << "2.Display All Buildings" << endl;
        cout << "3.Display Surround Buildings" << endl;
        cout << "4.Display Specific Buildings" << endl;
        cout << "5.Exit" << endl;
        cin >> user_choice;

        switch (user_choice)
        {
        case 1:
            cout << "还没做" << endl;
            break;

        case 2:
            g.print_building();
            break;

        case 3:
        {
            int location;
            cout << "Please Enter Your Location:" << endl;
            cin >> location;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Input" << endl;
            }
            else
            {
                g.get_closest_buildings(g.buildings[location].get_building_id());
            }
            break;
        }

        case 4:
        {
            int location;
            string kind;
            BUILDING_KIND building_kind;
            vector<BUILDING_KIND> kinds;
            int filter_choice = 0;
            bool exitLoop = false;

            // 读取location
            cout << "Please Enter Your Location:" << endl;
            cin >> location;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Input" << endl;
            }
            else
            {
                // 读取kind
                while (!exitLoop)
                {
                    cout << "1.Input The Kind" << endl;
                    cout << "2.Quit" << endl;
                    cin >> filter_choice;
                    switch (filter_choice)
                    {
                    case 1:
                    {
                        cout << "Please Enter Building Kind:" << endl;
                        cin >> kind;
                        optional _kind = string_to_building_kind(kind);
                        if (_kind.has_value())
                        {
                            kinds.push_back(_kind.value());
                        }
                        else
                        {
                            cout << "Invalid building kind: " << kind << endl;
                            // 其他处理逻辑，例如打印错误信息或执行其他操作
                        }
                        break;
                    }
                    case 2:
                        exitLoop = true;
                        break;

                    default:
                    {
                        cin.clear(); // 清除错误标志
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid Input" << endl;
                        break;
                    }
                    }
                }
                g.get_closest_buildings_by_kinds(g.buildings[location].get_building_id(), kinds);
            }
            break;
        }

        case 5:
            return;

        default:
        {
            cout << "Invalid Input" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        }
    }
}

int main()
{
    int user_choice;
    srand(static_cast<unsigned>(time(nullptr)));
    home_page();

    for (;;)
    {
        cout << "Please Choose Pattern:" << endl;
        cout << "1.Attraction Pattern" << endl;
        cout << "2.Exit" << endl;
        cin >> user_choice;

        switch (user_choice)
        {
        case 1:
            attraction_pattern();
            break;
        case 2:
            return 0;
        default:
        {
            cout << "Invalid Input" << endl;
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        }
    }

    return 0;
}
