#include"DoublyLinkedList.h"
#include<iostream>
using namespace std;

DoublyNode::DoublyNode(string val) {
    value = val;
    next = nullptr;
    prev = nullptr;
}

DoublyLinkedList::DoublyLinkedList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

DoublyLinkedList::~DoublyLinkedList(){
    DoublyNode* current = head;
    while (current != nullptr) {
        DoublyNode* next = current->next;
        delete current;
        current = next;
    }
}

int DoublyLinkedList::getSize() const{
    return size;
}

void DoublyLinkedList::print() const{
    DoublyNode* current = head;
    cout<<"[";
    while (current != nullptr) {
        cout << current->value;
        if (current->next != nullptr) {
            cout << " <-> ";
        }
        current = current->next;
    }
    cout<<"]";
}

void DoublyLinkedList::pushFront(string value){

    if(head==nullptr){
        DoublyNode* new_node = new DoublyNode(value);
        head = new_node;
        tail = new_node;
        size++;
    }else{
        DoublyNode* new_node = new DoublyNode(value);
        new_node->next = head;
        head->prev = new_node;
        head=new_node;
        size++;
    }
}

void DoublyLinkedList::pushBack(string value){
    if(head==nullptr){
        DoublyNode* new_node = new DoublyNode(value);
        head = new_node;
        tail = new_node;
        size++;
    }else{
        DoublyNode* new_node = new DoublyNode(value);
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;
        size++;
    }
}

DoublyNode* DoublyLinkedList::pushBackNode(string value){
    pushBack(value);
    return tail;
}

DoublyNode* DoublyLinkedList::front() const{
    return head;
}

void DoublyLinkedList::removeByNode(DoublyNode* node){
    if(node!=nullptr) unlinkNode(node);
}

void DoublyLinkedList::popFront(){
    if(getSize() == 0){
        cout<<"Ошибка, список пуст"<<endl;
        return;
    }
    if(head == tail){
        delete head;
        head = nullptr;
        tail = nullptr;
        size--;
        return;
    }
    DoublyNode* tmp = head;
    head = head->next;
    head->prev = nullptr;
    delete tmp;
    size--;
    return;
}

void DoublyLinkedList::popBack(){
    if(head==nullptr){
        cout<<"Ошибка, список пуст"<<endl;
        return;
    }
    if(head==tail){
        delete head;
        head = nullptr;
        tail = nullptr;
        size--;
        return;
    }
    DoublyNode* tmp = tail;
    tail = tail->prev;
    tail->next = nullptr;
    delete tmp;
    size--;
    return;
}

DoublyNode* DoublyLinkedList::find(string value) const{
    DoublyNode* current = head;
    while(current!=nullptr){
        if(current->value == value){
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void DoublyLinkedList::insertAfter(string target, string value){
    DoublyNode* node = find(target);
    if(node==nullptr){
        cout<<"Узел с таким значением не найден"<<endl;
        return;
    }
    if(node==tail){
        pushBack(value);
        return;
    }
    DoublyNode* new_node = new DoublyNode(value);
    new_node->next=node->next;
    new_node->prev=node;
    node->next->prev = new_node;
    node->next=new_node;
    size++;
    return;
}

void DoublyLinkedList::insertBefore(string target, string value){
    DoublyNode* node = find(target);
    if(node==nullptr){
        cout<<"Узел с таким значением не найден"<<endl;
        return;
    }
    if(node==head){
        pushFront(value);
        return;
    }
    DoublyNode* new_node = new DoublyNode(value);
    new_node->next = node;
    new_node->prev = node->prev;
    node->prev->next = new_node;
    node->prev=new_node;
    size++;
    return;
}

void DoublyLinkedList::unlinkNode(DoublyNode* node){
    if(node==head){
        popFront();
        return;
    }
    if(node==tail){
        popBack();
        return;
    }
    DoublyNode* tmpn = node->next;
    DoublyNode* tmpp = node->prev;
    node->prev->next = tmpn;
    node->next->prev = tmpp;
    delete node;
    size--;
    return;
}

void DoublyLinkedList::removeNode(string value){
    if(head==nullptr){
        cout<<"Ошибка, список пуст"<<endl;
        return;
    }
    DoublyNode* node = find(value);
    if(node==nullptr){
        cout<<"Узел с таким значением не найден"<<endl;
        return;
    }
    unlinkNode(node);
    return;
}

void DoublyLinkedList::removeByValue(string value){
    removeNode(value);
}

void DoublyLinkedList::removeAfter(string target){
    DoublyNode* node = find(target);
    if(node==nullptr){
        cout<<"Узел с таким значением не найден"<<endl;
        return;
    }
    if(node->next==nullptr){
        cout<<"После заданного узла нет узла"<<endl;
        return;
    }
    unlinkNode(node->next);
}

void DoublyLinkedList::removeBefore(string target){
    DoublyNode* node = find(target);
    if(node==nullptr){
        cout<<"Узел с таким значением не найден"<<endl;
        return;
    }
    if(node->prev==nullptr){
        cout<<"Перед заданным узлом нет узла"<<endl;
        return;
    }
    unlinkNode(node->prev);
}

string DoublyLinkedList::getAt(int index) const{
    if(index<0 || index>=size){
        cout<<"Ошибка, индекс за пределами даопозна"<<endl;
        return "";
    }
    DoublyNode* current = head;
    for(int i=0; i<index; i++){
        current=current->next;
    }
    return current->value;
}
