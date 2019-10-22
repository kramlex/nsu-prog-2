//
// Created by markd on 15.10.2019.
//

#include <iostream>
#include <fstream>
#include <string>
#include <tr1/functional>
#include <vector>
#include <cmath>

using namespace std;

ofstream fout("output.txt");
ifstream fin("input.txt");

template <typename K, typename T>
class LinkedHashEntry{
public:
    LinkedHashEntry(K key , T value) : key(key) , value(value) , next(nullptr) {}

    K get_key() { return key; }

    T get_value() {return value;}

    void set_value(const T& _value) {this->value = _value; }

    LinkedHashEntry* get_next() { return next; }

    void set_next(LinkedHashEntry* _next) { this->next = _next; }


protected:
    K key;
    T value;
    LinkedHashEntry* next;
};


template <typename K, typename T>
class HashTable{
public:

    explicit HashTable(size_t _size , double _ratio) {
        size = _size;
        table.resize(size, nullptr);
        used = 0;
        ratio = _ratio;
    }

    ~HashTable(){
        LinkedHashEntry<K,T> *tmp , *delTmp;
        for(auto list : table){
            if(!list) continue;
            tmp = list;
            while(tmp){
                delTmp = tmp;
                tmp = tmp->get_next();
                delete delTmp;
            }
        }
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
            table.resize(size);
            rehash_table();
        }
    }

    size_t get_index(K key){
        return hashFunction(key) % size;
    }

    void insert(const K key , const T& val){
        T tmpT;
        if(find(key,tmpT)){
            size_t index = get_index(key);
            LinkedHashEntry<K,T>* tmp = table[index];
            while(tmp){
                if(tmp->get_key() == key){
                      tmp->set_value(val);
                    break;
                }
                tmp = tmp->get_next();
            }
        }
        else{
            check_resize();
            size_t index = get_index(key);
            if(!table[index]){
                table[index] = new LinkedHashEntry<K,T>(key, val);
            }
            else{
                auto* tmp = new LinkedHashEntry<K,T>(key,val);
                tmp->set_next(table[index]);
                table[index] = tmp;
            }
            used++;
        }
    }

    void del(K key){
        tr1::hash<K> hash_function;
        size_t index = hash_function(key) % size;
        if(!table[index])
            return;
        LinkedHashEntry<K,T>* tmp = table[index];
        if(tmp->get_key() == key){
            LinkedHashEntry<K,T>* delTmp = tmp;
            table[index] = tmp->get_next();
            used--;
            delete delTmp;
        }
        while(tmp->get_next()){
            if(tmp->get_next()->get_key() == key){
                LinkedHashEntry<K,T>* delTmp = tmp->get_next();
                tmp = tmp->get_next()->get_next();
                delete delTmp;
                used--;
            }
        }
    }

    bool find (K key , T& find_val ){
        tr1::hash<K> hash_function;
        size_t index = hash_function(key) % size;
        LinkedHashEntry<K,T>* tmp = table[index];
        while(tmp){
            if(tmp->get_key() == key){
                find_val = tmp->get_value();
                return true;
            }
            tmp = tmp->get_next();
        }
        return false;
    }

    friend class Iterator;
    class Iterator{
        friend class HashTable<K,T>;
    public:
        Iterator() : ptr(nullptr) , index(0) {}
        Iterator(LinkedHashEntry<K,T>* _ptr, size_t _index) : ptr(_ptr) , index(_index) {}
        Iterator(const Iterator& iterator){
            *this = iterator;
        }

        Iterator&operator=(const Iterator& that){
            ptr = that.ptr;
            index = that.index;
            return *this;
        }

        Iterator& inc(HashTable& hashTable){
            if(ptr->get_next()){
                ptr = ptr->get_next();
                return *this;
            }
            else{
                for(int i = index+1; i < hashTable.size ; i++){
                    if(hashTable.table[i]){
                        ptr = hashTable.table[i];
                        index = i;
                        return *this;
                    }
                }
                ptr = nullptr;
                index = 0;
                return *this;
            }
        }
        /*
        Iterator&operator++(){
            if(ptr->get_next())
                ptr = ptr->get_next();
                else{
                for(int i = index+1; i < size; i++){
                    if(table[i]){
                        ptr = table[i];
                        index = i;
                        return *this;
                    }
                }
            }
            ptr = nullptr;
            index = 0;
            return *this;
        }
         */

        // ?????????????????????????????????????????????
        LinkedHashEntry<K,T>*operator->(){
            return ptr;
        };

        bool operator==(const Iterator &that){
            return (ptr == that.ptr && index == that.index);
        }
        bool operator!=(const Iterator &that){
            return !(ptr == that.ptr && index == that.index);
        }

    private:
        LinkedHashEntry<K,T>* ptr;
        size_t index;
    };


    Iterator begin(){
        for(size_t i = 0; i < size ; i++){
            if(!table[i]) continue;
            return Iterator(table[i], i);
        }
        return Iterator();
    }

    Iterator end(){
        return Iterator();
    }

    size_t get_used(){
        return used;
    }

    size_t get_size(){ return size; }

private:
    vector<LinkedHashEntry<K,T>*> table;
    size_t used , size;
    tr1::hash<K> hashFunction;
    double ratio;

};


template <typename K, typename T>
void all_functional(){
    HashTable<K,T> hashTable(128,0.75);
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
                hashTable.del(key);
                break;
            }
        }
    }
    HashTable<T,int> hashTable1(128,0.75);
    for(typename HashTable<K,T>::Iterator i = hashTable.begin() ; i != hashTable.end(); i.inc(hashTable))
        hashTable1.insert(i->get_value() , 0);
    fout << hashTable.get_used() << " " << hashTable1.get_used() << endl;

}

int main(){
    char type1, type2;
    fin >> type1 >> type2;
    if(type1 == 'I'){
        if(type2 == 'I') all_functional<int, int>();
        if(type2 == 'D') all_functional<int, double>();
        if(type2 == 'S') all_functional<int, string>();
    }
    else if(type1 == 'D'){
        if(type2 == 'I') all_functional<double, int>();
        if(type2 == 'D') all_functional<double, double>();
        if(type2 == 'S') all_functional<double, string>();
    }
    else if(type1 == 'S'){
        if(type2 == 'I') all_functional<string, int>();
        if(type2 == 'D') all_functional<string, double>();
        if(type2 == 'S') all_functional<string, string>();
    }
    return 0;
}