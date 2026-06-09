#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>
#include <iostream>
#include "Exceptions.h"

template<typename T>
class LinkedList{
public:
    struct Node {
        T data;
        Node *prev;
        Node *next;
        Node(const T &value);
    };
private:
    Node *head;
    Node *tail;
    int size;

    Node *GetNodeAtIndex(int index) const {
        if (head == nullptr) {
            throw SizeException("Empty list");
        }
        Node *temp = head;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        return temp;
    };

public:
    LinkedList();
    LinkedList(T *items, int count);
    LinkedList(const LinkedList &list);
    LinkedList(LinkedList &&other) noexcept;
    ~LinkedList();

    LinkedList &operator=(const LinkedList &other);
    LinkedList &operator=(LinkedList &&other) noexcept;

    void clear();
    void push_back(const T &value);
    void push_front(const T &value);
    void pop_back();
    void insertAt(T item, int idx);

    LinkedList<T> *GetSubList(int startIndex, int endIndex) const;

    Node* GetHead() const {
        return head;
    };
    Node* GetTail() const {
        return tail;
    };

    T GetFirst() const;
    T GetLast() const;
    T Get(int index) const;
    int GetLength() const;
    bool IsEmpty() const;

    T operator[](int index) const;

    void printLinkedList() const;
    LinkedList<T> *Concat(LinkedList<T> *list);
};

template<typename T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

template<typename T>
LinkedList<T>::LinkedList(T *items, int count) : head(nullptr), tail(nullptr), size(0) {
    if(count <= 0){
        return;
    }
    head = new Node(items[0]);
    Node *temp = head;
    for(int i = 1; i < count; i++){
        Node *newNode = new Node(items[i]);
        newNode->prev = temp;
        temp->next = newNode;
        temp = temp->next;
    }
    tail = temp;
    size = count;
}


template<typename T>
LinkedList<T>::LinkedList(const LinkedList &list) : head(nullptr), tail(nullptr), size(0) {
    Node *other = list.head;
    while(other){
        push_back(other->data);
        other = other->next;
    }
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList &&other) noexcept : head(other.head), tail(other.tail), size(other.size) {
    other.head = other.tail = nullptr;
    other.size = 0;
}

template<typename T>
LinkedList<T>::~LinkedList() {
    clear();
}

template<typename T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList &other) {
    if (this != &other) {
        clear();
        Node *otherCurrent = other.head;
        while (otherCurrent) {
            push_back(otherCurrent->data);
            otherCurrent = otherCurrent->next;
        }
    }
    return *this;
}

template<typename T>
LinkedList<T> &LinkedList<T>::operator=(LinkedList &&other) noexcept {
    if (this != &other) {
        clear();
        head = other.head;
        tail = other.tail;
        size = other.size;
        other.head = other.tail = nullptr;
        other.size = 0;
    }
    return *this;
}

template<typename T>
void LinkedList<T>::clear() {
    while (head != nullptr) {
        Node *temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    size = 0;
}

template<typename T>
void LinkedList<T>::push_back(const T &value) {
    Node *newNode = new Node(value);
    if(!head){
        head = tail = newNode;
    } else{
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    size++;
}

template<typename T>
void LinkedList<T>::push_front(const T &value) {
    Node *newNode = new Node(value);
    if(!head){
        head = tail = newNode;
    } else{
        head->prev = newNode;
        newNode->next = head;
        head = newNode;
    }
    size++;
}

template<typename T>
void LinkedList<T>::pop_back() {
    if(!tail){
        return;
    }
    Node *temp = tail;
    tail = tail->prev;
    if(tail){
        tail->next = nullptr;
    } else{
        head = nullptr;
    }
    delete temp;
    size--;
}

template<typename T>
void LinkedList<T>::insertAt(T item, int idx) {
    if(idx < 0 || idx > size){
        throw IndexOutOFBoundsException("Index out of range");
    }
    if(idx == 0){
        push_front(item);
        return;
    } else if(idx == size){
        push_back(item);
        return;
    }
    Node *current = GetNodeAtIndex(idx);
    Node *newNode = new Node(item);
    Node *prev = current->prev;
    prev->next = newNode;
    newNode->prev = prev;
    newNode->next = current;
    current->prev = newNode;
    size++;
}

template<typename T>
LinkedList<T> *LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= size || startIndex > endIndex) {
        throw IndexOutOFBoundsException("Invalid indices");
    }
    LinkedList<T> *result = new LinkedList<T>();
    Node *current = GetNodeAtIndex(startIndex);
    for (int i = startIndex; i <= endIndex; i++) {
        result->push_back(current->data);
        current = current->next;
    }
    return result;
}

template<typename T>
T LinkedList<T>::GetFirst() const {
    if (head == nullptr){
        throw IndexOutOFBoundsException("Empty list");
    }
    return head->data;
}

template<typename T>
T LinkedList<T>::GetLast() const {
    if (tail == nullptr){
        throw IndexOutOFBoundsException("Empty list");
    }
    return tail->data;
}

template<typename T>
T LinkedList<T>::Get(int index) const {
    Node *temp = head;
    int i = 0;
    while (temp && i < index) {
        temp = temp->next;
        i++;
    }
    if (!temp){
        throw IndexOutOFBoundsException("Index out of range");
    }
    return temp->data;
}

template<typename T>
int LinkedList<T>::GetLength() const {
    return size;
}

template<typename T>
bool LinkedList<T>::IsEmpty() const {
    return size == 0;
}

template<typename T>
T LinkedList<T>::operator[](int index) const {
    return Get(index);
}

template<typename T>
void LinkedList<T>::printLinkedList() const {
    std::cout << "[";
    Node *current = head;
    while (current != nullptr) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << "]";
}

template<typename T>
LinkedList<T> *LinkedList<T>::Concat(LinkedList<T> *list) {
    LinkedList<T> *result = new LinkedList<T>();
    Node *current = head;
    while (current) {
        result->push_back(current->data);
        current = current->next;
    }
    current = list->head;
    while (current) {
        result->push_back(current->data);
        current = current->next;
    }
    return result;
}

template<typename T>
LinkedList<T>::Node::Node(const T &value) : data(value), prev(nullptr), next(nullptr) {}

#endif // LINKEDLIST_H