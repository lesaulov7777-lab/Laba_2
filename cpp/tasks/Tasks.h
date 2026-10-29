#ifndef TASKS_H
#define TASKS_H

#include <iostream>
#include <string>

// Каждая функция возвращает 0 при успехе. Ошибки входных данных - исключение InputError,
// ошибки файлов - IoError (их перехватывает интерфейс, программа не падает).
int runTask1(std::istream& in);                                         // Стек: DFS графа/дерева (вариант 6)
int runTask2(const std::string& file, const std::string& query);        // АТД Множество
int runTask3(std::istream& in);                                         // Множество: схожие подмножества (вариант 2)
int runTask4(std::istream& in);                                         // Массив: поиск суммы (вариант 2)
int runTask5(std::istream& in);                                         // Двоичное дерево: глубина (вариант 8)
int runTask6(std::istream& in);                                         // Хеш-таблица: цепочки + кукушка, Блум (6.1.2, 6.2.8)
int runTask7(std::istream& in);                                         // LFU-кэш (вариант 2)

#endif
