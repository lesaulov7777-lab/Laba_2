// Задание 3, вариант 2: схожие подмножества. Разбить множество натуральных чисел на два
// непересекающихся подмножества с минимальной разницей сумм. Множество - из задания 2.
#include "Tasks.h"
#include "Safe.h"
#include <cstdlib>
#include <vector>
#include "Set.h"
using namespace std;

namespace {
const long long MAX_HALF = 20000000;   // память под таблицу динамики
const long long MAX_WORK = 400000000;  // n * (sum / 2)

void printSet(const char* name, const vector<int>& v, long long sum) {
    cout << name << " = {";
    for (size_t i = 0; i < v.size(); i++) cout << (i ? ", " : "") << v[i];
    cout << "} (сумма " << sum << ")\n";
}
}

int runTask3(istream& in) {
    Set s; // повторяющиеся числа отбрасываются: это множество
    vector<int> nums;
    string tok;
    long long total = 0;
    int idx = 0;
    while (in >> tok) {
        ++idx;
        long long v = parseInt(tok, "элемент №" + to_string(idx), 1, 1000000000LL);
        if (s.add(to_string(v))) {
            nums.push_back((int)v);
            total += v;
        }
    }
    int n = (int)nums.size();
    if (n == 0) throw InputError("пустое множество: введите натуральные числа через пробел");

    long long half = total / 2;
    if (half > MAX_HALF || (long long)n * half > MAX_WORK)
        throw InputError("слишком большая задача: сумма элементов " + to_string(total) + " (n=" + to_string(n) +
                         "); допустимо: сумма/2 <= " + to_string(MAX_HALF) + " и n*сумма/2 <= " + to_string(MAX_WORK));

    // dp[sum] = индекс последнего элемента, которым достигается сумма sum; -1 - не достигается
    vector<int> dp((size_t)half + 1, -1);
    dp[0] = 0;
    for (int i = 0; i < n; i++)
        for (long long sum = half; sum >= nums[i]; --sum)
            if (dp[sum] == -1 && dp[sum - nums[i]] != -1) dp[sum] = i;

    long long best = 0;
    for (long long sum = 0; sum <= half; sum++)
        if (dp[sum] != -1) best = sum;

    vector<char> inA(n, 0);
    for (long long cur = best; cur > 0;) {
        int i = dp[cur];
        inA[i] = 1;
        cur -= nums[i];
    }
    vector<int> a, b;
    long long sa = 0, sb = 0;
    for (int i = 0; i < n; i++) {
        if (inA[i]) { a.push_back(nums[i]); sa += nums[i]; }
        else        { b.push_back(nums[i]); sb += nums[i]; }
    }
    printSet("A", a, sa);
    printSet("B", b, sb);
    cout << "Разница сумм: " << llabs(sb - sa) << endl;
    return 0;
}
