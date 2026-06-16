#ifndef LINKEDLISTSEQUENCE_H
#define LINKEDLISTSEQUENCE_H

#include "Sequence.h"
#include "LinkedList.h"
#include <functional>

template<typename T>
class LinkedListSequence : public Sequence<T> {
private:
    LinkedList<T> data_;

public:
    LinkedListSequence();
    LinkedListSequence(int size, T defaultValue);
    LinkedListSequence(T *items, int count);
    LinkedListSequence(int size);
    LinkedListSequence(const LinkedListSequence<T> &other);
    LinkedListSequence(const LinkedList<T> &list);
    ~LinkedListSequence() = default;

    LinkedListSequence<T> &operator=(const LinkedListSequence<T> &other);

    int GetLength() const override;
    T Get(int index) const override;
    void Set(int index, const T &item) override;
    void Append(const T &item) override;
    void Prepend(const T &item) override;
    void InsertAt(const T &item, int index) override;
    void RemoveAt(int index) override;
    T GetFirst() const override;
    T GetLast() const override;
    Sequence<T> *GetSubsequence(int startIndex, int endIndex) const override;

    T &operator[](int index) override;
    const T &operator[](int index) const override;

    Sequence<T> *Map(std::function<T(const T &)> function) const override;
    T Reduce(std::function<T(const T &, const T &)> function, const T &initial) const override;
    Sequence<T> *Where(std::function<bool(const T &)> predicate) const override;
    Sequence<T> *Concat(Sequence<T> *other) const override;
    void Slice(int start, int count, Sequence<T> *insertSequence = nullptr) override;
    void Sort() override;
    int FindSubsequence(const Sequence<T>& pattern) const override;
    void Merge(const Sequence<T>& other) override;

    void printSeq() const;

    class LinkedListSequenceIterator {
    private:
        const typename LinkedList<T>::Node* node_;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        LinkedListSequenceIterator(const typename LinkedList<T>::Node* node = nullptr);
        reference operator*() const;
        pointer operator->() const;
        LinkedListSequenceIterator& operator++();
        LinkedListSequenceIterator operator++(int);
        LinkedListSequenceIterator& operator--();
        LinkedListSequenceIterator operator--(int);
        bool operator==(const LinkedListSequenceIterator& other) const;
        bool operator!=(const LinkedListSequenceIterator& other) const;
    };

    using iterator = LinkedListSequenceIterator;
    using const_iterator = LinkedListSequenceIterator;

    const_iterator begin() const;
    const_iterator end() const;
};



template<typename T>
LinkedListSequence<T>::LinkedListSequence() : data_(LinkedList<T>()) {}

template<typename T>
LinkedListSequence<T>::LinkedListSequence(int size, T defaultValue) {
    for (int i = 0; i < size; i++) Append(defaultValue);
}

template<typename T>
LinkedListSequence<T>::LinkedListSequence(T *items, int count) : data_(LinkedList<T>(items, count)) {}



template<typename T>
LinkedListSequence<T>::LinkedListSequence(int size) {
    for (int i = 0; i < size; ++i) Append(T());
}

template<typename T>
LinkedListSequence<T>::LinkedListSequence(const LinkedListSequence<T> &other) : data_(LinkedList<T>(other.data_)) {}

template<typename T>
LinkedListSequence<T>::LinkedListSequence(const LinkedList<T> &list) : data_(LinkedList<T>(list)) {}

template<typename T>
LinkedListSequence<T> &LinkedListSequence<T>::operator=(const LinkedListSequence<T> &other) {
    if (this != &other) data_ = other.data_;
    return *this;
}

template<typename T>
int LinkedListSequence<T>::GetLength() const {
    return data_.GetLength();
}

template<typename T>
T LinkedListSequence<T>::Get(int index) const {
    if (index < 0 || index >= GetLength()) throw IndexOutOFBoundsException("Index out of range");
    return data_.Get(index);
}

template<typename T>
void LinkedListSequence<T>::Set(int index, const T &item) {
    if (index < 0 || index >= GetLength()) throw IndexOutOFBoundsException("Index out of range");
    LinkedList<T> newList;
    for (int i = 0; i < index; i++) newList.push_back(data_.Get(i));
    newList.push_back(item);
    for (int i = index + 1; i < GetLength(); i++) newList.push_back(data_.Get(i));
    data_ = newList;
}

template<typename T>
void LinkedListSequence<T>::Append(const T &item) {
    data_.push_back(item);
}

template<typename T>
void LinkedListSequence<T>::Prepend(const T &item) {
    data_.push_front(item);
}

template<typename T>
void LinkedListSequence<T>::InsertAt(const T &item, int index) {
    if (index < 0 || index > data_.GetLength()) throw IndexOutOFBoundsException("Index out of range");
    data_.insertAt(item, index);
}

template<typename T>
void LinkedListSequence<T>::RemoveAt(int index) {
    if (index < 0 || index >= data_.GetLength()) throw IndexOutOFBoundsException("Index out of range");
    LinkedList<T> newList;
    for (int i = 0; i < index; i++) newList.push_back(data_.Get(i));
    for (int i = index + 1; i < GetLength(); i++) newList.push_back(data_.Get(i));
    data_ = newList;
}

template<typename T>
T LinkedListSequence<T>::GetFirst() const {
    if (GetLength() == 0) throw SizeException("Empty list");
    return data_.GetFirst();
}

template<typename T>
T LinkedListSequence<T>::GetLast() const {
    if (data_.GetLength() == 0) throw SizeException("Empty list");
    return data_.GetLast();
}

template<typename T>
Sequence<T> *LinkedListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex)
        throw IndexOutOFBoundsException("Indexes out of range");
    auto *sublist = data_.GetSubList(startIndex, endIndex);
    auto *result = new LinkedListSequence<T>(*sublist);
    delete sublist;
    return result;
}

template<typename T>
T &LinkedListSequence<T>::operator[](int index) {
    if (index < 0 || index >= GetLength()) throw IndexOutOFBoundsException("Index out of range");
    static T temp;
    temp = data_.Get(index);
    return temp;
}

template<typename T>
const T &LinkedListSequence<T>::operator[](int index) const {
    return const_cast<LinkedListSequence<T> *>(this)->operator[](index);
}

template<typename T>
Sequence<T> *LinkedListSequence<T>::Map(std::function<T(const T &)> function) const {
    auto *result = new LinkedListSequence<T>();
    for (int i = 0; i < GetLength(); i++) result->Append(function(data_.Get(i)));
    return result;
}

template<typename T>
T LinkedListSequence<T>::Reduce(std::function<T(const T &, const T &)> function, const T &initial) const {
    T result = initial;
    for (int i = 0; i < GetLength(); i++) result = function(result, data_.Get(i));
    return result;
}

template<typename T>
Sequence<T> *LinkedListSequence<T>::Where(std::function<bool(const T &)> predicate) const {
    auto *result = new LinkedListSequence<T>();
    for (int i = 0; i < GetLength(); i++)
        if (predicate(data_.Get(i))) result->Append(data_.Get(i));
    return result;
}

template<typename T>
Sequence<T> *LinkedListSequence<T>::Concat(Sequence<T> *other) const {
    auto *result = new LinkedListSequence<T>();
    for (int i = 0; i < GetLength(); i++) result->Append(data_.Get(i));
    for (int i = 0; i < other->GetLength(); i++) result->Append(other->Get(i));
    return result;
}

template<typename T>
void LinkedListSequence<T>::Slice(int start, int count, Sequence<T> *insertSequence) {
    int length = GetLength();
    if (start < 0) start = length + start;
    if (start < 0 || start >= length) throw IndexOutOFBoundsException("Start index out of range");
    if (count < 0 || start + count > length) throw IndexOutOFBoundsException("Invalid count");

    LinkedList<T> newData;
    for (int i = 0; i < start; i++) newData.push_back(data_.Get(i));
    if (insertSequence != nullptr)
        for (int i = 0; i < insertSequence->GetLength(); i++) newData.push_back(insertSequence->Get(i));
    for (int i = start + count; i < length; i++) newData.push_back(data_.Get(i));
    data_ = newData;
}





template<typename T>
void LinkedListSequence<T>::Sort() {
    if (GetLength() <= 1) return;
    std::vector<T> temp;
    temp.reserve(GetLength());
    for (int i = 0; i < GetLength(); ++i)
        temp.push_back(Get(i));
    std::sort(temp.begin(), temp.end());
    for (int i = 0; i < GetLength(); ++i)
        Set(i, temp[i]);
}

template<typename T>
int LinkedListSequence<T>::FindSubsequence(const Sequence<T>& pattern) const{
    int lengthOfPattern = pattern.GetLength();
    int startIndex = 0;

    while (startIndex+lengthOfPattern<=GetLength()){
        bool flag=true;
        for (int i=0;i<lengthOfPattern;i++){
            if (pattern[i]!=this->Get(startIndex+i)){
                flag = false;
                break;
            }
        }
        if (flag){ return startIndex;}
        startIndex++;
    }

    return -1;
}

template<typename T>
void LinkedListSequence<T>::Merge(const Sequence<T>& other) {

    LinkedListSequence<T> result;

    int i = 0;
    int j = 0;
    int lenThis = GetLength();
    int lenOther = other.GetLength();


    while (i < lenThis && j < lenOther) {
        if (Get(i) <= other.Get(j)) {
            result.Append(Get(i));
            ++i;
        } else {
            result.Append(other.Get(j));
            ++j;
        }
    }

    while (i < lenThis) {
        result.Append(Get(i));
        ++i;
    }

    while (j < lenOther) {
        result.Append(other.Get(j));
        ++j;
    }

    *this = std::move(result);
}


template<typename T>
void LinkedListSequence<T>::printSeq() const {
    data_.printLinkedList();
}



template<typename T>
LinkedListSequence<T>::LinkedListSequenceIterator::LinkedListSequenceIterator(const typename LinkedList<T>::Node* node)
    : node_(node) {}

template<typename T>
typename LinkedListSequence<T>::LinkedListSequenceIterator::reference
LinkedListSequence<T>::LinkedListSequenceIterator::operator*() const {
    return node_->data_;
}

template<typename T>
typename LinkedListSequence<T>::LinkedListSequenceIterator::pointer
LinkedListSequence<T>::LinkedListSequenceIterator::operator->() const {
    return &(node_->data_);
}

template<typename T>
typename LinkedListSequence<T>::LinkedListSequenceIterator&
LinkedListSequence<T>::LinkedListSequenceIterator::operator++() {
    node_ = node_->next_;
    return *this;
}

template<typename T>
typename LinkedListSequence<T>::LinkedListSequenceIterator
LinkedListSequence<T>::LinkedListSequenceIterator::operator++(int) {
    LinkedListSequenceIterator tmp = *this;
    ++(*this);
    return tmp;
}

template<typename T>
typename LinkedListSequence<T>::LinkedListSequenceIterator&
LinkedListSequence<T>::LinkedListSequenceIterator::operator--() {
    node_ = node_->prev_;
    return *this;
}

template<typename T>
typename LinkedListSequence<T>::LinkedListSequenceIterator
LinkedListSequence<T>::LinkedListSequenceIterator::operator--(int) {
    LinkedListSequenceIterator tmp = *this;
    --(*this);
    return tmp;
}

template<typename T>
bool LinkedListSequence<T>::LinkedListSequenceIterator::operator==(const LinkedListSequenceIterator& other) const {
    return node_ == other.node_;
}

template<typename T>
bool LinkedListSequence<T>::LinkedListSequenceIterator::operator!=(const LinkedListSequenceIterator& other) const {
    return !(*this == other);
}

template<typename T>
typename LinkedListSequence<T>::const_iterator LinkedListSequence<T>::begin() const {
    return const_iterator(data_.GetHead());
}

template<typename T>
typename LinkedListSequence<T>::const_iterator LinkedListSequence<T>::end() const {
    return const_iterator(nullptr);
}

#endif // LINKEDLISTSEQUENCE_H
