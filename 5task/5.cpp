//
// Created by markd on 13.12.2019.
//

#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

using namespace std;

string downloadRepository = "C:\\crawler\\";

class Crawler {
public:
    Crawler(int count, string input) : countThreads(count) {
        toProcessed.push(input);
        processed.insert(input);
    }

    void ThreadNewFileAdd (string href){
        lock_guard<mutex> lockGuard(contLock);

        if(processed.find(href) == processed.end())
            toProcessed.push(href);
    }

    static void ThreadCopyFile(string filename) {
        ifstream in(filename);
        string outputFile = downloadRepository + filename;
        ofstream out(outputFile);
        string input;
        out << in.rdbuf();
        in.close();
        out.close();
    }

    void ThreadAddDoneFile(string filename) {
        lock_guard<mutex> lockGuard(contLock);
        processed.insert(filename);
    }
    string ThreadGetNewFile() {
        lock_guard<mutex> guard(contLock);
        if(!toProcessed.empty()) {
            string nextFile = toProcessed.front();
            toProcessed.pop();
            return nextFile;
        }
        else
            return "-0";
    }

    void ThreadProcessFile(string file) {
        try {
            ifstream in(file);
            ThreadCopyFile(file);
            string line;
            char c;
            vector<string> hrefs;

            while (in) {
                line.clear();
                c = in.get();
                if(c == '<' && in.get() == 'a' ) {
                    while(c != '\"') {
                        c = in.get();
                    }
                    c = in.get();
                    while(c != '\"') {
                        line += c;
                        c = in.get();
                    }
//                    cout << line << endl;
                    hrefs.push_back(line.substr(7));
                }
                // line = "file://...."
            }
            for(auto i : hrefs) {
                ThreadNewFileAdd(i);
            }
            ThreadAddDoneFile(file);
        }
        catch (...) { }
    }

    void ThreadProgram () {
        while (!toProcessed.empty() || workingThreads != 0) {
            if (!toProcessed.empty()) {
                string file = ThreadGetNewFile();
                if (file != "-0") {
                    workingThreads++;
                    ThreadProcessFile(file);
                    workingThreads--;
                }
            }
            cout << toProcessed.size() << " " << workingThreads << endl;
        }
    }

    pair<int, int> crawl() {
        pair<int, int> result;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();

        for(int i = 0; i < countThreads; ++i){
            threads.push_back(thread(&Crawler::ThreadProgram, ref(*this)));
        }
        for(int i = 0; i < countThreads; ++i) {
            threads.at(i).join();
        }

        end = chrono::system_clock::now();
        int time = chrono::duration_cast<chrono::microseconds>(end - start).count();
        result.second = time;
        result.first = processed.size();

        return result;
    }


private:
    string startPath;
    int countThreads;
    vector<thread> threads;
    queue<string> toProcessed;
    set<string> processed;
    mutex contLock;
    atomic<int> workingThreads;
};

int main(){
    ifstream fin("input.txt");
    string inp;
    int numOfThreads;
    fin >> inp >> numOfThreads;
    fin.close();

    Crawler crawler(numOfThreads, inp);
    pair<int, int> result = crawler.crawl();
    ofstream out("output.txt");
    out << result.first << " " << result.second << " microsec";
    out.close();
    return 0;
}
