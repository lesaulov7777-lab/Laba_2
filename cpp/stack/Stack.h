#ifndef STACK_H
#define STACK_H

#include <string>
#include "SinglyLinkedList.h"

class Stack {
private:
    SinglyLinkedList list;

public:
    Stack();
    ~Stack();

    std::string getAt(int index) const;
    void push(std::string value);
    std::string pop();
    std::string top() const;
    bool isEmpty() const;
    int getSize() const;
    void print() const;
};

#endif
