#include "Bloom.h"

BloomFilter::BloomFilter(int mBits, int kHashes) : m(mBits < 64 ? 64 : mBits), k(kHashes < 2 ? 3 : kHashes) {
    bits.assign((m + 63) / 64, 0);
}
std::vector<int> BloomFilter::positions(const std::string& key) const {
    int h1 = hash1(key, m);
    int h2 = hash2(key, m);
    if (h2 == 0) h2 = 1;
    std::vector<int> out(k);
    for (int i = 0; i < k; i++) out[i] = (h1 + i * h2) % m;
    return out;
}
void BloomFilter::add(const std::string& key) {
    for (int p : positions(key)) bits[p / 64] |= 1ull << (p % 64);
}
bool BloomFilter::maybeContains(const std::string& key) const {
    for (int p : positions(key)) {
        if ((bits[p / 64] & (1ull << (p % 64))) == 0) return false;
    }
    return true;
}

BloomTable::BloomTable(int cap, int mBits, int k) : inner(cap), bloom(mBits, k) {}
bool BloomTable::insert(const std::string& key, const std::string& value) {
    bloom.add(key);
    return inner.insert(key, value);
}
bool BloomTable::get(const std::string& key, std::string& value, bool& bloomReject) const {
    if (!bloom.maybeContains(key)) {
        bloomReject = true;
        return false;
    }
    bloomReject = false;
    return inner.get(key, value);
}
bool BloomTable::contains(const std::string& key, bool& bloomReject) const {
    if (!bloom.maybeContains(key)) {
        bloomReject = true;
        return false;
    }
    bloomReject = false;
    return inner.contains(key);
}
bool BloomTable::erase(const std::string& key) { return inner.erase(key); }
