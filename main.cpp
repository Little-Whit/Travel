#include "user.h"
#include "mysql.cpp"
#include "diary.cpp"

int user_choice;
string target_attraction;

void home_page()
{
    for (;;)
    {
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
    for (;;)
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
                return;
            }
            else
            {
                g.print_road();
                g.print_building();
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

    int attraction_user_choice;
    for (;;)
    {
        cout << "1.Route Plan By Length" << endl;
        cout << "2.Display All Buildings" << endl;
        cout << "3.Display Surround Buildings" << endl;
        cout << "4.Display Surround Buildings By Specific Kind" << endl;
        cout << "5.Exit" << endl;
        cin >> attraction_user_choice;

        switch (attraction_user_choice)
        {
        case 1:
        {
            g.formMatrix();
            int start_id, end_id;
            int num_midpoints;
            vector<int> mid_ids;
            string location;
            int kind;

            cout << "Please Enter Start ID: ";
            cin >> start_id;
            cout << "Please Enter End ID: ";
            cin >> end_id;
            cout << "Please Enter Num of Midpoints: ";
            cin >> num_midpoints;
            mid_ids.resize(num_midpoints);

            for (int i = 0; i < num_midpoints; ++i)
            {
                cout << "Please Enter the Midpoint ID of" << i + 1;
                cin >> mid_ids[i];
            }

            cout << "Shortest Path with Mid Points:" << endl;
            vector<building> mid;
            for (int id : mid_ids)
            {
                mid.push_back(g.buildings[id]);
            }
            g.findShortestPathWithMid(g.buildings[start_id], g.buildings[end_id], mid);
            g.printShortestPathBFS();

            cout << "\nShortest Path without Mid Points:" << endl;
            g.findShortestPath(g.buildings[start_id], g.buildings[end_id]);
            g.printShortestPathBFS();
            break;
        }
        case 2:
            g.print_building();
            break;
        case 3:
        {
            string location;
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please Enter Your Location:" << endl;
            getline(cin, location);
            g.print_closest_buildings(location);
            break;
        }
        case 4:
        {
            string location;
            int kind;
            BUILDING_KIND building_kind;

            // 读取location
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please Enter Your Location:" << endl;
            getline(cin, location);

            // 读取kind
            while (true)
            {
                cout << "Please Enter Building Kind:" << endl;
                cin.clear(); // 清除错误标志
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please Repeat Building Kind:" << endl;
                cin >> kind;

                if (cin.fail())
                {
                    cout << "Invalid Input";
                    cin.clear();                                         // 清除错误标志
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略输入流中的无效数据
                }
                else
                {
                    break; // 输入有效，退出循环
                }
            }

            building_kind = static_cast<BUILDING_KIND>(kind);
            g.print_closest_buildings_by_kind(location, building_kind);
            break; // 别忘了break语句结束case块
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
    // manager.loadDiaries(filename); // Load diaries by default

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
