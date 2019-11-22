#include <vector>
#include <fstream>
#include <list>
#include <algorithm>
#include <string>
#include <iostream>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

// расчленение
void sokolov(list<string> &listString, string &word){
    if(word[0] == '\"' && word[word.length() - 1] == '\"'){
        string subword = word.substr(1,word.length() - 2);
        listString.push_back("\"");
        listString.push_back(subword);
        listString.push_back("\"");
    }
    else{
        listString.push_back(word);
    }

}

int main(){
    list<pair<list<string>,char>> text;
    pair<list<string>,char> sentence;
    string word;
    char lastChar;

    string word1, word2, word3;
    fin >> word1 >> word2 >> word3;

    while(!fin.eof()){
        fin >> word;
        lastChar = word[word.length() - 1];
//        cout << word << " ";
        if(lastChar == '.' || lastChar == '!' || lastChar == '?'){
            string subword = word.substr(0,word.length()-1);
            sokolov(sentence.first, subword);
            sentence.second = lastChar;
            text.push_back(sentence);
            sentence.first.clear();
            continue;
        }
        sokolov(sentence.first, word);
    }


    for(auto it1 = text.begin(); it1 != text.end(); ++it1){
        for(auto it2 = it1->first.begin(); it2 != it1->first.end(); ++it2)
            if(*it2 == word1) *it2 = word2;
    }



    vector<list<pair<list<string>,char>>::iterator> deleteIterators;
    for(auto it1 = text.begin(); it1 != text.end(); ++it1){
        for(auto it2 = it1->first.begin(); it2 != it1->first.end(); ++it2){
            if(*it2 == word3){
                deleteIterators.push_back(it1);
                break;
            }
        }
    }
    for(auto i: deleteIterators)
        text.erase(i);

    for(auto it1 = text.begin(); it1 != text.end(); ++it1){
        bool lol = false;
        for(auto it2 = it1->first.begin(); it2 != it1->first.end(); ++it2){
            if(*it2 == "\"" && !lol) lol = true;
            else if(*it2 == "\"" && lol) lol = false;
            fout << *it2 << ((it2 == --it1->first.end() || lol )  ? "" : " ");
        }
        fout << it1->second << " ";
    }
}

