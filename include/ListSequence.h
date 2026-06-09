#ifndef LISTSEQUENCE_H
#define LISTSEQUENCE_H

#include "Sequence.h"
#include "LinkedList.h"
#include <functional>

template<typename T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T> data;
public:
    ListSequence();
    ListSequence(int size, T defaultValue);
    ListSequence(T *items, int count);
    ListSequence(const ListSequence<T> &other);
    ListSequence(const LinkedList<T> &list);
    ~ListSequence() = default;

    ListSequence<T> &operator=(const ListSequence<T> &other);

    int GetLength() const override;
    T Get(int index) const override;
    T GetFirst() const override;
    T GetLast() const override;

    void Set(int index, const T &item) override;
    void Append(const T &item) override;
    void Prepend(const T &item) override;
    void InsertAt(const T &item, int index) override;
    void RemoveAt(int index) override;

    Sequence<T> *GetSubsequence(int startIndex, int endIndex) const override;

    T &operator[](int index) override;
    const T &operator[](int index) const override;

    Sequence<T> *Map(std::function<T(const T &)> function) const override;
    T Reduce(std::function<T(const T &, const T &)> function, const T &initial) const override;
    Sequence<T> *Where(std::function<bool(const T &)> predicate) const override;
    Sequence<T> *Concat(Sequence<T> *other) const override;
    void Slice(int start, int count, Sequence<T> *insertSequence = nullptr) override;

    void printSeq() const;

    class ListSequenceIterator {
    private:
        const typename LinkedList<T>::Node* node_;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ListSequenceIterator(const typename LinkedList<T>::Node* node = nullptr);
        reference operator*() const;
        pointer operator->() const;
        ListSequenceIterator& operator++();
        ListSequenceIterator operator++(int);
        ListSequenceIterator& operator--();
        ListSequenceIterator operator--(int);
        bool operator==(const ListSequenceIterator& other) const;
        bool operator!=(const ListSequenceIterator& other) const;
    };

    using iterator = ListSequenceIterator;
    using const_iterator = ListSequenceIterator;

    const_iterator begin() const;
    const_iterator end() const;
};

template<typename T>
ListSequence<T>::ListSequence() : data(LinkedList<T>()) {}

template<typename T>
ListSequence<T>::ListSequence(int size, T defaultValue) {
    for (int i = 0; i < size; i++) {
        Append(defaultValue);
    }
}

template<typename T>
ListSequence<T>::ListSequence(T *items, int count) : data(LinkedList<T>(items, count)) {}

template<typename T>
ListSequence<T>::ListSequence(const ListSequence<T> &other) : data(LinkedList<T>(other.data)) {}

template<typename T>
ListSequence<T>::ListSequence(const LinkedList<T> &list) : data(LinkedList<T>(list)) {}

template<typename T>
ListSequence<T> &ListSequence<T>::operator=(const ListSequence<T> &other) {
    if (this != &other){
        data = other.data;
    }
    return *this;
}

template<typename T>
int ListSequence<T>::GetLength() const {
    return data.GetLength();
}

template<typename T>
T ListSequence<T>::Get(int index) const {
    if(index < 0 || index >= GetLength()){
        throw IndexOutOFBoundsException("Index out of range");
    }
    return data.Get(index);
}

template<typename T>
T ListSequence<T>::GetFirst() const {
    if(GetLength() == 0){
        throw SizeException("Empty list");
    }
    return data.GetFirst();
}

template<typename T>
T ListSequence<T>::GetLast() const {
    if(GetLength() == 0){
        throw SizeException("Empty list");
    }
    return data.GetLast();
}

template<typename T>
void ListSequence<T>::Set(int index, const T &item){
    if(index < 0 || index >= GetLength()){
        throw IndexOutOFBoundsException("Index out of range");
    }
    LinkedList<T> newList;
    for(int i = 0; i < index; i++){
        newList.push_back(data.Get(i));
    }
    newList.push_back(item);
    for(int i = index + 1; i < GetLength(); i++){
        newList.push_back(data.Get(i));
    }
    data = newList;
}

template<typename T>
void ListSequence<T>::Append(const T &item){
    data.push_back(item);
}

template<typename T>
void ListSequence<T>::Prepend(const T &item){
    data.push_front(item);
}

template<typename T>
void ListSequence<T>::InsertAt(const T &item, int index){
    if(index < 0 || index >= data.GetLength()){
        throw IndexOutOFBoundsException("Index out of range");
    }
    data.insertAt(item, index);
}

template<typename T>
void ListSequence<T>::RemoveAt(int index){
    if(index < 0 || index >= data.GetLength()){
        throw IndexOutOFBoundsException("Index out of range");
    }
    LinkedList<T> newList;
    for(int i = 0; i < index; i++){
        newList.push_back(data.Get(i));
    }
    for(int i = index + 1; i < GetLength(); i++){
        newList.push_back(data.Get(i));
    }
    data = newList;
}

template<typename T>
Sequence<T> *ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex){
        throw IndexOutOFBoundsException("Indexes out of range");
    }
    LinkedList<T> *sublist = data.GetSubList(startIndex, endIndex);
    ListSequence<T> *result = new ListSequence<T>(*sublist);
    delete sublist;
    return result;
}

template<typename T>
T &ListSequence<T>::operator[](int index) {
    if (index < 0 || index >= GetLength()){
        throw IndexOutOFBoundsException("Index out of range");
    }
    static T temp;
    temp = data.Get(index);
    return temp;
}

template<typename T>
const T &ListSequence<T>::operator[](int index) const {
    return const_cast<ListSequence<T> *>(this)->operator[](index);
}

template<typename T>
Sequence<T> *ListSequence<T>::Map(std::function<T(const T &)> function) const {
    ListSequence<T> *result = new ListSequence<T>();
    for (int i = 0; i < GetLength(); i++) {
        result->Append(function(data.Get(i)));
    }
    return result;
}

template<typename T>
T ListSequence<T>::Reduce(std::function<T(const T &, const T &)> function, const T &initial) const {
    T result = initial;
    for (int i = 0; i < GetLength(); i++) {
        result = function(result, data.Get(i));
    }
    return result;
}

template<typename T>
Sequence<T> *ListSequence<T>::Where(std::function<bool(const T &)> predicate) const {
    ListSequence<T> *result = new ListSequence<T>();
    for (int i = 0; i < GetLength(); i++){
        if (predicate(data.Get(i))){
            result->Append(data.Get(i));
        }
    }
    return result;
}

template<typename T>
Sequence<T> *ListSequence<T>::Concat(Sequence<T> *other) const {
    ListSequence<T> *result = new ListSequence<T>();
    for (int i = 0; i < GetLength(); i++){
        result->Append(data.Get(i));
    }
    for (int i = 0; i < other->GetLength(); i++){
        result->Append(other->Get(i));
    }
    return result;
}

template<typename T>
void ListSequence<T>::Slice(int start, int count, Sequence<T> *insertSequence) {
    int length = GetLength();
    if (start < 0){
        start = length + start;
    }
    if (start < 0 || start >= length) {
        throw IndexOutOFBoundsException("Start index out of range");
    }
    if (count < 0 || start + count > length) {
        throw IndexOutOFBoundsException("Invalid count");
    }

    LinkedList<T> newData;
    for (int i = 0; i < start; i++) {
        newData.push_back(data.Get(i));
    }
    if (insertSequence != nullptr){
        for (int i = 0; i < insertSequence->GetLength(); i++){
            newData.push_back(insertSequence->Get(i));
        }
    }
    for (int i = start + count; i < length; i++) {
        newData.push_back(data.Get(i));
    }
    data = newData;
}

template<typename T>
void ListSequence<T>::printSeq() const {
    data.printLinkedList();
}

template<typename T>
ListSequence<T>::ListSequenceIterator::ListSequenceIterator(const typename LinkedList<T>::Node* node)
    : node_(node) {}

template<typename T>
typename ListSequence<T>::ListSequenceIterator::reference
ListSequence<T>::ListSequenceIterator::operator*() const {
    return node_->data;
}

template<typename T>
typename ListSequence<T>::ListSequenceIterator::pointer
ListSequence<T>::ListSequenceIterator::operator->() const {
    return &(node_->data);
}

template<typename T>
typename ListSequence<T>::ListSequenceIterator&
ListSequence<T>::ListSequenceIterator::operator++() {
    node_ = node_->next;
    return *this;
}

template<typename T>
typename ListSequence<T>::ListSequenceIterator
ListSequence<T>::ListSequenceIterator::operator++(int) {
    ListSequenceIterator tmp = *this;
    ++(*this);
    return tmp;
}

template<typename T>
typename ListSequence<T>::ListSequenceIterator&
ListSequence<T>::ListSequenceIterator::operator--() {
    node_ = node_->prev_;
    return *this;
}

template<typename T>
typename ListSequence<T>::ListSequenceIterator
ListSequence<T>::ListSequenceIterator::operator--(int) {
    ListSequenceIterator tmp = *this;
    --(*this);
    return tmp;
}

template<typename T>
bool ListSequence<T>::ListSequenceIterator::operator==(const ListSequenceIterator& other) const {
    return node_ == other.node_;
}

template<typename T>
bool ListSequence<T>::ListSequenceIterator::operator!=(const ListSequenceIterator& other) const {
    return !(*this == other);
}

template<typename T>
typename ListSequence<T>::const_iterator ListSequence<T>::begin() const {
    return const_iterator(data.GetHead());
}

template<typename T>
typename ListSequence<T>::const_iterator ListSequence<T>::end() const {
    return const_iterator(nullptr);
}


#endif // LISTSEQUENCE_H