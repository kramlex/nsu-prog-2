//
// Created by markd on 22.10.2019.
//
#include <iostream>
#include <fstream>
#include <string>
#include <tr1/functional>
#include <cmath>
#include <vector>

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

private:
    K key;
    T value;
    LinkedHashEntry* next;
};


template <typename K, typename T>
class HashTable{
public:

    explicit HashTable(size_t _size , double _ratio) : size(_size) , ratio(_ratio) , used(0) {
        table.resize(size, nullptr);
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

    void remove(K key){
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

    class Iterator {
    public:
        Iterator(typename vector<LinkedHashEntry<K,T>*>::iterator vec_its ,
                typename vector<LinkedHashEntry<K,T>*>::iterator vec_ends,
                LinkedHashEntry<K,T>* list_its) : vec_it(vec_its) , vec_end(vec_ends) , list_it(list_its) {}

        Iterator(const Iterator& iterator){
            *this = iterator;
        }

        Iterator&operator=(const Iterator& other){
            vec_it = other.vec_it;
            list_it = other.list_it;
            vec_end = other.vec_end;
            return *this;
        }

        bool operator==(const Iterator &other){
            return (list_it  == other.list_it);
        }

        bool operator!=(const Iterator &other){
            return !(list_it == other.list_it);
        }

        Iterator&operator++(){
            if(list_it->get_next())
                list_it = list_it->get_next();
            else{
                while(vec_it != vec_end){
                    vec_it++;
                    if(*vec_it) {
                        break;
                    }
                }
                list_it = *vec_it;
            }
            return *this;
        }

        LinkedHashEntry<K,T>*operator->(){
            return list_it;
        }
        LinkedHashEntry<K,T>&operator*(){
            return *list_it;
        }
    private:
        typename vector<LinkedHashEntry<K,T>*>::iterator vec_it, vec_end;
        LinkedHashEntry<K,T>* list_it;

    };


    Iterator begin(){
        for(auto it = table.begin(); it != table.end() ; ++it){
            if(*it) return Iterator(it, table.end() , *it);
        }
        return Iterator(table.begin() , table.begin() , *table.end());
    }

    Iterator end(){
        auto it = table.end();
        for(; it != table.begin() ; --it){
            if(*it){
                auto ptr = *it;
                for(; ptr->get_next() ; ptr = ptr->get_next());
                return Iterator(it , table.end(), ptr);
            }
        }
        return Iterator(table.end() , table.end() , *table.end());
    }

    size_t get_used() { return used; }

    size_t get_size() { return size; }

private:
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

    vector<LinkedHashEntry<K,T>*> table;
    size_t used , size;
    tr1::hash<K> hashFunction;
    double ratio;

};

int main(){
    HashTable<string, int> hashTable(128, 0.75);
    hashTable.insert("mark" , 5);
    hashTable.insert("kostya" , 5);
    HashTable<string, int>::Iterator it = hashTable.begin();

}