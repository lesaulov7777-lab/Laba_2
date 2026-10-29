// Задание 6. 6.1 (вариант 2): хеш-таблицы с методом цепочек и хешированием кукушки + эмпирический анализ (BENCH).
//           6.2 (вариант 8): фильтр Блума, интегрированный в хеш-таблицу (BloomTable).
#include "Tasks.h"
#include "Safe.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include "HashTable.h"
#include "Bloom.h"
using namespace std;

namespace {
const long long MAX_BENCH = 1000000;

// Токен "ключ=значение"; без "=" значением считается сам ключ
void splitPair(const string& tok, string& key, string& value) {
    size_t eq = tok.find('=');
    if (eq == string::npos) { key = tok; value = tok; }
    else { key = tok.substr(0, eq); value = tok.substr(eq + 1); }
}

template <class T>
void demo(T& table, const vector<string>& pairs) {
    vector<string> keys;
    for (const string& p : pairs) {
        string k, v;
        splitPair(p, k, v);
        if (k.empty()) throw InputError("пустой ключ в \"" + p + "\"");
        table.insert(k, v);
        keys.push_back(k);
    }
    table.print();
    for (const string& k : keys) {
        string v;
        bool ok = table.get(k, v);
        cout << "get " << k << " = " << (ok ? v : string("(нет)")) << endl;
    }
}

typedef chrono::steady_clock Clock;
double ms(Clock::time_point a, Clock::time_point b) {
    return chrono::duration<double, milli>(b - a).count();
}

void bench(int n) {
    vector<string> keys(n), miss(n), vals(n);
    for (int i = 0; i < n; i++) {
        keys[i] = "key" + to_string(i);
        miss[i] = "miss" + to_string(i);
        vals[i] = "value" + to_string(i);
    }
    cout << fixed << setprecision(2);
    cout << "Эмпирический анализ, n = " << n << " ключей (оба метода стартуют с таблицы на 8 ячеек)\n";

    {
        ChainTable t(8);
        Clock::time_point t0 = Clock::now();
        for (int i = 0; i < n; i++) t.insert(keys[i], vals[i]);
        Clock::time_point t1 = Clock::now();
        long long hit = 0, bad = 0;
        { string v; for (int i = 0; i < n; i++) hit += (t.get(keys[i], v) && v == vals[i]); }
        Clock::time_point t2 = Clock::now();
        for (const string& k : miss) bad += t.contains(k);
        Clock::time_point t3 = Clock::now();
        cout << "Цепочки : вставка " << ms(t0, t1) << " мс, поиск есть " << ms(t1, t2) << " мс, поиск нет "
             << ms(t2, t3) << " мс | корзин " << t.bucketCount() << ", заполнение "
             << (double)t.size() / t.bucketCount() << ", макс. цепочка " << t.maxChainLength()
             << ", перестроек " << t.rehashCount() << (hit == n && bad == 0 ? "" : " | ОШИБКА КОРРЕКТНОСТИ") << "\n";
    }
    {
        CuckooTable t(8);
        Clock::time_point t0 = Clock::now();
        for (int i = 0; i < n; i++) t.insert(keys[i], vals[i]);
        Clock::time_point t1 = Clock::now();
        long long hit = 0, bad = 0;
        { string v; for (int i = 0; i < n; i++) hit += (t.get(keys[i], v) && v == vals[i]); }
        Clock::time_point t2 = Clock::now();
        for (const string& k : miss) bad += t.contains(k);
        Clock::time_point t3 = Clock::now();
        cout << "Кукушка : вставка " << ms(t0, t1) << " мс, поиск есть " << ms(t1, t2) << " мс, поиск нет "
             << ms(t2, t3) << " мс | ячеек " << 2 * t.tableSize() << ", заполнение "
             << (double)t.size() / (2.0 * t.tableSize()) << ", макс. проб при поиске 2"
             << ", перестроек " << t.rehashCount() << (hit == n && bad == 0 ? "" : " | ОШИБКА КОРРЕКТНОСТИ") << "\n";
    }
    {
        long long mBits = 10LL * n;
        int k = 7;
        BloomTable bt(n < 16 ? 16 : n, (int)mBits, k);
        for (int i = 0; i < n; i++) bt.insert(keys[i], vals[i]);
        long long rejected = 0;
        bool rej = false;
        for (const string& key : miss) {
            { string v; bt.get(key, v, rej); }
            if (rej) rejected++;
        }
        double fp = 100.0 * (n - rejected) / n;
        double theory = 100.0 * pow(1.0 - exp(-(double)k * n / (double)mBits), k);
        cout << "Блум    : бит на ключ 10, хеш-функций " << k << " | отсеяно несуществующих " << rejected << " из "
             << n << ", ложноположительных " << fp << "% (теория " << theory << "%)\n";
    }
}
}

int runTask6(istream& in) {
    BloomTable bloomTable(16, 256, 3);
    string line;
    int lineNo = 0, errors = 0;

    while (getline(in, line)) {
        ++lineNo;
        istringstream ss(line);
        string cmd;
        if (!(ss >> cmd) || cmd[0] == '#') continue;
        cmd = upper(cmd);
        try {
            if (cmd == "CHAIN" || cmd == "CUCKOO") {
                vector<string> keys;
                string k;
                while (ss >> k) keys.push_back(k);
                if (keys.empty()) throw InputError(cmd + " требует хотя бы одну пару ключ=значение");
                if (cmd == "CHAIN") { ChainTable t(8); demo(t, keys); }
                else                { CuckooTable t(8); demo(t, keys); }
            } else if (cmd == "BLOOM") {
                cout << "BloomTable: ADD <ключ> <значение> / HAS <ключ> / DEL <ключ>" << endl;
            } else if (cmd == "ADD" || cmd == "HAS" || cmd == "DEL") {
                string k, v, extra;
                if (!(ss >> k)) throw InputError(cmd + " требует ключ");
                if (cmd == "ADD" && !(ss >> v)) throw InputError("ADD требует ключ и значение: ADD <ключ> <значение>");
                if (ss >> extra) throw InputError(cmd + (cmd == "ADD" ? " принимает ключ и значение" : " принимает ровно один ключ"));
                if (cmd == "ADD") {
                    cout << (bloomTable.insert(k, v) ? "OK" : "UPDATED (значение ключа заменено)") << endl;
                } else if (cmd == "HAS") {
                    bool reject = false;
                    string val;
                    bool ok = bloomTable.get(k, val, reject);
                    if (reject) cout << "FALSE (фильтр Блума отсёк запрос, таблица не просматривалась)" << endl;
                    else if (ok) cout << "TRUE (значение: " << val << ")" << endl;
                    else cout << "FALSE (фильтр пропустил, но в таблице ключа нет)" << endl;
                } else {
                    cout << (bloomTable.erase(k) ? "OK (бит в фильтре Блума не снимается)" : "MISSING") << endl;
                }
            } else if (cmd == "BENCH") {
                string num, extra;
                if (!(ss >> num)) throw InputError("BENCH требует число ключей, например: BENCH 100000");
                if (ss >> extra) throw InputError("BENCH принимает одно число");
                bench((int)parseInt(num, "число ключей", 1, MAX_BENCH));
            } else {
                throw InputError("неизвестная команда \"" + cmd + "\" (CHAIN, CUCKOO, BLOOM, ADD, HAS, DEL, BENCH)");
            }
        } catch (const InputError& e) {
            cerr << "Ошибка (строка " << lineNo << "): " << e.what() << endl;
            errors++;
        }
    }
    return errors ? 1 : 0;
}
