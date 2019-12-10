#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
using namespace std;

int main(){
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    unordered_map<string,int> dictionary;
    string data;
    while(!fin.eof()){
        fin >> data;
        if(!isalpha(data[data.length() - 1]))
            data = data.substr(0,data.length()-1);
        for(int i = 0 ; i < data.length(); i++)
            data[i] = tolower(data[i]);
        auto it = dictionary.find(data);
        if(it != dictionary.end())
            it->second++;
        else
            dictionary.insert(make_pair(data,1));
    }
    int max,min;
    max = 0;
    for(auto i : dictionary){
        if(i.second > max)
            max = i.second;
    }
    min = max;
    for(auto i : dictionary){
        if(i.second < min)
            min = i.second;
    }
    fout << dictionary.size() << " " << max << " " << min;

}