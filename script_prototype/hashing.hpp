#ifndef __hashink
#define __hashink

#include "pch.h"

struct Node {
    std::string_view key;
    int value;
    Node* next;
};


class Hashtable {
private:
    Node** table;
    int table_size;
    int occupied;
public:
    Hashtable(int size);
    ~Hashtable();

    int hash(const std::string_view& key) const;
    void insert(const std::string_view& key, int value);
    int get(const std::string_view& key) const;
    void remove(const std::string_view& key);
    void print();
    int size() const { return occupied; }
    
};

#endif