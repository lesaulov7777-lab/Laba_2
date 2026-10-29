#ifndef MYARRAY_H
#define MYARRAY_H

#include <iostream>
#include <string>
using namespace std;

class MyArray {
private:
    string* data;
    int capacity;
    int size;

public:
    MyArray(int cap = 2);
    MyArray(const MyArray& other);
    MyArray& operator=(const MyArray& other);
    ~MyArray();

    int GetSize() const;
    int getCapacity() const;
    void pushBack(string value);
    void insertAt(int index, string value);
    string getAt(int index) const;
    void setAt(int index, string value);
    void removeAt(int index);
    void print() const;
};

#endif
