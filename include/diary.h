#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>
#include <algorithm>

using namespace std;

struct Diary
{
    string title;
    string content;
    string destination;
    int views;
    vector<int> ratings;

    Diary(string t, string c, string d) : title(t), content(c), destination(d), views(0) {}
};

class DiaryManager
{
    vector<Diary> diaries;

    void buildHuffmanTree(string text, unordered_map<char, string> &huffmanCode)
    {
        unordered_map<char, int> freq;
        for (char ch : text)
        {
            freq[ch]++;
        }

        struct Node
        {
            char ch;
            int freq;
            Node *left, *right;
            Node(char ch, int freq, Node *left = nullptr, Node *right = nullptr)
            {
                this->ch = ch;
                this->freq = freq;
                this->left = left;
                this->right = right;
            }
        };

        struct compare
        {
            bool operator()(Node *left, Node *right)
            {
                return left->freq > right->freq;
            }
        };

        priority_queue<Node *, vector<Node *>, compare> pq;
        for (auto pair : freq)
        {
            pq.push(new Node(pair.first, pair.second));
        }

        while (pq.size() != 1)
        {
            Node *left = pq.top();
            pq.pop();
            Node *right = pq.top();
            pq.pop();
            int sum = left->freq + right->freq;
            pq.push(new Node('\0', sum, left, right));
        }

        Node *root = pq.top();

        std::function<void(Node *, string)> encode = [&](Node *node, string str)
        {
            if (node == nullptr)
                return;
            if (!node->left && !node->right)
            {
                huffmanCode[node->ch] = str;
            }
            encode(node->left, str + "0");
            encode(node->right, str + "1");
        };

        encode(root, "");
    }

    string encodeText(string text, unordered_map<char, string> &huffmanCode)
    {
        string encodedText = "";
        for (char ch : text)
        {
            encodedText += huffmanCode[ch];
        }
        return encodedText;
    }

    string decodeText(string encodedText, unordered_map<string, char> &reverseHuffmanCode)
    {
        string decodedText = "";
        string currentCode = "";
        for (char bit : encodedText)
        {
            currentCode += bit;
            if (reverseHuffmanCode.find(currentCode) != reverseHuffmanCode.end())
            {
                decodedText += reverseHuffmanCode[currentCode];
                currentCode = "";
            }
        }
        return decodedText;
    }

public:
    void addDiary()
    {
        string input, title, content, destination;

        cout << "Enter diary title : ";
        getline(cin, input);
        if (!input.empty())
        {
            title += input + "\n";
        }
        if (!title.empty())
        {
            title = title.substr(0, title.size() - 1); // Remove the last newline character
        }
        if (title.empty())
            return;

        cout << "Enter diary content (type 'exit' when done): ";
        while (true)
        {
            getline(cin, input);
            if (input == "exit")
                break;
            if (!input.empty())
            {
                content += input + "\n";
            }
        }
        if (!content.empty())
        {
            content = content.substr(0, content.size() - 1); // Remove the last newline character
        }
        if (content.empty())
            return;

        cout << "Enter diary destination : ";

        getline(cin, input);

        if (!input.empty())
        {
            destination += input + "\n";
        }
        if (!destination.empty())
        {
            destination = destination.substr(0, destination.size() - 1); // Remove the last newline character
        }
        if (destination.empty())
            return;

        diaries.push_back(Diary(title, content, destination));
    }

    void saveDiaries(string filename)
    {
        ofstream outFile(filename, ios::binary);
        if (outFile.is_open())
        {
            size_t diaryCount = diaries.size();
            outFile.write(reinterpret_cast<const char *>(&diaryCount), sizeof(diaryCount));

            for (const auto &diary : diaries)
            {
                size_t titleSize = diary.title.size();
                size_t destinationSize = diary.destination.size();

                outFile.write(reinterpret_cast<const char *>(&titleSize), sizeof(titleSize));
                outFile.write(diary.title.c_str(), titleSize);

                outFile.write(reinterpret_cast<const char *>(&destinationSize), sizeof(destinationSize));
                outFile.write(diary.destination.c_str(), destinationSize);

                unordered_map<char, string> huffmanCode;
                buildHuffmanTree(diary.content, huffmanCode);
                string encodedContent = encodeText(diary.content, huffmanCode);

                size_t mapSize = huffmanCode.size();
                outFile.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));

                for (auto pair : huffmanCode)
                {
                    outFile.write(reinterpret_cast<const char *>(&pair.first), sizeof(pair.first));
                    size_t codeSize = pair.second.size();
                    outFile.write(reinterpret_cast<const char *>(&codeSize), sizeof(codeSize));
                    outFile.write(pair.second.c_str(), codeSize);
                }

                size_t encodedSize = encodedContent.size();
                outFile.write(reinterpret_cast<const char *>(&encodedSize), sizeof(encodedSize));
                outFile.write(encodedContent.c_str(), encodedSize);

                outFile.write(reinterpret_cast<const char *>(&diary.views), sizeof(diary.views));

                size_t ratingsSize = diary.ratings.size();
                outFile.write(reinterpret_cast<const char *>(&ratingsSize), sizeof(ratingsSize));
                for (int rating : diary.ratings)
                {
                    outFile.write(reinterpret_cast<const char *>(&rating), sizeof(rating));
                }
            }

            outFile.close();
        }
        else
        {
            cerr << "Unable to open file for writing." << endl;
        }
    }

    void loadDiaries(string filename)
    {
        ifstream inFile(filename, ios::binary);
        if (!inFile.is_open())
        {
            cerr << "Unable to open file for reading." << endl;
            return;
        }

        size_t diaryCount;
        inFile.read(reinterpret_cast<char *>(&diaryCount), sizeof(diaryCount));

        for (size_t i = 0; i < diaryCount; ++i)
        {
            size_t titleSize;
            inFile.read(reinterpret_cast<char *>(&titleSize), sizeof(titleSize));
            string title(titleSize, ' ');
            inFile.read(&title[0], titleSize);

            size_t destinationSize;
            inFile.read(reinterpret_cast<char *>(&destinationSize), sizeof(destinationSize));
            string destination(destinationSize, ' ');
            inFile.read(&destination[0], destinationSize);

            size_t mapSize;
            inFile.read(reinterpret_cast<char *>(&mapSize), sizeof(mapSize));

            unordered_map<string, char> reverseHuffmanCode;
            char ch;
            for (size_t j = 0; j < mapSize; ++j)
            {
                inFile.read(reinterpret_cast<char *>(&ch), sizeof(ch));
                size_t codeSize;
                inFile.read(reinterpret_cast<char *>(&codeSize), sizeof(codeSize));
                string code(codeSize, ' ');
                inFile.read(&code[0], codeSize);
                reverseHuffmanCode[code] = ch;
            }

            size_t encodedSize;
            inFile.read(reinterpret_cast<char *>(&encodedSize), sizeof(encodedSize));

            string encodedText(encodedSize, ' ');
            inFile.read(&encodedText[0], encodedSize);

            string content = decodeText(encodedText, reverseHuffmanCode);

            int views;
            inFile.read(reinterpret_cast<char *>(&views), sizeof(views));

            size_t ratingsSize;
            inFile.read(reinterpret_cast<char *>(&ratingsSize), sizeof(ratingsSize));
            vector<int> ratings(ratingsSize);
            for (size_t j = 0; j < ratingsSize; ++j)
            {
                inFile.read(reinterpret_cast<char *>(&ratings[j]), sizeof(ratings[j]));
            }

            diaries.push_back(Diary(title, content, destination));
            diaries.back().views = views;
            diaries.back().ratings = ratings;
        }

        inFile.close();
    }

    void displayDiaryCount()
    {
        cout << "Currently there are " << diaries.size() << " travel diaries." << endl;
    }

    void displayAllDiaries()
    {
        for (const auto &diary : diaries)
        {
            cout << "Title: " << diary.title << ", Destination: " << diary.destination << ", Views: " << diary.views << endl;
        }
    }

    void viewDiary(string title)
    {
        for (auto &diary : diaries)
        {
            if (diary.title == title)
            {
                diary.views++;
                cout << "Content: " << diary.content << endl;
                return;
            }
        }
        cout << "Diary with title " << title << " not found." << endl;
    }

    void rateDiary(string title, int rating)
    {
        for (auto &diary : diaries)
        {
            if (diary.title == title)
            {
                diary.ratings.push_back(rating);
                return;
            }
        }
        cout << "Diary with title " << title << " not found." << endl;
    }

    double getAverageRating(const Diary &diary)
    {
        if (diary.ratings.empty())
            return 0;
        double sum = 0;
        for (int rating : diary.ratings)
        {
            sum += rating;
        }
        return sum / diary.ratings.size();
    }

    void recommendDiaries()
    {
        sort(diaries.begin(), diaries.end(), [this](const Diary &a, const Diary &b)
             { return a.views + getAverageRating(a) > b.views + getAverageRating(b); });

        for (const auto &diary : diaries)
        {
            cout << "Title: " << diary.title << ", Destination: " << diary.destination << ", Views: " << diary.views << ", Average Rating: " << getAverageRating(diary) << endl;
        }
    }

    void searchDiariesByDestination(string destination)
    {
        vector<Diary> filteredDiaries;
        for (const auto &diary : diaries)
        {
            if (diary.destination == destination)
            {
                filteredDiaries.push_back(diary);
            }
        }

        sort(filteredDiaries.begin(), filteredDiaries.end(), [this](const Diary &a, const Diary &b)
             { return a.views + getAverageRating(a) > b.views + getAverageRating(b); });

        for (const auto &diary : filteredDiaries)
        {
            cout << "Title: " << diary.title << ", Views: " << diary.views << ", Average Rating: " << getAverageRating(diary) << endl;
        }
    }
};

void createFileIfNotExists(const string &filename)
{
    ifstream inFile(filename);
    if (!inFile.good())
    {
        ofstream outFile(filename);
        if (!outFile.is_open())
        {
            cerr << "Unable to create file: " << filename << endl;
        }
        outFile.close();
    }
}

