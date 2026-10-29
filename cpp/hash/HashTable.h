#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <utility>
#include <vector>

int hash1(const std::string& key, int mod);
int hash2(const std::string& key, int mod);

// Хеш-таблица "ключ -> значение", метод цепочек: каждая корзина - односвязный список пар
struct ChainNode {
    std::string key;
    std::string value;
    ChainNode* next;
    ChainNode(const std::string& k, const std::string& v, ChainNode* n) : key(k), value(v), next(n) {}
};

class ChainTable {
    ChainNode** buckets;
    int capacity;
    int sz;
    int rehashes;
    void rehash(int newCap);
    ChainNode* find(const std::string& key) const;
public:
    explicit ChainTable(int cap = 8);
    ~ChainTable();
    ChainTable(const ChainTable&) = delete;
    ChainTable& operator=(const ChainTable&) = delete;
    int size() const;
    bool contains(const std::string& key) const;
    // true - ключ новый; false - ключ уже был, его значение заменено на value
    bool insert(const std::string& key, const std::string& value);
    // true и значение в value, если ключ найден
    bool get(const std::string& key, std::string& value) const;
    bool erase(const std::string& key);
    std::vector<std::string> keys() const;
    void print() const;
    int bucketCount() const;
    int maxChainLength() const;
    int rehashCount() const;
};

// Хеш-таблица "ключ -> значение", хеширование кукушки: две таблицы, две хеш-функции
class CuckooTable {
    std::string* k1;
    std::string* k2;
    std::string* v1;
    std::string* v2;
    bool* used1;
    bool* used2;
    int capacity;
    int sz;
    int rehashes;
    void allocate(int cap);
    // При неудаче (цикл вытеснений) key и value содержат "бездомную" пару, не попавшую в таблицу
    bool insertRaw(std::string& key, std::string& value);
    void rehash(int newCap, const std::string* extraKey, const std::string* extraValue);
public:
    explicit CuckooTable(int cap = 8);
    ~CuckooTable();
    CuckooTable(const CuckooTable&) = delete;
    CuckooTable& operator=(const CuckooTable&) = delete;
    int size() const;
    bool contains(const std::string& key) const;
    // true - ключ новый; false - ключ уже был, его значение заменено на value
    bool insert(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;
    bool erase(const std::string& key);
    std::vector<std::string> keys() const;
    void print() const;
    int tableSize() const;
    int rehashCount() const;
};

#endif
