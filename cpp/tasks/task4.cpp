// Задание 4, вариант 2: поиск подмассивов с заданной суммой. Данные хранятся в MyArray из ЛР1.
#include "Tasks.h"
#include "Safe.h"
#include <vector>
#include "MyArray.h"
using namespace std;

namespace {
const size_t MAX_N = 20000;
const long long LIM = 1000000000000LL; // |элемент| <= 1e12: суммы не переполнят long long
const long long MAX_PRINT = 1000;
}

int runTask4(istream& in) {
    vector<string> tokens;
    string t;
    while (in >> t) tokens.push_back(t);
    if (tokens.size() < 2)
        throw InputError("ожидалось: элементы массива через пробел, последним числом - целевая сумма");

    long long target = parseInt(tokens.back(), "целевая сумма", -LIM, LIM);
    size_t n = tokens.size() - 1;
    if (n > MAX_N) throw InputError("слишком много элементов: " + to_string(n) + " (максимум " + to_string(MAX_N) + ")");

    MyArray arr(8);
    vector<long long> pref(n + 1, 0);
    for (size_t i = 0; i < n; i++) {
        long long v = parseInt(tokens[i], "элемент №" + to_string(i + 1), -LIM, LIM);
        arr.pushBack(to_string(v));
        pref[i + 1] = pref[i] + v;
    }

    long long found = 0;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i; j < n; j++) {
            if (pref[j + 1] - pref[i] != target) continue;
            if (++found > MAX_PRINT) continue;
            cout << "[";
            for (size_t k = i; k <= j; k++) cout << (k > i ? ", " : "") << arr.getAt((int)k);
            cout << "]\n";
        }
    }
    if (found == 0) cout << "подмассивов с суммой " << target << " нет\n";
    else if (found > MAX_PRINT) cout << "... показаны первые " << MAX_PRINT << " из " << found << "\n";
    else cout << "Найдено подмассивов: " << found << "\n";
    return 0;
}
