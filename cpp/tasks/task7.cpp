// Задание 7, вариант 2: LFU-кэш. Ввод: cap Q, затем Q запросов вида "SET x y" или "GET x".
#include "Tasks.h"
#include "Safe.h"
#include <vector>
#include "LFU.h"
using namespace std;

namespace {
const long long MAX_Q = 1000000;

struct Query {
    bool isSet;
    string x, y;
};

bool isKeyword(const string& s) {
    string u = upper(s);
    return u == "SET" || u == "GET";
}
}

int runTask7(istream& in) {
    string tc, tq;
    if (!(in >> tc >> tq)) throw InputError("ожидалось: cap Q, затем Q запросов (SET x y / GET x)");
    long long cap = parseInt(tc, "емкость кэша", 1, MAX_Q);
    long long Q = parseInt(tq, "количество запросов", 0, MAX_Q);

    // сначала проверяем весь ввод, и только потом выполняем - при ошибке частичного вывода не будет
    vector<Query> qs;
    qs.reserve((size_t)Q);
    for (long long i = 1; i <= Q; i++) {
        string cmd;
        string where = "запрос №" + to_string(i);
        if (!(in >> cmd)) throw InputError("ожидалось " + to_string(Q) + " запросов, получено " + to_string(i - 1));
        Query q;
        cmd = upper(cmd);
        if (cmd == "SET") {
            q.isSet = true;
            if (!(in >> q.x >> q.y)) throw InputError(where + ": SET требует два аргумента (SET x y)");
            if (isKeyword(q.x) || isKeyword(q.y)) throw InputError(where + ": SET требует два аргумента, а найдена команда");
        } else if (cmd == "GET") {
            q.isSet = false;
            if (!(in >> q.x)) throw InputError(where + ": GET требует аргумент (GET x)");
            if (isKeyword(q.x)) throw InputError(where + ": GET требует аргумент, а найдена команда");
        } else {
            throw InputError(where + ": неизвестный запрос \"" + cmd + "\" (ожидалось SET или GET)");
        }
        qs.push_back(q);
    }
    string extra;
    if (in >> extra) cerr << "Предупреждение: данные после " << Q << " запросов проигнорированы" << endl;

    LFUCache cache((int)cap);
    vector<string> answers;
    for (const Query& q : qs) {
        if (q.isSet) cache.set(q.x, q.y);
        else answers.push_back(cache.get(q.x));
    }
    if (answers.empty()) {
        cout << "(запросов GET не было)" << endl;
        return 0;
    }
    for (size_t i = 0; i < answers.size(); i++) cout << (i ? " " : "") << answers[i];
    cout << endl;
    return 0;
}
