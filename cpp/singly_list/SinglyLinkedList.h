#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#include <string>

struct SinglyNode {
    std::string value;
    SinglyNode* next;
    SinglyNode(std::string val);
};

class SinglyLinkedList {
private:
    SinglyNode* head;
    int size;

public:
    SinglyLinkedList();
    ~SinglyLinkedList();

    int GetSize() const;
    void print() const;

    void pushFront(std::string value);
    void pushBack(std::string value);

    void popFront();
    void popBack();
    void removeByValue(std::string value);
    void removeNode(std::string value);
    void removeAfter(std::string target);
    void removeBefore(std::string target);

    SinglyNode* find(std::string value) const;
    bool contains(std::string value) const;
    void insertAfter(std::string target, std::string value);
    void insertBefore(std::string target, std::string value);
    std::string getAt(int index) const;
    SinglyNode* getHead() const;
};

#endif
