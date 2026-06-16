#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>
#include <iostream>
#include "SequenceException.h"

template<typename T>
class LinkedList {
public:

    struct Node {
        T data_;
        Node *prev_;
        Node *next_;
        Node(const T &value);
    };

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
    T GetFirst() const;
    T GetLast() const;
    T Get(int index) const;
    int GetLength() const;
    bool IsEmpty() const;
    T operator[](int index) const;
    void printLinkedList() const;
    LinkedList<T> *Concat(LinkedList<T> *list);
    Node* GetHead() const {return head_;};
    Node* GetTail() const {return tail_;};

private:

    Node *head_;
    Node *tail_;
    unsigned int size_;

    Node *GetNodeAtIndex(int index) const;

};



template<typename T>
LinkedList<T>::LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

template<typename T>
LinkedList<T>::LinkedList(T *items, int count) : head_(nullptr), tail_(nullptr), size_(0) {
    if (count <= 0) return;
    head_ = new Node(items[0]);
    Node *temp = head_;
    for (int i = 1; i < count; i++) {
        Node *newNode = new Node(items[i]);
        newNode->prev_ = temp;
        temp->next_ = newNode;
        temp = temp->next_;
    }
    tail_ = temp;
    size_ = count;
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList &list) : head_(nullptr), tail_(nullptr), size_(0) {
    Node *otherCurrent = list.head_;
    while (otherCurrent) {
        push_back(otherCurrent->data_);
        otherCurrent = otherCurrent->next_;
    }
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList &&other) noexcept
    : head_(other.head_), tail_(other.tail_), size_(other.size_) {
    other.head_ = other.tail_ = nullptr;
    other.size_ = 0;
}

template<typename T>
LinkedList<T>::~LinkedList() {
    clear();
}

template<typename T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList &other) {
    if (this != &other) {
        clear();
        Node *otherCurrent = other.head_;
        while (otherCurrent) {
            push_back(otherCurrent->data_);
            otherCurrent = otherCurrent->next_;
        }
    }
    return *this;
}

template<typename T>
LinkedList<T> &LinkedList<T>::operator=(LinkedList &&other) noexcept {
    if (this != &other) {
        clear();
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template<typename T>
void LinkedList<T>::clear() {
    while (head_ != nullptr) {
        Node *temp = head_;
        head_ = head_->next_;
        delete temp;
    }
    tail_ = nullptr;
    size_ = 0;
}

template<typename T>
void LinkedList<T>::push_back(const T &value) {
    Node *newNode = new Node(value);
    if (!head_) {
        head_ = tail_ = newNode;
    } else {
        tail_->next_ = newNode;
        newNode->prev_ = tail_;
        tail_ = newNode;
    }
    size_++;
}

template<typename T>
void LinkedList<T>::push_front(const T &value) {
    Node *newNode = new Node(value);
    if (!head_) {
        head_ = tail_ = newNode;
    } else {
        head_->prev_ = newNode;
        newNode->next_ = head_;
        head_ = newNode;
    }
    size_++;
}

template<typename T>
void LinkedList<T>::pop_back() {
    if (!tail_) return;
    Node *temp = tail_;
    tail_ = tail_->prev_;
    if (tail_) {
        tail_->next_ = nullptr;
    } else {
        head_ = nullptr;
    }
    delete temp;
    size_--;
}

template<typename T>
void LinkedList<T>::insertAt(T item, int idx) {
    if (idx < 0 || idx > static_cast<int>(size_)) {
        throw IndexOutOFBoundsException("Index out of range");
    }
    if (idx == 0) {
        push_front(item);
        return;
    } else if (idx == static_cast<int>(size_)) {
        push_back(item);
        return;
    }
    Node *current = GetNodeAtIndex(idx);
    Node *newNode = new Node(item);
    Node *prev = current->prev_;
    prev->next_ = newNode;
    newNode->prev_ = prev;
    newNode->next_ = current;
    current->prev_ = newNode;
    size_++;
}

template<typename T>
LinkedList<T> *LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= static_cast<int>(size_) || startIndex > endIndex) {
        throw IndexOutOFBoundsException("Invalid indices");
    }
    LinkedList<T> *result = new LinkedList<T>();
    Node *current = GetNodeAtIndex(startIndex);
    for (int i = startIndex; i <= endIndex; i++) {
        result->push_back(current->data_);
        current = current->next_;
    }
    return result;
}

template<typename T>
T LinkedList<T>::GetFirst() const {
    if (head_ == nullptr) throw IndexOutOFBoundsException("Empty list");
    return head_->data_;
}

template<typename T>
T LinkedList<T>::GetLast() const {
    if (tail_ == nullptr) throw IndexOutOFBoundsException("Empty list");
    return tail_->data_;
}

template<typename T>
T LinkedList<T>::Get(int index) const {
    Node *temp = head_;
    int i = 0;
    while (temp && i < index) {
        temp = temp->next_;
        i++;
    }
    if (!temp) throw IndexOutOFBoundsException("Index out of range");
    return temp->data_;
}

template<typename T>
int LinkedList<T>::GetLength() const {
    return size_;
}

template<typename T>
bool LinkedList<T>::IsEmpty() const {
    return size_ == 0;
}

template<typename T>
T LinkedList<T>::operator[](int index) const {
    return Get(index);
}

template<typename T>
void LinkedList<T>::printLinkedList() const {
    std::cout << "[";
    Node *current = head_;
    while (current != nullptr) {
        std::cout << current->data_ << " ";
        current = current->next_;
    }
    std::cout << "]";
}

template<typename T>
LinkedList<T> *LinkedList<T>::Concat(LinkedList<T> *list) {
    LinkedList<T> *result = new LinkedList<T>();
    Node *current = head_;
    while (current) {
        result->push_back(current->data_);
        current = current->next_;
    }
    current = list->head_;
    while (current) {
        result->push_back(current->data_);
        current = current->next_;
    }
    return result;
}



template<typename T>
typename LinkedList<T>::Node *LinkedList<T>::GetNodeAtIndex(int index) const {
    if (head_ == nullptr) {
        throw SizeException("Empty list");
    }
    Node *temp = head_;
    for (int i = 0; i < index; i++) {
        temp = temp->next_;
    }
    return temp;
}





template<typename T>
LinkedList<T>::Node::Node(const T &value) : data_(value), prev_(nullptr), next_(nullptr) {}

#endif // LINKEDLIST_H
