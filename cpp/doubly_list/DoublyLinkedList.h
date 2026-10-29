#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include <string>

struct DoublyNode {
    std::string value;
    DoublyNode* next;
    DoublyNode* prev;
    DoublyNode(std::string val);
};

class DoublyLinkedList {
private:
    DoublyNode* head;
    DoublyNode* tail;
    int size;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    int getSize() const;

    void pushFront(std::string value);
    void pushBack(std::string value);
    void insertAfter(std::string target, std::string value);
    void insertBefore(std::string target, std::string value);

    void popFront();
    void popBack();
    void removeByValue(std::string value);
    void removeNode(std::string value);
    void removeAfter(std::string target);
    void removeBefore(std::string target);

    DoublyNode* pushBackNode(std::string value);   // O(1), возвращает созданный узел
    DoublyNode* front() const;                     // первый узел или nullptr
    void removeByNode(DoublyNode* node);           // O(1), узел должен принадлежать этому списку
    DoublyNode* find(std::string value) const;
    std::string getAt(int index) const;
    void print() const;

private:
    void unlinkNode(DoublyNode* node);
};

#endif
