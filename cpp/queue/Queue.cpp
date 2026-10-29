#include "Queue.h"
#include <iostream>
using namespace std;

QueueNode::QueueNode(string val) : value(val), next(nullptr) {}

Queue::Queue() : head(nullptr), tail(nullptr), size(0) {} //конструктор

Queue::~Queue() { //деструктор
    while (head != nullptr) {
        QueueNode* next = head->next;
        delete head;
        head = next;
    }
    tail = nullptr;
    size = 0;
}

void Queue::push(string value) { //добавление в конец
    QueueNode* node = new QueueNode(value);
    if (tail == nullptr) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    size++;
}

string Queue::pop() { //извлечение из начала
    if (head == nullptr) {
        return "";
    }
    QueueNode* node = head;
    string val = node->value;
    head = head->next;
    if (head == nullptr) {
        tail = nullptr;
    }
    delete node;
    size--;
    return val;
}

string Queue::getFront() const { //первый элемент
    if (head == nullptr) {
        cout << "Очередь пуста" << endl;
        return "";
    }
    return head->value;
}

bool Queue::isEmpty() const {
    return size == 0;
}

int Queue::getSize() const {
    return size;
}

string Queue::getAt(int index) const { //элемент по индексу
    if (index < 0 || index >= size) {
        cout << "Ошибка, индекс за пределами диапазона" << endl;
        return "";
    }
    QueueNode* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->value;
}

void Queue::print() const { //вывод
    cout << "[";
    for (QueueNode* current = head; current != nullptr; current = current->next) {
        cout << current->value;
        if (current->next != nullptr) {
            cout << " -> ";
        }
    }
    cout << "]" << endl;
}
