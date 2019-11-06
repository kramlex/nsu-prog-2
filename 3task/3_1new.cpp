//
// Created by markd on 22.10.2019.
//
#include <iostream>
#include <fstream>
#include <string>
#include <tr1/functional>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

template <typename T>
class Array {
public:
    Array() : size(0), arr(nullptr) {}
    Array(size_t newSize, const T& val) : size(newSize), arr(new T[size]) {
        for(size_t i = 0; i != size; ++i)
            arr[i] = val;
    }
    Array(const Array<T> &other) {
        delete[] arr;
        size = other.size;
        arr = new T[size];
        for(size_t i = 0; i != size; ++i)
            arr[i] = other.arr[i];
    }
    ~Array(){
        delete[] arr;
    }

    size_t getSize() const {
        return size;
    }

    void resize(size_t newSize, T val) {
        T *newArr = new T[newSize];
        for(size_t i = 0; i != newSize; ++i)
            newArr[i] = (i < min(size, newSize) ? arr[i] : val);
        delete[] arr;
        arr = newArr; size = newSize;
    }

    T &operator[](size_t index) {
        return arr[index];
    }

    Array<T> &operator=(Array<T> &other){
        delete[] arr;
        size = other.size;
        arr = new T[size];
        for(int i = 0; i < size; ++i)
            arr[i] = other.arr[i];
        return *this;
    }

    typedef T* iterator;
    iterator begin(){
        return arr;
    }

    iterator end(){
        return arr + size;
    }

private:
    T *arr;
    size_t size;
};

template <typename K, typename T>
class LinkedHashEntry {
public:
    LinkedHashEntry(K key, T value) : key(key), value(value), next(nullptr) {}

    K getKey() { return key; }

    T getValue() { return value; }
    void setValue(const T& _value) { this->value = _value; }

    LinkedHashEntry<K,T>* getNext() { return next; }
    void setNext(LinkedHashEntry<K,T>* _next) { this->next = _next; }

private:
    K key;
    T value;
    LinkedHashEntry *next;
};

template <typename K, typename T>
class HashTable {
public:
    HashTable(size_t size) : size(size) {
        table.resize(size, nullptr);
        used = 0;
    }

    ~HashTable() {
        for(auto list : table) {
            if(!list) continue;
            auto ptr = list;
            while(ptr) {
                auto delPtr = ptr;
                ptr = ptr->getNext();
                delete delPtr;
            }
        }
    }

    // iterator
    class iterator {
    public:
        iterator(LinkedHashEntry<K,T>* _it, typename Array<LinkedHashEntry<K,T>*>::iterator _arr_it,
                 typename Array<LinkedHashEntry<K,T>*>::iterator _arr_end) :
                 it(_it), arr_it(_arr_it), arr_end(_arr_end), prev(nullptr) {}
        iterator(const iterator& other) : it(other.it), arr_it(other.arr_it), arr_end(other.arr_end), prev(other.prev) {}

        iterator &operator=(const iterator& other) {
            prev = other.prev;
            it = other.it;
            arr_it = other.arr_it;
            arr_end = other.arr_end;
        }

        LinkedHashEntry<K,T>*&operator->() {
            return it;
        }
        LinkedHashEntry<K,T>&operator*() {
            return *it;
        }

        bool operator==(const iterator& other) {
            return ((it == other.it) && (arr_it == other.arr_it) && (arr_end == other.arr_end));
        }
        bool operator!=(const iterator& other) {
            return !(*this == other);
        }

        iterator &operator++() {
            if(it->getNext()) {
                prev = it;
                it = it->getNext();
                return *this;
            }
            prev = nullptr;
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
        iterator operator++(int d) {
            iterator returned = *this;
            if(it->getNext()) {
                prev = it;
                it = it->getNext();
                return returned;
            }
            prev = nullptr;
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

        typename Array<LinkedHashEntry<K,T>*>::iterator getArrIterator(){
            return arr_it;
        }
        bool hasValue(){
            return it != nullptr;
        }

        LinkedHashEntry<K,T>* getPrev(){
            return prev;
        }

        LinkedHashEntry<K,T>* getPtr(){
            return it;
        }

    private:
        LinkedHashEntry<K,T>* it;
        LinkedHashEntry<K,T>* prev;
        typename Array<LinkedHashEntry<K,T>*>::iterator arr_it;
        typename Array<LinkedHashEntry<K,T>*>::iterator arr_end;
    };

    iterator begin() {
        for(typename Array<LinkedHashEntry<K,T>*>::iterator it = table.begin(); it != table.end() ; it++){
            if(*it){
                return iterator(*it , it , table.end());
            }
        }
        return end();
    }

    iterator end(){
        return iterator(nullptr, table.end(), table.end());
    }

    size_t getUsed() const {
        return used;
    }

    // functional
    iterator find(K key) {
        size_t index = getIndex(key);
        if(!table[index]) return end();

        iterator required = iterator(table[index], table.begin() + index, table.end());
        auto tableIterator = required.getArrIterator();
        while(true){
            if(required.getArrIterator() != tableIterator) return end();
            if(required->getKey() == key) return required;
            ++required;
        }
    }

    void insert(K key, T val) {
        size_t index;
        iterator found = find(key);
        if(found.hasValue())
            found->setValue(val);
        else if(!found.hasValue()) {
            index = getIndex(key);
            checkResize();
            if(!table[index])
                table[index] = new LinkedHashEntry<K,T>(key,val);
            else {
                LinkedHashEntry<K,T> *ptr = new LinkedHashEntry<K,T>(key, val);
                ptr->setNext(table[index]);
                table[index] = ptr;
            }
            used++;
        }

    }

    void remove(K key) {
        size_t index = getIndex(key);
        iterator found = find(key);
        if(!found.hasValue()) return;
        if(found.getPrev())
            found.getPrev()->setNext(found->getNext());
        else
            *(found.getArrIterator()) = found->getNext();
        used--;
        delete found.getPtr();
    }

    HashTable<K,T> &operator=(HashTable<K,T> &other) {
        size = other.size;
        used = other.used;
        table = other.table;
        return *this;
    }

// private method
private:
    size_t getIndex(K key) {
        return hashFunction(key) % size;
    }

    void rehashList(LinkedHashEntry<K,T>* ptr, HashTable<K,T> &newTable){
        LinkedHashEntry<K,T>* delTmp;
        while(ptr){
            delTmp = ptr;
            newTable.insert(ptr->getKey(), ptr->getValue());
            ptr = ptr->getNext();
            delete delTmp;
        }
    }

    void checkResize() {
        if( (double) used >= (double) size * ratio) {
            HashTable<K,T> newHashTable(this->size * 2);
            LinkedHashEntry<K,T>* tmp;
            for(size_t i = 0; i < size; ++i){
                if(!table[i]) continue;
                tmp = table[i];
                table[i] = nullptr;
                rehashList(tmp, newHashTable);
            }
            *this = newHashTable;
        }
    }

// private variable;
private:
    Array<LinkedHashEntry<K,T>*> table;
    size_t size, used;
    tr1::hash<K> hashFunction;
    double ratio = 0.75;
};

template <typename K , typename T>
void work_with_table(){
    HashTable<K,T> hashTable(32);
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
    HashTable<T,int> hashTable1(32);
    for(auto cell : hashTable)
        hashTable1.insert(cell.getValue() , 0);
    fout << hashTable.getUsed() << " " << hashTable1.getUsed();
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