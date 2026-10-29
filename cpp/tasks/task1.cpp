// Задание 1, вариант 6: обход деревьев и графов в глубину (DFS) с помощью стека из ЛР1.
#include "Tasks.h"
#include "Safe.h"
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include "SinglyLinkedList.h"
#include "MyArray.h"
#include "Stack.h"
#include "CompleteBinaryTree.h"
using namespace std;

namespace {
// Вставка в полное дерево из ЛР1 идёт обходом в ширину (O(n)), поэтому размер ограничен
const int MAX_TREE = 20000;
}

int runTask1(istream& in) {
    map<string, unique_ptr<SinglyLinkedList>> adj; // списки смежности
    MyArray order(8);                              // вершины в порядке появления
    CompleteBinaryTree tree;
    string mode, start, line;
    int lineNo = 0, treeCount = 0;
    bool declared = false;

    auto ensure = [&](const string& v) {
        if (adj.find(v) == adj.end()) {
            adj[v].reset(new SinglyLinkedList());
            order.pushBack(v);
        }
    };

    while (getline(in, line)) {
        ++lineNo;
        string where = "строка " + to_string(lineNo);
        istringstream ss(line);
        string first;
        if (!(ss >> first) || first[0] == '#') continue;

        string up = upper(first);
        if (up == "GRAPH" || up == "TREE") {
            string extra;
            if (ss >> extra) throw InputError(where + ": после " + up + " не должно быть других слов");
            mode = up;
            declared = true;
            continue;
        }
        if (up == "START") {
            string v, extra;
            if (!(ss >> v)) throw InputError(where + ": START требует имя вершины");
            if (ss >> extra) throw InputError(where + ": START принимает ровно одну вершину");
            start = v;
            continue;
        }
        if (mode.empty()) throw InputError(where + ": данные до объявления GRAPH или TREE");

        if (mode == "TREE") {
            string tok = first;
            do {
                int v = (int)parseInt(tok, where + ", значение дерева", INT_MIN, INT_MAX);
                if (++treeCount > MAX_TREE)
                    throw InputError(where + ": слишком большое дерево (максимум " + to_string(MAX_TREE) + " узлов)");
                tree.insert(v);
            } while (ss >> tok);
        } else {
            string a = first, b, extra;
            if (ss >> b) {
                if (ss >> extra) throw InputError(where + ": ребро задаётся двумя вершинами \"a b\"");
                ensure(a);
                ensure(b);
                if (!adj[a]->contains(b)) adj[a]->pushBack(b);
            } else {
                ensure(a); // одиночная вершина без рёбер
            }
        }
    }

    if (!declared) throw InputError("нет данных: ожидается секция GRAPH и/или TREE");
    if (adj.empty() && tree.getSize() == 0) throw InputError("секции GRAPH/TREE пусты");

    if (!adj.empty()) {
        if (start.empty()) start = order.getAt(0);
        else if (adj.find(start) == adj.end())
            throw InputError("START: вершина \"" + start + "\" отсутствует в графе");

        Stack st;
        st.push(start);
        map<string, bool> seen;
        cout << "DFS-граф (стек):";
        while (!st.isEmpty()) {
            string v = st.pop();
            if (seen[v]) continue;
            seen[v] = true;
            cout << " " << v;
            vector<string> neigh;
            for (SinglyNode* n = adj[v]->getHead(); n; n = n->next) neigh.push_back(n->value);
            // кладём в обратном порядке, чтобы первым обработался первый сосед
            for (int i = (int)neigh.size() - 1; i >= 0; --i)
                if (!seen[neigh[i]]) st.push(neigh[i]);
        }
        cout << endl;
    }

    if (tree.getSize() > 0) {
        // дерево полное: у узла с индексом i (порядок по уровням) дети 2i+1 и 2i+2
        vector<int> vals = tree.getValues();
        int n = (int)vals.size();
        Stack st;
        st.push("0");
        cout << "DFS-дерево (стек):";
        while (!st.isEmpty()) {
            int i = (int)parseInt(st.pop(), "индекс узла", 0, n - 1);
            cout << " " << vals[i];
            if (2 * i + 2 < n) st.push(to_string(2 * i + 2));
            if (2 * i + 1 < n) st.push(to_string(2 * i + 1));
        }
        cout << endl;
    }
    return 0;
}
