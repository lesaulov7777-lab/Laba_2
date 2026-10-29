#ifndef BLOOM_H
#define BLOOM_H

#include <string>
#include <vector>
#include "HashTable.h"

class BloomFilter {
    std::vector<unsigned long long> bits;
    int m;
    int k;
    std::vector<int> positions(const std::string& key) const;
public:
    BloomFilter(int mBits = 256, int kHashes = 3);
    void add(const std::string& key);
    bool maybeContains(const std::string& key) const;
};

class BloomTable {
    ChainTable inner;
    BloomFilter bloom;
public:
    BloomTable(int cap = 16, int mBits = 256, int k = 3);
    // true - ключ новый; false - ключ уже был, его значение заменено
    bool insert(const std::string& key, const std::string& value);
    // bloomReject = true, если фильтр Блума отсёк запрос и таблица не просматривалась
    bool get(const std::string& key, std::string& value, bool& bloomReject) const;
    bool contains(const std::string& key, bool& bloomReject) const;
    bool erase(const std::string& key);
};

#endif
