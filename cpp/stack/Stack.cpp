#include"Stack.h"
#include<iostream>
using namespace std;

Stack::Stack(){};
Stack::~Stack(){}

void Stack::push(string value){
    list.pushFront(value);
}

bool Stack::isEmpty() const {
    return list.GetSize() == 0;
}

string Stack::getAt(int index) const {
    if (index < 0 || index >= getSize()) {
        cout << "Ошибка, индекс за пределами диапазона" << endl;
        return "";
    }
    return list.getAt(index);
}

string Stack::pop(){
    if (!isEmpty()) {
        string val = list.getAt(0);
        list.popFront();
        return val;
    }
    return "";
}

string Stack::top() const{
    if(isEmpty()){
        cout<<"Стэк пуст"<<endl;
        return "";
    }
    return list.getAt(0);
}

int Stack::getSize() const{
    return list.GetSize();
}

void Stack::print() const{
    list.print();
}
