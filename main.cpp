#include "include/user.h"
#include "include/mysql.h"
#include "include/diary.h"

string target_attraction;

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
            g.export_to_json("graph_data.json");
            system("python3 draw_graph.py graph_data.json &");
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
        cout << "1.Route Plan By Length" << endl;
        cout << "2.Display All Buildings" << endl;
        cout << "3.Display Surround Buildings" << endl;
        cout << "4.Display Specific Buildings" << endl;
        cout << "5.Exit" << endl;
        cin >> user_choice;

        switch (user_choice)
        {
        case 1:
        {
            g.formMatrix();
            int start_id, end_id;
            int num_midpoints, kind;
            int useCrowding;
            vector<int> mid_ids;
            string location;

            cout << "Please Enter Start ID: ";
            cin >> start_id;
            cout << "Please Enter End ID: ";
            cin >> end_id;
            cout << "Please Enter Num of Midpoints: ";
            cin >> num_midpoints;
            mid_ids.resize(num_midpoints);

            for (int i = 0; i < num_midpoints; ++i)
            {
                cout << "Please Enter the Midpoint ID of " << i + 1 << ": ";
                cin >> mid_ids[i];
            }

            vector<building> mid;
            for (int id : mid_ids)
            {
                mid.push_back(g.buildings[id]);
            }

            cout << "1.Shortest Time strategy " << endl;
            cout << "2.Shortest Distance strategy " << endl;
            cout << "Please Choose Strategy: ";
            cin >> useCrowding;

            if (useCrowding == 1)
            {
                cout << "Shortest Path with Mid Points:" << endl;
                g.findFastestPathWithMid(g.buildings[start_id], g.buildings[end_id], mid);
                g.printShortestPathBFS();

                cout << "\nShortest Path without Mid Points:" << endl;
                g.findFastestPath(g.buildings[start_id], g.buildings[end_id]);
                g.printShortestPathBFS();
            }
            else if (useCrowding == 2)
            {
                cout << "Shortest Path with Mid Points:" << endl;
                g.findShortestPathWithMid(g.buildings[start_id], g.buildings[end_id], mid);
                g.printShortestPathBFS();

                cout << "\nShortest Path without Mid Points:" << endl;
                g.findShortestPath(g.buildings[start_id], g.buildings[end_id]);
                g.printShortestPathBFS();
            }
            else
            {
                cout << "Invalid Input" << endl;
                cin.clear(); // 清除错误标志
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            break;
        }

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

void diary_pattern()
{
    string filename = "diaries.dat";
    createFileIfNotExists(filename);

    DiaryManager manager;
    manager.loadDiaries(filename); // Load diaries by default

    ifstream inFile(filename, ios::binary);
    size_t diaryCount;
    inFile.read(reinterpret_cast<char *>(&diaryCount), sizeof(diaryCount));
    inFile.close();

    if (diaryCount == 0)
    {
        cout << "No diaries found. Please add your first diary." << endl;
        manager.addDiary();
        manager.saveDiaries(filename); // Save the new diary
    }

    int choice;

    for (;;)
    {
        cout << "1. Add Diary\n2. View Diary Count\n3. Browse All Diaries\n4. View Diary\n5. Rate Diary\n6. Recommend Diaries\n7. Search Diaries by Destination\n8. Exit\nEnter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
            manager.addDiary();
            break;
        case 2:
            manager.displayDiaryCount();
            break;
        case 3:
            manager.displayAllDiaries();
            break;
        case 4:
        {
            string title;
            cout << "Enter diary title: ";
            getline(cin, title);
            manager.viewDiary(title);
            break;
        }
        case 5:
        {
            string title;
            int rating;
            cout << "Enter diary title: ";
            getline(cin, title);
            cout << "Enter rating (1-5): ";
            cin >> rating;
            cin.ignore();
            manager.rateDiary(title, rating);
            break;
        }
        case 6:
            manager.recommendDiaries();
            break;
        case 7:
        {
            string destination;
            cout << "Enter destination: ";
            getline(cin, destination);
            manager.searchDiariesByDestination(destination);
            break;
        }
        case 8:
        {
            return;
        }
        default:
        {
            cout << "Invalid Input" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        }

        manager.saveDiaries(filename); // Save diaries by default
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
        cout << "2.Diary Pattern" << endl;
        cout << "3.Exit" << endl;
        cin >> user_choice;

        switch (user_choice)
        {
        case 1:
            attraction_pattern();
            break;
        case 2:
            diary_pattern();
            break;
        case 3:
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
