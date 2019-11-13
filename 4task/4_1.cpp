//
// Created by markd on 07.11.2019.
//
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

class DFA {
public:
    struct Node {
        Node(bool final) : final(final) {}

        unordered_map<size_t,size_t> transition;
        bool final;

        bool hasTransition(size_t key){
            return transition.find(key) != transition.end();
        }
    };

    DFA(size_t _countState, size_t _startState, vector<size_t> &_finalStates) :
    countStates(_countState), startState(_startState) {
        states.resize(countStates, nullptr);
        for(size_t i = 0; i < countStates; ++i)
            states[i] = new Node(false);
        for(auto i : _finalStates)
            states[i]->final = true;
    }

    void insert(size_t from, size_t to, char value){
        states[from]->transition.insert(make_pair(charCode(value), to));
    }

    bool has(string scanned){
        Node* ptr = states[startState];
        for(auto i: scanned){
            if(!ptr->hasTransition(charCode(i))) return false;
            size_t index = ptr->transition[charCode(i)];
            ptr = states[index];
        }
        return ptr->final;
    }

    void print(){
        for(size_t i = 0 ; i < countStates; ++i) {
            cout << "Node " << i << ": [ ";
            for(auto j : states[i]->transition)
                cout << " {" << (char)(j.first + 'a') << ";" << j.second << "} ";
            cout  << " ] final = "<< (states[i]->final ? "true" : "false") << endl;
        }
    }
private:
    static size_t charCode(char c){
        return c - 'a';
    }

private:
    size_t countStates;
    size_t startState;
    vector<Node*> states;
};

int main(){
    size_t n,k,f,p;
    fin >> n >> k >> f;
    vector<size_t> finalStates(f);
    for(size_t i = 0; i < f; ++i)
        fin >> finalStates[i];

    DFA dfa(n,k,finalStates);

    fin >> p;
    size_t from, to;
    char value;
    for(size_t i = 0; i < p; ++i){
        fin >> from >> to >> value;
        dfa.insert(from, to, value);
    }
    dfa.print();
    size_t t;
    string s;
    fin >> t;
    for(size_t i = 0; i < t; ++i){
        fin >> s;
        fout <<  (dfa.has(s) ? "YES" : "NO") << endl;
    }
}
