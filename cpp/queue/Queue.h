#ifndef QUEUE_H
#define QUEUE_H

#include <string>

struct QueueNode {
    std::string value;
    QueueNode* next;
    QueueNode(std::string val);
};

class Queue {
private:
    QueueNode* head;
    QueueNode* tail;
    int size;

public:
    Queue();
    ~Queue();

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    void push(std::string value);
    std::string pop();
    std::string getFront() const;
    bool isEmpty() const;
    int getSize() const;

    std::string getAt(int index) const;
    void print() const;
};

#endif
