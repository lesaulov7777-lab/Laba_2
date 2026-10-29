// Задание 5, вариант 8: глубина каждого добавленного в двоичное дерево поиска элемента.
// Ввод - последовательность целых, оканчивающаяся нулём (ноль в последовательность не входит).
#include "Tasks.h"
#include "Safe.h"
#include <vector>
#include "BST.h"
using namespace std;

int runTask5(istream& in) {
    BST t;
    vector<int> depths;
    string tok;
    bool any = false, terminated = false;
    while (in >> tok) {
        any = true;
        long long x = parseInt(tok, "элемент", INT_MIN, INT_MAX);
        if (x == 0) { terminated = true; break; }
        int d = t.insert((int)x);
        if (d > 0) depths.push_back(d); // 0 - элемент уже был в дереве
    }
    if (!any) throw InputError("пустой ввод: введите целые числа, оканчивающиеся нулём");
    if (!terminated) cerr << "Предупреждение: последовательность не завершена нулём" << endl;

    if (depths.empty()) {
        cout << "(ни одного нового элемента не добавлено)" << endl;
        return 0;
    }
    for (size_t i = 0; i < depths.size(); i++) cout << (i ? " " : "") << depths[i];
    cout << endl;
    return 0;
}
