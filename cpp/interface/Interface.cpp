#include "Interface.h"
#include "Safe.h"
#include "Tasks.h"
#include <functional>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace {

struct TaskInfo {
    const char* title;
    const char* help;
};

const TaskInfo TASKS[7] = {
    {"Стек: обход графа/дерева в глубину (вариант 6)",
     "Секции GRAPH и/или TREE.\n"
     "  GRAPH      затем строки \"a b\" (ориентированное ребро a->b) или \"a\" (одиночная вершина)\n"
     "  START v    вершина, с которой начать обход (по умолчанию - первая)\n"
     "  TREE       затем строки с целыми числами (полное двоичное дерево, до 20000 узлов)\n"
     "Пример:  GRAPH / A B / A C / B D / START A / TREE / 1 2 3 4 5"},
    {"АТД Множество: SETADD / SETDEL / SET_AT", ""},
    {"Множество: схожие подмножества (вариант 2)",
     "Натуральные числа через пробел (повторы отбрасываются). Результат: два подмножества с минимальной\n"
     "разницей сумм.  Пример:  5 8 1 14 7"},
    {"Массив: поиск подмассивов с заданной суммой (вариант 2)",
     "Элементы массива (целые) через пробел, ПОСЛЕДНЕЕ число - целевая сумма.\n"
     "Пример:  4 -7 1 5 -4 0 -3 2 4 1 5"},
    {"Двоичное дерево: глубина добавляемых элементов (вариант 8)",
     "Целые числа через пробел, последовательность оканчивается нулём.  Пример:  7 3 2 1 9 5 4 6 8 0"},
    {"Хеш-таблица: цепочки + кукушка (6.1.2), фильтр Блума (6.2.8)",
     "Команды, по одной на строку:\n"
     "  CHAIN k1 k2 ...    таблица на цепочках: построить, вывести, проверить ключи\n"
     "  CUCKOO k1 k2 ...   хеширование кукушки: то же самое\n"
     "  ADD k / HAS k / DEL k   таблица с фильтром Блума (состояние сохраняется между строками)\n"
     "  BENCH n            эмпирический анализ методов на n ключах (1..1000000)"},
    {"LFU-кэш (вариант 2)",
     "Первая строка: cap Q (ёмкость кэша и число запросов), далее Q запросов SET x y / GET x.\n"
     "Пример:  2 7 SET 1 10 SET 2 20 GET 1 GET 1 GET 2 SET 3 30 GET 2"}};

const char* SET_HELP =
    "Запросы: SETADD <имя> <ключ> | SETDEL <имя> <ключ> | SET_AT <имя> <ключ> | SETPRINT <имя>";

void printUsage(ostream& out) {
    out << "Лабораторная работа №2. Алгоритмы и АТД (вариант: 1.6  3.2  4.2  5.8  6.1.2  6.2.8  7.2)\n"
           "\n"
           "Использование:\n"
           "  lab2                                  интерактивное меню\n"
           "  lab2 --file <файл> --query '<запрос>' запрос к любому заданию; файл нужен только для множеств\n"
           "  lab2 --task N                         задания 1,3..7: данные читаются из стандартного ввода\n"
           "  lab2 --help                           эта справка\n"
           "\n"
           "Запросы (--query), ';' разделяет строки данных:\n"
           "  SETADD|SETDEL|SET_AT <имя> <ключ>, SETPRINT <имя>   задание 2 (нужен --file)\n"
           "  DFS GRAPH;A B;A C;START A;TREE;1 2 3                задание 1\n"
           "  SIMILAR 5 8 1 14 7                                  задание 3\n"
           "  SUBARRAY 4 -7 1 5 -4 0 -3 2 4 1 5                   задание 4 (последнее число - сумма)\n"
           "  DEPTH 7 3 2 1 9 5 4 6 8 0                           задание 5\n"
           "  HASH CHAIN a b c;CUCKOO a b c;BENCH 1000            задание 6\n"
           "  LFU 2 SET 1 10 SET 2 20 GET 1 GET 3                 задание 7 (первое число - ёмкость)\n"
           "\n"
           "Коды возврата: 0 - успех, 1 - ошибка запроса или входных данных, 2 - ошибка файла, 3 - внутренняя ошибка.\n";
}

// Единая защита: любая ошибка превращается в сообщение и код возврата, программа не падает
int guarded(const function<int()>& f) {
    try {
        return f();
    } catch (const InputError& e) {
        cerr << "Ошибка ввода: " << e.what() << endl;
        return 1;
    } catch (const IoError& e) {
        cerr << "Ошибка файла: " << e.what() << endl;
        return 2;
    } catch (const bad_alloc&) {
        cerr << "Ошибка: недостаточно памяти для такого объёма данных" << endl;
        return 3;
    } catch (const exception& e) {
        cerr << "Внутренняя ошибка: " << e.what() << endl;
        return 3;
    } catch (...) {
        cerr << "Внутренняя ошибка: неизвестное исключение" << endl;
        return 3;
    }
}

int dispatch(int task, istream& in) {
    switch (task) {
        case 1: return runTask1(in);
        case 3: return runTask3(in);
        case 4: return runTask4(in);
        case 5: return runTask5(in);
        case 6: return runTask6(in);
        case 7: return runTask7(in);
        default: throw InputError("задание " + to_string(task) + " не принимает данные из стандартного ввода");
    }
}

// Запрос --query: первое слово выбирает задание, остальное - его данные (';' = перевод строки)
int runQuery(const string& file, bool haveFile, const string& query) {
    istringstream qs(query);
    string kw;
    if (!(qs >> kw)) throw InputError("пустой запрос; см. --help");
    string cmd = upper(kw);
    if (cmd == "SETADD" || cmd == "SETDEL" || cmd == "SET_AT" || cmd == "SETPRINT") {
        if (!haveFile) throw InputError("не указан --file");
        return runTask2(file, query);
    }
    int task = 0;
    if (cmd == "DFS") task = 1;
    else if (cmd == "SIMILAR") task = 3;
    else if (cmd == "SUBARRAY") task = 4;
    else if (cmd == "DEPTH") task = 5;
    else if (cmd == "HASH") task = 6;
    else if (cmd == "LFU") task = 7;
    else throw InputError("неизвестная команда \"" + kw + "\" (см. --help)");

    string rest;
    getline(qs, rest);
    for (char& c : rest)
        if (c == ';') c = '\n';
    if (task == 7) {
        // в запросе число запросов не пишется: считаем SET/GET сами и подставляем после ёмкости
        istringstream ts(rest);
        string cap, t;
        if (!(ts >> cap)) throw InputError("LFU: ожидалась ёмкость кэша и запросы SET/GET");
        long long q = 0;
        while (ts >> t) {
            string u = upper(t);
            if (u == "SET" || u == "GET") ++q;
        }
        rest = cap + " " + to_string(q) + " " + rest.substr(rest.find(cap) + cap.size());
    }
    istringstream in(rest + "\n");
    return dispatch(task, in);
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Читает строки до пустой строки или конца ввода. Возвращает false, если ввод закончился сразу (EOF)
bool readBlock(string& text) {
    text.clear();
    string line;
    bool gotAny = false;
    while (getline(cin, line)) {
        gotAny = true;
        if (trim(line).empty()) return true;
        text += line + "\n";
    }
    return gotAny;
}

void menuSets() {
    cout << "Файл с множествами [sets.data]: " << flush;
    string file;
    if (!getline(cin, file)) return;
    file = trim(file);
    if (file.empty()) file = "sets.data";
    cout << SET_HELP << "\nПустая строка - вернуться в меню.\n";
    while (true) {
        cout << "> " << flush;
        string q;
        if (!getline(cin, q)) return;
        q = trim(q);
        if (q.empty()) return;
        guarded([&] { return runTask2(file, q); });
    }
}

int runMenu() {
    while (true) {
        cout << "\n=== Лабораторная работа №2 ===\n";
        for (int i = 0; i < 7; i++) cout << (i + 1) << ") " << TASKS[i].title << "\n";
        cout << "0) Выход\nВыберите пункт: " << flush;

        string line;
        if (!getline(cin, line)) { cout << endl; return 0; }
        line = trim(line);
        if (line.empty()) continue;

        long long choice = 0;
        try {
            choice = parseInt(line, "пункт меню", 0, 7);
        } catch (const InputError& e) {
            cerr << "Ошибка: " << e.what() << " (нужно число от 0 до 7)" << endl;
            continue;
        }
        if (choice == 0) return 0;
        if (choice == 2) { menuSets(); continue; }

        cout << "\n" << TASKS[choice - 1].help << "\nВведите данные (пустая строка - конец ввода):\n";
        string text;
        if (!readBlock(text)) { cout << endl; return 0; }
        istringstream in(text);
        guarded([&] { return dispatch((int)choice, in); });
    }
}

// Значение параметра не должно отсутствовать или быть следующим флагом
bool takeValue(int argc, char** argv, int& i, string& out) {
    if (i + 1 >= argc) return false;
    string v = argv[i + 1];
    if (v.rfind("--", 0) == 0) return false;
    out = v;
    ++i;
    return true;
}
}

int runCli(int argc, char** argv) {
    string file, query, taskStr;
    bool haveFile = false, haveQuery = false, haveTask = false;

    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "--help" || a == "-h") {
            printUsage(cout);
            return 0;
        } else if (a == "--file" || a == "--query" || a == "--task") {
            string v;
            if (!takeValue(argc, argv, i, v)) {
                cerr << "Ошибка: отсутствует значение для " << a << "\n\n";
                printUsage(cerr);
                return 1;
            }
            if (a == "--file") { file = v; haveFile = true; }
            else if (a == "--query") { query = v; haveQuery = true; }
            else { taskStr = v; haveTask = true; }
        } else {
            cerr << "Ошибка: неизвестный аргумент \"" << a << "\"\n\n";
            printUsage(cerr);
            return 1;
        }
    }

    return guarded([&]() -> int {
        if (haveTask) {
            int task = (int)parseInt(taskStr, "номер задания (--task)", 1, 7);
            if (task == 2) {
                if (!haveFile || !haveQuery) throw InputError("для задания 2 нужны параметры --file и --query");
                return runTask2(file, query);
            }
            if (haveFile || haveQuery) throw InputError("параметры --file/--query относятся только к заданию 2");
            return dispatch(task, cin);
        }
        if (haveFile && !haveQuery) throw InputError("не указан --query");
        if (haveQuery) return runQuery(file, haveFile, query);
        return runMenu();
    });
}
