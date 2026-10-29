#include "LFU.h"

LFUCache::LFUCache(int capacity) : cap(capacity < 1 ? 1 : capacity), minFreq(0) {}
LFUCache::~LFUCache() {
    for (auto& p : freq) delete p.second;
}
void LFUCache::attach(const std::string& key, LFUEntry& e, int f) {
    DoublyLinkedList*& lst = freq[f];
    if (!lst) lst = new DoublyLinkedList();
    e.freq = f;
    e.node = lst->pushBackNode(key);
}
void LFUCache::detach(LFUEntry& e) {
    auto it = freq.find(e.freq);
    it->second->removeByNode(e.node);
    if (it->second->getSize() == 0) {
        delete it->second;
        freq.erase(it);
    }
}
void LFUCache::bump(const std::string& key) {
    LFUEntry& e = items[key];
    int old = e.freq;
    detach(e);
    attach(key, e, old + 1);
    if (old == minFreq && !freq.count(old)) minFreq = old + 1;
}
void LFUCache::evict() {
    auto it = freq.find(minFreq);
    if (it == freq.end()) return;
    std::string key = it->second->front()->value;
    detach(items[key]);
    items.erase(key);
}
void LFUCache::set(const std::string& key, const std::string& value) {
    auto it = items.find(key);
    if (it != items.end()) {
        it->second.value = value;
        bump(key);
        return;
    }
    if ((int)items.size() >= cap) evict();
    LFUEntry& e = items[key];
    e.value = value;
    attach(key, e, 1);
    minFreq = 1;
}
std::string LFUCache::get(const std::string& key) {
    auto it = items.find(key);
    if (it == items.end()) return "-1";
    bump(key);
    return it->second.value;
}
