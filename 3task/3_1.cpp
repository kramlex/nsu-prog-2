//
// Created by markd on 22.10.2019.
//
#include <iostream>
#include <fstream>
#include <string>
#include <tr1/functional>
#include <cmath>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

template <typename T>
class Array {
public:
    typedef T* iterator;

    Array() : size(0) , capacity(0) , arr(nullptr) {}
    Array(size_t newSize , const T& val) : size(newSize) , capacity(newSize*2) , arr(new T[capacity]) {
        for(size_t i = 0; i != size; ++i)
            arr[i] = val;
    }
    Array(const Array<T>& other) : size(other.size) , capacity(other.capacity) , arr(new T[capacity]) {
        for(size_t i = 0; i != size; i++)
            arr[i] = other.arr[i];
    }
    ~Array() {
        delete[] arr;
    }

    size_t get_size() const{
        return size;
    }

    void resize(size_t newSize , T val){
        if(capacity == 0) capacity = 32;
        while(capacity < newSize)
            capacity *= 2;
        T* newArray = new T[capacity];
        for(size_t i = 0; i != min(size,newSize) ; ++i)
            newArray[i] = arr[i];
        for(size_t i = min(size,newSize) ; i != newSize ; ++i)
            newArray[i] = val;
        delete[] arr;
        size = newSize;
        arr = newArray;
    }

    T &operator[](size_t index){
        return arr[index];
    }

    iterator begin(){
        return arr;
    }

    iterator end(){
        return arr + size;
    }

private:
    T *arr;
    size_t capacity, size;
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

    bool find(K key){
        size_t index = get_index(key);
        LinkedHashEntry<K,T>* ptr = table[index];
        while(ptr){
            if(ptr->get_key() == key){
                lastFound = ptr->get_value();
                return true;
            }
            ptr = ptr->get_next();
        }
        return false;
    }

    void insert(K key, T val){
        size_t index;
        if(find(key)){
            index = get_index(key);
            LinkedHashEntry<K,T>* ptr = table[index];
            while(ptr){
                if(ptr->get_key() == key){
                    ptr->set_value(val);
                    break;
                }
                ptr = ptr->get_next();
            }
        }
        else{
            check_resize();
            index = get_index(key);
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
        if(!table[index]) return;
        LinkedHashEntry<K,T>* ptr = table[index] , *delPtr;
        if(ptr->get_key() == key) {
            delPtr = ptr;
            table[index] = ptr->get_next();
            used--;
            delete delPtr;
            return;
        }
        while(ptr->get_next()){
            if(ptr->get_next()->get_key() == key){
                delPtr = ptr->get_next();
                delete delPtr;
                used--;
                return;
            }
        }

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

    public:
        LinkedHashEntry<K,T>* it;
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

private:
    size_t get_index(K key){
        return hashFunction(key) % size;
    }

    void rehash_list(LinkedHashEntry<K,T> *ptr){
        LinkedHashEntry<K,T>* delTmp;
        while(ptr){
            delTmp = ptr;
            used--;
            insert(ptr->get_key() , ptr->get_value());
            ptr = ptr->get_next();
            delete delTmp;
        }
    }

    void rehash_table(){
        LinkedHashEntry<K,T> *tmp;
        for(size_t i = 0; i < size/2; i++){
            if(!table[i]) continue;
            tmp = table[i];
            table[i] = nullptr;
            rehash_list(tmp);
        }
    }

    void check_resize(){
        if( (double) used >= (double) size * ratio){
            size *= 2;
            table.resize(size, nullptr);
            rehash_table();
        }
    }

private:
    Array<LinkedHashEntry<K,T>*> table;
    size_t size,used;
    tr1::hash<K> hashFunction;
    double ratio = 0.75;
    T lastFound;
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
        cout << key << " " << value << endl;
    }
    HashTable<T,int> hashTable1(128);
    for(auto cell : hashTable){
        hashTable1.insert(cell.get_value() , 0);
    }
//    ********
//    for(typename HashTable<K,T>::iterator it = hashTable.begin() ; it != hashTable.end() ; it++){
//        hashTable1.insert(it->get_value() , 0);
//    }
//    OR
//    for(typename HashTable<K,T>::iterator it = hashTable.begin() ; it != hashTable.end() ; ++it){
//        hashTable1.insert(it->get_value() , 0);
//    }
//    ********
    fout << hashTable.get_used() << " " << hashTable1.get_used() << endl;
}

int main(){
    char type1, type2;
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
    return 0;
}