#include "MyArray.h"

MyArray::MyArray(int cap) {
    capacity = cap;
    size = 0;
    data = new string[capacity];
}

MyArray::MyArray(const MyArray& other) {
    capacity = other.capacity;
    size = other.size;
    data = new string[capacity];
    for (int i = 0; i < size; ++i) data[i] = other.data[i];
}

MyArray& MyArray::operator=(const MyArray& other) {
    if (this == &other) return *this;
    delete[] data;
    capacity = other.capacity;
    size = other.size;
    data = new string[capacity];
    for (int i = 0; i < size; ++i) data[i] = other.data[i];
    return *this;
}

MyArray::~MyArray() {
    delete[] data;
}

int MyArray::GetSize() const {
    return size;
}

int MyArray::getCapacity() const {
    return capacity;
}

void MyArray::pushBack(string value) {
    if (size >= capacity) {
        capacity *= 2;
        string* newData = new string[capacity];
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    data[size++] = value;
}

void MyArray::insertAt(int index, string value) {
    if (index < 0 || index > size) {
        cout << "Индекс вне диапазона" << endl;
        return;
    }
    if (size == capacity) {
        int newCapacity = capacity * 2;
        string* newData = new string[newCapacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    for (int i = size; i > index; i--) {
        data[i] = data[i - 1];
    }
    data[index] = value;
    size++;
}

string MyArray::getAt(int index) const {
    if (index < 0 || index >= size) {
        cout << "Индекс вне диапазона" << endl;
        return "";
    }
    return data[index];
}

void MyArray::setAt(int index, string value) {
    if (index < 0 || index >= size) {
        cout << "Индекс вне диапазона" << endl;
    } else {
        data[index] = value;
    }
}

void MyArray::removeAt(int index) {
    if (index < 0 || index >= size) {
        cout << "Индекс вне диапазона" << endl;
        return;
    }
    for (int i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
}

void MyArray::print() const {
    cout << "[";
    for (int i = 0; i < size; i++) {
        cout << data[i];
        if (i < size - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}
