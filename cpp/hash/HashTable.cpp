#include "HashTable.h"
#include <iostream>
#include <stdexcept>
using namespace std;

int hash1(const string& key, int mod) {
    if (mod <= 0) return 0;
    unsigned long long h = 5381;
    for (unsigned char c : key) h = h * 33 + c;
    return (int)(h % (unsigned long long)mod);
}

int hash2(const string& key, int mod) {
    if (mod <= 0) return 0;
    unsigned long long h = 2166136261ull;
    for (unsigned char c : key) {
        h ^= c;
        h *= 16777619ull;
    }
    int idx = (int)(h % (unsigned long long)mod);
    if (idx == hash1(key, mod)) idx = (idx + 1) % mod;
    return idx;
}

ChainTable::ChainTable(int cap) : capacity(cap < 8 ? 8 : cap), sz(0), rehashes(0) {
    buckets = new ChainNode*[capacity]();
}
ChainTable::~ChainTable() {
    for (int i = 0; i < capacity; i++) {
        ChainNode* n = buckets[i];
        while (n) {
            ChainNode* next = n->next;
            delete n;
            n = next;
        }
    }
    delete[] buckets;
}
int ChainTable::size() const { return sz; }
ChainNode* ChainTable::find(const string& key) const {
    for (ChainNode* n = buckets[hash1(key, capacity)]; n; n = n->next)
        if (n->key == key) return n;
    return nullptr;
}
bool ChainTable::contains(const string& key) const { return find(key) != nullptr; }
bool ChainTable::get(const string& key, string& value) const {
    ChainNode* n = find(key);
    if (!n) return false;
    value = n->value;
    return true;
}
void ChainTable::rehash(int newCap) {
    ChainNode** nb = new ChainNode*[newCap]();
    for (int i = 0; i < capacity; i++) {
        ChainNode* n = buckets[i];
        while (n) { // узлы переносятся как есть, без копирования строк
            ChainNode* next = n->next;
            int idx = hash1(n->key, newCap);
            n->next = nb[idx];
            nb[idx] = n;
            n = next;
        }
    }
    delete[] buckets;
    buckets = nb;
    capacity = newCap;
    rehashes++;
}
bool ChainTable::insert(const string& key, const string& value) {
    ChainNode* n = find(key);
    if (n) { n->value = value; return false; }
    // коэффициент заполнения > 0.75 -> удваиваем число корзин, иначе цепочки растут и операции перестают быть O(1)
    if ((long long)sz * 4 >= (long long)capacity * 3) rehash(capacity * 2);
    int idx = hash1(key, capacity);
    buckets[idx] = new ChainNode(key, value, buckets[idx]);
    sz++;
    return true;
}
int ChainTable::bucketCount() const { return capacity; }
int ChainTable::rehashCount() const { return rehashes; }
int ChainTable::maxChainLength() const {
    int mx = 0;
    for (int i = 0; i < capacity; i++) {
        int len = 0;
        for (ChainNode* n = buckets[i]; n; n = n->next) len++;
        if (len > mx) mx = len;
    }
    return mx;
}
bool ChainTable::erase(const string& key) {
    ChainNode** link = &buckets[hash1(key, capacity)];
    while (*link) {
        if ((*link)->key == key) {
            ChainNode* dead = *link;
            *link = dead->next;
            delete dead;
            sz--;
            return true;
        }
        link = &(*link)->next;
    }
    return false;
}
vector<string> ChainTable::keys() const {
    vector<string> out;
    for (int i = 0; i < capacity; i++)
        for (ChainNode* n = buckets[i]; n; n = n->next) out.push_back(n->key);
    return out;
}
void ChainTable::print() const {
    cout << "chain size=" << sz << " cap=" << capacity << endl;
    for (int i = 0; i < capacity; i++) {
        if (!buckets[i]) continue;
        cout << "  [" << i << "]";
        for (ChainNode* n = buckets[i]; n; n = n->next) cout << " " << n->key << "=" << n->value;
        cout << endl;
    }
}

static const int CUCKOO_MAX_CAP = 1 << 23;

void CuckooTable::allocate(int cap) {
    capacity = cap < 8 ? 8 : cap;
    k1 = new string[capacity];
    k2 = new string[capacity];
    v1 = new string[capacity];
    v2 = new string[capacity];
    used1 = new bool[capacity]();
    used2 = new bool[capacity]();
    sz = 0;
}
CuckooTable::CuckooTable(int cap)
    : k1(nullptr), k2(nullptr), v1(nullptr), v2(nullptr), used1(nullptr), used2(nullptr), capacity(0), sz(0), rehashes(0) {
    allocate(cap);
}
CuckooTable::~CuckooTable() {
    delete[] k1; delete[] k2; delete[] v1; delete[] v2; delete[] used1; delete[] used2;
}
int CuckooTable::size() const { return sz; }
int CuckooTable::tableSize() const { return capacity; }
int CuckooTable::rehashCount() const { return rehashes; }
bool CuckooTable::contains(const string& key) const {
    int i1 = hash1(key, capacity), i2 = hash2(key, capacity);
    return (used1[i1] && k1[i1] == key) || (used2[i2] && k2[i2] == key);
}
bool CuckooTable::get(const string& key, string& value) const {
    int i1 = hash1(key, capacity), i2 = hash2(key, capacity);
    if (used1[i1] && k1[i1] == key) { value = v1[i1]; return true; }
    if (used2[i2] && k2[i2] == key) { value = v2[i2]; return true; }
    return false;
}
bool CuckooTable::erase(const string& key) {
    int i1 = hash1(key, capacity), i2 = hash2(key, capacity);
    if (used1[i1] && k1[i1] == key) { used1[i1] = false; sz--; return true; }
    if (used2[i2] && k2[i2] == key) { used2[i2] = false; sz--; return true; }
    return false;
}
bool CuckooTable::insertRaw(string& key, string& value) {
    int table = 1;
    int limit = capacity * 4;
    for (int i = 0; i < limit; i++) {
        if (table == 1) {
            int idx = hash1(key, capacity);
            if (!used1[idx]) { k1[idx] = key; v1[idx] = value; used1[idx] = true; sz++; return true; }
            swap(k1[idx], key);
            swap(v1[idx], value);
            table = 2;
        } else {
            int idx = hash2(key, capacity);
            if (!used2[idx]) { k2[idx] = key; v2[idx] = value; used2[idx] = true; sz++; return true; }
            swap(k2[idx], key);
            swap(v2[idx], value);
            table = 1;
        }
    }
    return false; // key/value - вытесненная пара, для которой не нашлось места
}
void CuckooTable::rehash(int newCap, const string* extraKey, const string* extraValue) {
    vector<pair<string, string>> items;
    for (int i = 0; i < capacity; i++) {
        if (used1[i]) items.push_back(make_pair(k1[i], v1[i]));
        if (used2[i]) items.push_back(make_pair(k2[i], v2[i]));
    }
    if (extraKey) items.push_back(make_pair(*extraKey, *extraValue)); // "бездомная" пара не должна потеряться
    int cap = newCap < 8 ? 8 : newCap;
    rehashes++;
    while (true) {
        if (cap > CUCKOO_MAX_CAP) throw runtime_error("CuckooTable: не удалось разместить ключи");
        delete[] k1; delete[] k2; delete[] v1; delete[] v2; delete[] used1; delete[] used2;
        allocate(cap);
        bool ok = true;
        for (const auto& it : items) {
            string key = it.first, value = it.second;
            if (!insertRaw(key, value)) { ok = false; break; }
        }
        if (ok) return;
        cap *= 2; // не получилось - начинаем заново с полного списка пар и большей таблицей
    }
}
bool CuckooTable::insert(const string& key, const string& value) {
    int i1 = hash1(key, capacity), i2 = hash2(key, capacity);
    if (used1[i1] && k1[i1] == key) { v1[i1] = value; return false; }
    if (used2[i2] && k2[i2] == key) { v2[i2] = value; return false; }
    if ((long long)sz * 100 >= (long long)capacity * 70) rehash(capacity * 2, nullptr, nullptr);
    string curKey = key, curValue = value;
    if (!insertRaw(curKey, curValue)) rehash(capacity * 2, &curKey, &curValue);
    return true;
}
vector<string> CuckooTable::keys() const {
    vector<string> out;
    for (int i = 0; i < capacity; i++) {
        if (used1[i]) out.push_back(k1[i]);
        if (used2[i]) out.push_back(k2[i]);
    }
    return out;
}
void CuckooTable::print() const {
    cout << "cuckoo size=" << sz << " cap=" << capacity << endl;
    cout << "  t1:";
    for (int i = 0; i < capacity; i++) if (used1[i]) cout << " [" << i << "]" << k1[i] << "=" << v1[i];
    cout << "\n  t2:";
    for (int i = 0; i < capacity; i++) if (used2[i]) cout << " [" << i << "]" << k2[i] << "=" << v2[i];
    cout << endl;
}
