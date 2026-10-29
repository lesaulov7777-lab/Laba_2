#ifndef SET_H
#define SET_H

#include "HashTable.h"

class Set {
    ChainTable table;
public:
    Set() : table(16) {}
    bool add(const std::string& key) { return table.insert(key, ""); } // значение не нужно: множество хранит только ключи
    bool del(const std::string& key) { return table.erase(key); }
    bool at(const std::string& key) const { return table.contains(key); }
    int size() const { return table.size(); }
    std::vector<std::string> keys() const { return table.keys(); }
};

#endif
