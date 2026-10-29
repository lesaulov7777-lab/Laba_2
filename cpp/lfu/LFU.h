#ifndef LFU_H
#define LFU_H

#include <string>
#include <unordered_map>
#include "DoublyLinkedList.h"

// LFU-кэш: set/get за O(1). При равной частоте вытесняется самый давний ключ.
struct LFUEntry {
    std::string value;
    int freq;
    DoublyNode* node; // узел ключа в списке своей частоты
};

class LFUCache {
    int cap;
    int minFreq;
    std::unordered_map<std::string, LFUEntry> items;
    std::unordered_map<int, DoublyLinkedList*> freq; // частота -> ключи в порядке последнего обращения
    void attach(const std::string& key, LFUEntry& e, int f);
    void detach(LFUEntry& e);
    void bump(const std::string& key);
    void evict();
public:
    explicit LFUCache(int capacity);
    ~LFUCache();
    LFUCache(const LFUCache&) = delete;
    LFUCache& operator=(const LFUCache&) = delete;
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
};

#endif
