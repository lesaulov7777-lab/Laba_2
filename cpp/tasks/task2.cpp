// Задание 2: АТД Множество. Команды SETADD / SETDEL / SET_AT (+ SETPRINT), все операции O(1) (в среднем).
#include "Tasks.h"
#include "Safe.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include "Set.h"
using namespace std;
namespace fs = std::filesystem;

namespace {
typedef map<string, unique_ptr<Set>> SetMap;

const char* USAGE =
    "формат запроса: SETADD <имя> <ключ> | SETDEL <имя> <ключ> | SET_AT <имя> <ключ> | SETPRINT <имя>";

SetMap loadSets(const string& path) {
    SetMap out;
    error_code ec;
    if (!fs::exists(path, ec)) return out; // файла ещё нет - пустая база
    if (fs::is_directory(path, ec)) throw IoError("\"" + path + "\" является каталогом, а не файлом");
    ifstream in(path);
    if (!in.is_open()) throw IoError("не удалось открыть файл для чтения: " + path);

    string line;
    int lineNo = 0;
    while (getline(in, line)) {
        ++lineNo;
        istringstream ss(line);
        string tag, name, k;
        if (!(ss >> tag)) continue;
        if (tag != "SET" || !(ss >> name)) {
            cerr << "Предупреждение: строка " << lineNo << " файла пропущена (ожидалось \"SET имя ключи...\")" << endl;
            continue;
        }
        unique_ptr<Set>& s = out[name];
        if (!s) s.reset(new Set());
        while (ss >> k) s->add(k);
    }
    return out;
}

// Запись через временный файл: при сбое старый файл остаётся целым
void saveSets(const string& path, const SetMap& sets) {
    string tmp = path + ".tmp";
    {
        ofstream out(tmp);
        if (!out.is_open()) throw IoError("не удалось открыть файл для записи: " + tmp);
        for (const auto& p : sets) {
            vector<string> keys = p.second->keys();
            sort(keys.begin(), keys.end());
            out << "SET " << p.first;
            for (const string& k : keys) out << " " << k;
            out << "\n";
        }
        out.flush();
        if (!out.good()) {
            out.close();
            remove(tmp.c_str());
            throw IoError("ошибка записи в файл: " + tmp);
        }
    }
    error_code ec;
    fs::rename(tmp, path, ec);
    if (ec) {
        remove(tmp.c_str());
        throw IoError("не удалось сохранить файл " + path + ": " + ec.message());
    }
}
}

int runTask2(const string& file, const string& query) {
    if (file.empty()) throw InputError("не указан путь к файлу (--file)");
    istringstream qs(query);
    vector<string> tok;
    string t;
    while (qs >> t) tok.push_back(t);
    if (tok.empty()) throw InputError(string("пустой запрос; ") + USAGE);

    string cmd = upper(tok[0]);
    bool needKey = (cmd == "SETADD" || cmd == "SETDEL" || cmd == "SET_AT");
    if (!needKey && cmd != "SETPRINT")
        throw InputError("неизвестная команда \"" + tok[0] + "\"; " + USAGE);
    size_t want = needKey ? 3 : 2;
    if (tok.size() < want)
        throw InputError("недостаточно аргументов для " + cmd + "; " + USAGE);
    if (tok.size() > want)
        throw InputError("лишние аргументы для " + cmd + " (значения с пробелами не поддерживаются); " + USAGE);

    const string& name = tok[1];
    SetMap sets = loadSets(file);
    auto it = sets.find(name);

    if (cmd == "SETADD") {
        if (it == sets.end()) it = sets.emplace(name, unique_ptr<Set>(new Set())).first;
        bool added = it->second->add(tok[2]);
        if (added) saveSets(file, sets); // сначала сохраняем: "OK" печатаем только если данные реально записаны
        cout << (added ? "OK" : "EXISTS") << endl;
    } else if (cmd == "SETDEL") {
        bool removed = (it != sets.end()) && it->second->del(tok[2]);
        if (removed) saveSets(file, sets);
        cout << (removed ? "OK" : "MISSING") << endl;
    } else if (cmd == "SET_AT") {
        cout << ((it != sets.end() && it->second->at(tok[2])) ? "TRUE" : "FALSE") << endl;
    } else { // SETPRINT
        vector<string> keys;
        if (it != sets.end()) keys = it->second->keys();
        sort(keys.begin(), keys.end());
        cout << "{";
        for (size_t i = 0; i < keys.size(); i++) cout << (i ? ", " : "") << keys[i];
        cout << "}" << endl;
    }
    return 0;
}
