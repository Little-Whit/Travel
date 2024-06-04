#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <functional>
using namespace std;
/* 构建next数组 */ 
vector<int> buildNext(const string &pattern)
{
    int m = pattern.length();
    vector<int> next(m, 0);
    int i = 1, j = 0;
    while (i < m)
    {
        if (pattern[i] == pattern[j])
        {
            next[i] = j + 1;
            i++;
            j++;
        }
        else
        {
            if (j > 0)
            {
                j = next[j - 1];
            }
            else
            {
                next[i] = 0;
                i++;
            }
        }
    }
    return next;
}

/* 使用KMP算法进行全文检索 */
vector<int> kmpSearch(const string &text, const string &pattern)
{
    vector<int> result;
    int n = text.length();
    int m = pattern.length();
    vector<int> next = buildNext(pattern);
    int i = 0, j = 0;
    while (i < n)
    {
        if (text[i] == pattern[j])
        {
            i++;
            j++;
            if (j == m)
            {
                result.push_back(i - j);
                j = next[j - 1];
            }
        }
        else
        {
            if (j > 0)
            {
                j = next[j - 1];
            }
            else
            {
                i++;
            }
        }
    }
    return result;
}

class Diary
{
private:
    string name;
    string fileAddress;
    string destination;
    string content;
    int score;
    int viewCount;

public:
    Diary(const string &name, const string &fileAddress, const string &destination, const string &content)
        : name(name), fileAddress(fileAddress), destination(destination), content(content), score(0), viewCount(0) {}

    string getName() const
    {
        return name;
    }

    string getFileAddress() const
    {
        return fileAddress;
    }

    string getDestination() const
    {
        return destination;
    }

    string getContent() const
    {
        return content;
    }

    int getScore() const
    {
        return score;
    }

    int getViewCount() const
    {
        return viewCount;
    }

    void setScore(int newScore)
    {
        score = newScore;
    }

    void increaseViewCount()
    {
        viewCount++;
    }

    void writeContentToFile()
    {
        ofstream file(fileAddress);
        if (static_cast<bool>(file.is_open()))
        {
            file << content;
            file.close();
        }
        else
        {
            cout << "Unable to write content to file: " << fileAddress << endl;
        }
    }

    void readContentFromFile()
    {
        ifstream file(fileAddress);
        if (static_cast<bool>(file.is_open()))
        {
            string line;
            while (getline(file, line))
            {
                content += line + "\n";
            }
            file.close();
        }
        else
        {
            cout << "Unable to read content from file: " << fileAddress << endl;
        }
    }
};

class DiaryManager
{
private:
    vector<Diary> diaries;

public:
    string fileAddress;
    void createDiary(const string &name, const string &destination, const string &content)
    {
        string fileAddress = name + ".txt";
        Diary diary(name, fileAddress, destination, content);
        diary.writeContentToFile();
        diaries.push_back(diary);
    }

    void evaluateDiary(const string &name, int newScore)
    {
        for (Diary &diary : diaries)
        {
            if (diary.getName() == name)
            {
                diary.setScore((diary.getScore() + newScore) / 2);
                break;
            }
        }
        sortDiaries();
    }

    void increaseViewCount(const string &name)
    {
        for (Diary &diary : diaries)
        {
            if (diary.getName() == name)
            {
                diary.increaseViewCount();
                break;
            }
        }
        sortDiaries();
    }

    void sortDiaries()
    {
        sort(diaries.begin(), diaries.end(), [](const Diary &d1, const Diary &d2)
             { return d1.getScore() > d2.getScore(); });
    }

    void searchDiaries(const string &pattern)
    {
        for (const Diary &diary : diaries)
        {
            vector<int> matches = kmpSearch(diary.getContent(), pattern);
            if (!matches.empty())
            {
                cout << "Pattern found in diary: " << diary.getName() << endl;
                for (int matchIndex : matches)
                {
                    cout << "Match found at index: " << matchIndex << endl;
                }
            }
        }
    }

    void rankByTotalScoreAndViews()
    {
        sort(diaries.begin(), diaries.end(), [](const Diary &d1, const Diary &d2)
             {
            int totalScore1 = d1.getScore() + d1.getViewCount();
            int totalScore2 = d2.getScore() + d2.getViewCount();
            return totalScore1 > totalScore2; });

        cout << "Ranking by total score and views:" << endl;
        for (int i = 0; i < min(10, static_cast<int>(diaries.size())); i++)
        {
            const Diary &diary = diaries[i];
            int totalScore = diary.getScore() + diary.getViewCount();
            cout << i + 1 << ". " << diary.getName() << " - Total Score: " << totalScore << endl;
        }
    }

    void rankByAddressScoreAndViews()
    {
        sort(diaries.begin(), diaries.end(), [](const Diary &d1, const Diary &d2)
             {
            int addressScore1 = d1.getDestination().length() + d1.getScore() + d1.getViewCount();
            int addressScore2 = d2.getDestination().length() + d2.getScore() + d2.getViewCount();
            return addressScore1 > addressScore2; });

        cout << "Ranking by address score and views:" << endl;
        for (int i = 0; i < min(10, static_cast<int>(diaries.size())); i++)
        {
            const Diary &diary = diaries[i];
            int addressScore = diary.getDestination().length() + diary.getScore() + diary.getViewCount();
            cout << i + 1 << ". " << diary.getName() << " - Address Score: " << addressScore << endl;
        }
    }

    void rankByAddressScoreAndViews(const string &address)
    {
        vector<Diary> diariesWithAddress;
        for (const Diary &diary : diaries)
        {
            if (diary.getDestination() == address)
            {
                diariesWithAddress.push_back(diary);
            }
        }

        sort(diariesWithAddress.begin(), diariesWithAddress.end(), [](const Diary &d1, const Diary &d2)
             {
            int addressScore1 = d1.getScore() + d1.getViewCount();
            int addressScore2 = d2.getScore() + d2.getViewCount();
            return addressScore1 > addressScore2; });

        cout << "Ranking by address score and views for address " << address << ":" << endl;
        for (int i = 0; i < min(10, static_cast<int>(diariesWithAddress.size())); i++)
        {
            const Diary &diary = diariesWithAddress[i];
            int addressScore = diary.getScore() + diary.getViewCount();
            cout << i + 1 << ". " << diary.getName() << " - Address Score: " << addressScore << endl;
        }
    }

    string searchDiaryByName(const string &name)
    {
        for (const Diary &diary : diaries)
        {
            if (diary.getName() == name)
            {
                return diary.getFileAddress();
            }
        }
        return ""; // 如果找不到匹配的日记，返回空字符串
    }
    void saveDiariesToFile()
    {
        ofstream file(fileAddress);
        if (file.is_open())
        {
            for (const Diary &diary : diaries)
            {
                file << diary.getName() << "," << diary.getFileAddress() << "," << diary.getDestination() << "," << diary.getScore() << "," << diary.getViewCount() << "\n";
            }
            file.close();
            cout << "Diaries saved to file: " << fileAddress << endl;
        }
        else
        {
            cout << "Unable to save diaries to file: " << fileAddress << endl;
        }
    }

    void loadDiariesFromFile()
    {
        ifstream file(fileAddress);
        if (file.is_open())
        {
            diaries.clear(); // 清空当前的日记数组

            string line;
            while (getline(file, line))
            {
                stringstream ss(line); // 初始化stringstream
                string name, fileAddress, destination, scoreStr, viewCountStr;
                getline(ss, name, ',');
                getline(ss, fileAddress, ',');
                getline(ss, destination, ',');
                getline(ss, scoreStr, ',');
                getline(ss, viewCountStr, ',');

                int score = stoi(scoreStr);
                int viewCount = stoi(viewCountStr);

                Diary diary(name, fileAddress, destination, "");
                diary.setScore(score);
                diary.increaseViewCount();

                diaries.push_back(diary);
            }

            file.close();
            cout << "Diaries loaded from file: " << fileAddress << endl;
        }
        else
        {
            cout << "Unable to load diaries from file: " << fileAddress << endl;
        }
    }
    void saveDiariesOnExit(DiaryManager &diaryManager)
    {
        diaryManager.saveDiariesToFile();
    }
};

void saveDiariesOnExit(DiaryManager &diaryManager)
{
    diaryManager.saveDiariesToFile();
}

int main()
{
    DiaryManager diaryManager;
    diaryManager.fileAddress = "diaries.txt"; // 设置文件地址

    diaryManager.loadDiariesFromFile(); // 加载之前保存的日记信息

    // 检查文件是否存在，如果不存在则创建文件
    ifstream file(diaryManager.fileAddress);
    if (!file.good())
    {
        ofstream createFile(diaryManager.fileAddress);
        createFile.close();
    }
    file.close();

    while (true)
    {
        cout << "1. Create Diary" << endl;
        cout << "2. Evaluate Diary" << endl;
        cout << "3. Search Diaries" << endl;
        cout << "4. Rank Diaries by Total Score and Views" << endl;
        cout << "5. Rank Diaries by Address Score and Views for Specific Address" << endl;
        cout << "6.Enter diary name to search: " << endl;
        cout << "7. Exit" << endl;

        int choice;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            string name, destination, content;
            cout << "Enter diary name: ";
            cin >> name;
            cout << "Enter destination: ";
            cin >> destination;
            cout << "Enter content: ";
            cin.ignore();
            getline(cin, content);

            diaryManager.createDiary(name, destination, content);
            cout << "Diary created successfully!" << endl;
        }
        else if (choice == 2)
        {
            string name;
            int newScore;
            cout << "Enter diary name: ";
            cin >> name;
            cout << "Enter new score: ";
            cin >> newScore;

            diaryManager.evaluateDiary(name, newScore);
            cout << "Diary evaluated successfully!" << endl;
        }
        else if (choice == 3)
        {
            string pattern;
            cout << "Enter search pattern: ";
            cin.ignore();
            getline(cin, pattern);

            diaryManager.searchDiaries(pattern);
        }
        else if (choice == 4)
        {
            diaryManager.rankByTotalScoreAndViews();
        }
        else if (choice == 5)
        {
            string address;
            cout << "Enter address: ";
            cin >> address;

            diaryManager.rankByAddressScoreAndViews(address);
        }
        else if (choice == 6)
        {
            string name;
            cout << "Enter diary name to search: ";
            cin >> name;
            string fileAddress = diaryManager.searchDiaryByName(name);
            if (!fileAddress.empty())
            {
                cout << "File address for diary " << name << ": " << fileAddress << endl;
            }
            else
            {
                cout << "Diary not found!" << endl;
            }
        }
        else if (choice == 7)
        {
            break;
        }
        else
        {
            cin.clear();
            cin.ignore();
            cout << "Invalid choice! Please try again." << endl;
        }
    }

    function<void()> saveDiariesFunc = [&diaryManager]()
    { saveDiariesOnExit(diaryManager); };                                       // 使用function包装lambda函数
    // atexit(reinterpret_cast<void (*)()>(saveDiariesFunc.target<void (*)()>())); // 使用reinterpret_cast进行函数指针类型转换
    return 0;
}