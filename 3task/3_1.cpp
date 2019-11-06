//
// Created by markd on 22.10.2019.
//
#include <iostream>
#include <fstream>
#include <string>
#include <tr1/functional>
#include <cassert>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

template <typename T>
class Array {
public:
    Array() : size(0), arr(nullptr) {}
    Array(size_t newSize , const T& val) : size(newSize), arr(new T[size]) {
        for(size_t i = 0; i != size; ++i) arr[i] = val;
    }
    Array(const Array<T>& other) : size(other.size), arr(new T[size]) {
        for(size_t i = 0; i != size; i++) arr[i] = other.arr[i];
    }
    ~Array() {
        delete[] arr;
    }

    size_t get_size() const{
        return size;
    }
    void resize(size_t newSize , T val){
        T* newArray = new T[newSize];
        for(size_t i = 0; i != newSize; ++i)
            newArray[i] = (i < min(size, newSize) ? arr[i] : val);
        delete[] arr;
        arr = newArray; size = newSize;
    }

    T &operator[](size_t index){
        return arr[index];
    }
    Array<T> &operator=(Array<T> &other){
        size = other.size;
        arr = new T[size];
        for(int i = 0; i < size; i++)
            arr[i] = other.arr[i];
        return *this;
    }

    typedef T* iterator;
    iterator begin() {
        return arr;
    }
    iterator end() {
        return arr + size;
    }

public:
    T *arr;
    size_t size;
};

template <typename K, typename T>
class LinkedHashEntry{
public:
    LinkedHashEntry(K key , T value) : key(key) , value(value) , next(nullptr) {}

    K get_key() { return key; }

    T get_value() {return value;}
    void set_value(const T& _value) {this->value = _value; }

    LinkedHashEntry* get_next() { return next; }
    void set_next(LinkedHashEntry* _next) { this->next = _next; }

private:
    K key;
    T value;
    LinkedHashEntry* next;
};

template <typename K, typename T>
class HashTable {
public:
    explicit HashTable(size_t _size) : size(_size) {
        table.resize(size , nullptr);
        used = 0;
    }
    ~HashTable(){
        for(auto list : table){
            if(!list) continue;
            auto ptr = list;
            while(ptr){
                auto delPtr = ptr;
                ptr = ptr->get_next();
                delete delPtr;
            }
        }
    }
    struct pair{
    public:
        pair(LinkedHashEntry<K,T>* _foundPtr , LinkedHashEntry<K,T>* _prevPtr , size_t _index) :
                foundPtr(_foundPtr ), prevPtr(_prevPtr) , index(_index) {}
        T getValue() {
            return foundPtr->get_value();
        }
        LinkedHashEntry<K,T>* getPtr(){
            return foundPtr;
        }
        LinkedHashEntry<K,T>* getPrev(){
            return  prevPtr;
        }
        bool hasValue(){
            return foundPtr != nullptr;
        }
        size_t getIndex(){
            return index;
        }
    private:
        LinkedHashEntry<K,T> *foundPtr , *prevPtr;
        size_t index;
    };

    pair find(K key){
        size_t index = get_index(key);
        if(!table[index])
            return pair(nullptr , nullptr , 0);
        LinkedHashEntry<K,T> *ptr = table[index] , *prevPtr = nullptr;
        if(ptr && ptr->get_next()){
            prevPtr = ptr; ptr = ptr->get_next();
        }
        while(ptr){
            if(ptr->get_key() == key)
                return pair(ptr , prevPtr , index);
            prevPtr = ptr; ptr = ptr->get_next();
        }
        return pair(nullptr , nullptr , 0);
    }

    void insert(K key, T val){
        cout << size << " " << used << " " << table.size <<endl;
        size_t index;
        pair found = find(key);
        if(found.hasValue())
            found.getPtr()->set_value(val);
        else{
            check_resize();
            index = get_index(key);
            cout << "lol";
            if(!table[index])
                table[index] = new LinkedHashEntry<K,T>(key,val);
            else{
                LinkedHashEntry<K,T>* ptr = new LinkedHashEntry<K,T>(key,val);
                ptr->set_next(table[index]);
                table[index] = ptr;
            }
            used++;
        }
    }

    void remove(K key){
        size_t index = get_index(key);
        pair found = find(key);
        if(!found.hasValue()) return;
        if(found.getPrev())
            found.getPrev()->set_next(found.getPtr()->get_next());
        if(!found.getPrev())
            table[found.getIndex()] = found.getPtr()->get_next();
        delete found.getPtr();
    }

    class iterator{
    public:
        iterator(LinkedHashEntry<K,T>* _it , typename Array<LinkedHashEntry<K,T>*>::iterator _arr_it,
                 typename Array<LinkedHashEntry<K,T>*>::iterator _arr_end) : it(_it) , arr_it(_arr_it) , arr_end(_arr_end) {}
        iterator(const iterator& other) : it(other.it) , arr_it(other.arr_it) , arr_end(other.arr_end) {}

        iterator &operator=(const iterator& other){
            it = other.it;
            arr_it = other.arr_it;
            arr_end = other.arr_end;
        }

        LinkedHashEntry<K,T>*&operator->(){
            return it;
        }
        LinkedHashEntry<K,T>&operator*(){
            return *it;
        }

        bool operator==(const iterator& other){
            return ((it == other.it) && (arr_it == other.arr_it) && (arr_end == other.arr_end));
        }
        bool operator!=(const iterator& other){
            return !(*this == other);
        }

        iterator &operator++(){
            if(it->get_next()) {
                it = it->get_next();
                return *this;
            }
            arr_it++;
            while(arr_it != arr_end){
                if(*arr_it){
                    it = *arr_it;
                    return *this;
                }
                arr_it++;
            }
            it = nullptr;
            return *this;
        }
        iterator operator++(int d){
            iterator returned = *this;
            if(it->get_next()) {
                it = it->get_next();
                return returned;
            }
            arr_it++;
            while(arr_it != arr_end){
                if(*arr_it){
                    it = *arr_it;
                    return returned;
                }
                arr_it++;
            }
            it = nullptr;
            return returned;
        }

    private:
        LinkedHashEntry<K,T>* it;
        LinkedHashEntry<K,T>* prev_it;
        typename Array<LinkedHashEntry<K,T>*>::iterator arr_it;
        typename Array<LinkedHashEntry<K,T>*>::iterator arr_end;
    };

    iterator begin(){
        for(typename Array<LinkedHashEntry<K,T>*>::iterator it = table.begin(); it != table.end() ; it++){
            if(*it){
                return iterator(*it , it , table.end());
            }
        }
        return end();
    }

    iterator end(){
        return iterator(nullptr , table.end() , table.end());
    }

    size_t get_used(){
        return used;
    }

    HashTable<K,T> &operator=(HashTable<K,T> &other){
        size = other.size;
        used = other.used;
        table = other.table;
        return *this;
    }

private:
    size_t get_index(K key){
        return hashFunction(key) % size;
    }

    void rehash_list(LinkedHashEntry<K,T> *ptr , HashTable<K,T> &newTable){
        LinkedHashEntry<K,T>* delTmp;
        while(ptr){
            delTmp = ptr;
            used--;
            newTable.insert(ptr->get_key() , ptr->get_value());
            ptr = ptr->get_next();
            delete delTmp;
        }
    }

    void rehash_table(){
        cout << endl << "rehash table" << endl;
        HashTable<K,T> newHashTable(size);
        LinkedHashEntry<K,T> *tmp;
        for(size_t i = 0; i < size/2; i++){
            if(!table[i]) continue;
            tmp = table[i];
            table[i] = nullptr;
            rehash_list(tmp , newHashTable);
        }
        cout << endl << newHashTable.get_used() << "  end rehash table   " << get_used() << endl;
        *this = newHashTable;
    }

    void check_resize(){
        if( (double) used >= (double) size * ratio){
            size *= 2;
            table.resize(size, nullptr);
            rehash_table();
        }
    }

public:
    Array<LinkedHashEntry<K,T>*> table;
    size_t size,used;
    tr1::hash<K> hashFunction;
    double ratio = 0.75;
};

template <typename K , typename T>
void work_with_table(){
    HashTable<K,T> hashTable(128);
    int n;
    fin >> n;
    char c;
    K key;
    T value;
    for(int i = 0; i < n; i++){
        fin >> c;
        switch(c){
            case 'A': {
                fin >> key >> value;
                hashTable.insert(key,value);
                break;
            }
            case 'R' : {
                fin >> key;
                hashTable.remove(key);
                break;
            }
        }
    }
    HashTable<T,int> hashTable1(128);
    for(auto cell : hashTable)
        hashTable1.insert(cell.get_value() , 0);
    fout << hashTable.getUsed() << " " << hashTable1.get_used();
}

int main(){
    /*char type1, type2;
    fin >> type1 >> type2;
    if(type1 == 'I'){
        if(type2 == 'I') work_with_table<int, int>();
        if(type2 == 'D') work_with_table<int, double>();
        if(type2 == 'S') work_with_table<int, string>();
    }
    else if(type1 == 'D'){
        if(type2 == 'I') work_with_table<double, int>();
        if(type2 == 'D') work_with_table<double, double>();
        if(type2 == 'S') work_with_table<double, string>();
    }
    else if(type1 == 'S'){
        if(type2 == 'I') work_with_table<string, int>();
        if(type2 == 'D') work_with_table<string, double>();
        if(type2 == 'S') work_with_table<string, string>();
    }
     */

    HashTable<string, int> hashTable(16);
    string m = "s";
    for(int i = 0; i < 60; ++i){
        hashTable.insert(m , i);
        m += (i % 2 ? "m" : "s");
    }

    return 0;
}