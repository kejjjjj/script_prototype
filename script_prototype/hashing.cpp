#include "pch.h"

//CHATGPT COPY PASTE
Hashtable::Hashtable(int size) {
    table_size = size;
    table = new Node * [table_size];
    for (int i = 0; i < table_size; i++) {
        table[i] = NULL;
    }
}

Hashtable::~Hashtable() {
    for (int i = 0; i < table_size; i++) {
        Node* current = table[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}

int Hashtable::hash(const std::string_view& key) const {
    int hash_value = 0;
    for (int i = 0; i < key.size(); i++) {
        hash_value += key[i];
    }
    return hash_value % table_size;
}

void Hashtable::insert(const std::string_view& key, int value) {
    int index = hash(key);
    Node* current = table[index];
    while (current != NULL) {
        if (key.compare(current->key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    Node* node = new Node();
    node->key = key;
    //strcpy(node->key, key);
    node->value = value;
    node->next = table[index];
    table[index] = node;
    occupied++;
}

int Hashtable::get(const std::string_view& key) const {
    int index = hash(key);
    Node* current = table[index];
    while (current != NULL) {
        if (key.compare(current->key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

void Hashtable::remove(const std::string_view& key) {
    int index = hash(key);
    Node* current = table[index];
    Node* prev = NULL;
    while (current != NULL) {
        if (key.compare(current->key) == 0) {
            if (prev == NULL) {
                table[index] = current->next;
            }
            else {
                prev->next = current->next;
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void Hashtable::print() {
    for (int i = 0; i < table_size; i++) {
        Node* current = table[i];
        std::cout << i << ": ";
        while (current != NULL) {
            std::cout << "(" << current->key << ", " << current->value << ") ";
            current = current->next;
        }
        std::cout << std::endl;
    }
}
