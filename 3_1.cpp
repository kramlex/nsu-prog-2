//
// Created by markd on 15.10.2019.
//

#include <iostream>
#include <string>
#include <tr1/functional>
#include <exception>
#include <vector>
#include <list>

using namespace std;
//EXCEPTION
class MyException : public exception{
public:
    explicit MyException(string ss) : s(std::move(ss)) {}
    ~MyException() noexcept override = default;

    const char *what() const noexcept override {
        return s.c_str();
    }

private:
    string s;
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

/**
 *
 * @tparam K Key Type
 * @tparam T Val Type
 */
template <typename K, typename T>
class HashTable{
class iterator : public std::iterator;

public:
    explicit HashTable(size_t _size) {
        size = _size;
        table.resize(size, nullptr);
        used = 0;
    }

    ~HashTable(){
        LinkedHashEntry<K,T>* tmp;
        LinkedHashEntry<K,T>* delTmp;
        for(int i = 0; i < size; i++){
            if(table[i]){
                tmp = table[i];
                while(tmp->get_next()){
                    delTmp = tmp;
                    tmp = tmp->get_next();
                    delete delTmp;
                }
            }
        }
    }

    void rehash(){
        tr1::hash<K> hash_function;
        for(int i = 0; i < size/2; i++){
            if(table[i]){
                LinkedHashEntry<K,T>* tmp = table[i];
                LinkedHashEntry<K,T>* delTmp;
                table[i] == nullptr;
                while(tmp->get_next()){
                    delTmp = tmp;
                    used--;
                    insert(tmp->get_key() , tmp->get_value());
                    tmp = tmp->get_next();
                    delete delTmp;
                }

            }
        }
    }

    void insert(const K key , const T& val){
        tr1::hash<K> hash_function;

        T tmpT;
        if(find(key,tmpT)){
            size_t index = hash_function(key) % size;
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
            if( (double) used >= (double) size * 0.75){
                size *= 2;
                table.resize(size);
                rehash();
            }
            size_t index = hash_function(key) % size;
            if(!table[index]){
                table[index] = new LinkedHashEntry<K,T>(key, val);
            }
            else{
                LinkedHashEntry<K,T>* tmp = new LinkedHashEntry<K,T>(key,val);
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
            delete delTmp;
        }
        while(tmp->get_next()){
            if(tmp->get_next()->get_key() == key){
                LinkedHashEntry<K,T>* delTmp = tmp->get_next();
                tmp = tmp->get_next()->get_next();
                delete delTmp;
            }
        }
    }

    bool find(K key , T& find_val){
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

private:
    vector<LinkedHashEntry<K,T>*> table;
    size_t used;
    size_t size;
};

int main(){
    HashTable<int,string> hashTable(128);
    string finded;
    hashTable.insert(5,"Mark");
    hashTable.del(5);
    cout << hashTable.find(5, finded);
    cout << " " + finded;
    return 0;

}
