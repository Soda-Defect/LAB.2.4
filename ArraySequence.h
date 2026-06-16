#ifndef ARRAYSEQUENCE_H
#define ARRAYSEQUENCE_H

#include "Sequence.h"
#include "DynamicArray.h"
#include "MapUtils.h"
#include <tuple>
#include <algorithm>
#include <functional>

template<typename T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T> data_;

public:
    ArraySequence();
    ArraySequence(int size);
    ArraySequence(int size, T defaultValue);
    ArraySequence(T *items, int count);
    ArraySequence(const ArraySequence<T> &other);
    ArraySequence(const DynamicArray<T> &arr);
    ~ArraySequence() = default;

    ArraySequence<T> &operator=(const ArraySequence<T> &other);

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


    bool operator==(const ArraySequence<T>& other) const;
    bool operator!=(const ArraySequence<T>& other) const;
    bool operator<(const ArraySequence<T>& other) const;
    bool operator<=(const ArraySequence<T>& other) const;
    bool operator>(const ArraySequence<T>& other) const;
    bool operator>=(const ArraySequence<T>& other) const;

    void printSeq() const;

    class ArraySequenceIterator;
    using iterator = ArraySequenceIterator;
    using const_iterator = ArraySequenceIterator;

    const_iterator begin() const;
    iterator begin();
    const_iterator end() const;
    iterator end();
};


template<typename T>
bool ArraySequence<T>::operator==(const ArraySequence<T>& other) const {
    if (GetLength() != other.GetLength()) return false;
    for (int i = 0; i < GetLength(); ++i)
        if ((*this)[i] != other[i]) return false;
    return true;
}

template<typename T>
bool ArraySequence<T>::operator!=(const ArraySequence<T>& other) const {
    return !(*this == other);
}

template<typename T>
bool ArraySequence<T>::operator<(const ArraySequence<T>& other) const {
    int n = GetLength();
    int m = other.GetLength();
    int minLen = std::min(n, m);
    for (int i = 0; i < minLen; ++i) {
        if ((*this)[i] < other[i]) return true;
        if (other[i] < (*this)[i]) return false;
    }
    return n < m;
}

template<typename T>
bool ArraySequence<T>::operator<=(const ArraySequence<T>& other) const {
    return (*this < other) || (*this == other);
}

template<typename T>
bool ArraySequence<T>::operator>(const ArraySequence<T>& other) const {
    return other < *this;
}

template<typename T>
bool ArraySequence<T>::operator>=(const ArraySequence<T>& other) const {
    return !(*this < other);
}

template<typename T>
ArraySequence<T>::ArraySequence() : data_(DynamicArray<T>()) {}

template<typename T>
ArraySequence<T>::ArraySequence(int size) : data_(DynamicArray<T>(size)) {}

template<typename T>
ArraySequence<T>::ArraySequence(int size, T defaultValue) : data_(size, defaultValue) {}

template<typename T>
ArraySequence<T>::ArraySequence(T *items, int count) : data_(DynamicArray<T>(items, count)) {}

template<typename T>
ArraySequence<T>::ArraySequence(const ArraySequence<T> &other) : data_(DynamicArray<T>(other.data_)) {}

template<typename T>
ArraySequence<T>::ArraySequence(const DynamicArray<T> &arr) : data_(DynamicArray<T>(arr)) {}

template<typename T>
ArraySequence<T> &ArraySequence<T>::operator=(const ArraySequence<T> &other) {
    if (this != &other) data_ = other.data_;
    return *this;
}

template<typename T>
int ArraySequence<T>::GetLength() const {
    return data_.GetSize();
}

template<typename T>
T ArraySequence<T>::Get(int index) const {
    return data_.Get(index);
}

template<typename T>
void ArraySequence<T>::Set(int index, const T &item) {
    data_.Set(index, item);
}

template<typename T>
void ArraySequence<T>::Append(const T &item) {
    data_.push_back(item);
}

template<typename T>
void ArraySequence<T>::Prepend(const T &item) {
    data_.insert(0, item);
}

template<typename T>
void ArraySequence<T>::InsertAt(const T &item, int index) {
    if (index < 0 || index > data_.GetSize())
        throw IndexOutOFBoundsException("Index out of range");
    data_.insert(index, item);
}

template<typename T>
void ArraySequence<T>::RemoveAt(int index) {
    if (index < 0 || index >= data_.GetSize())
        throw IndexOutOFBoundsException("Index out of range");
    data_.erase(index);
}

template<typename T>
T ArraySequence<T>::GetFirst() const {
    if (GetLength() == 0) throw SizeException("Empty array");
    return data_.Get(0);
}

template<typename T>
T ArraySequence<T>::GetLast() const {
    if (data_.GetSize() == 0) throw SizeException("Empty array");
    return data_.Get(data_.GetSize() - 1);
}

template<typename T>
Sequence<T> *ArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex)
        throw IndexOutOFBoundsException("Indexes out of range");
    auto *result = new ArraySequence<T>();
    for (int i = startIndex; i <= endIndex; i++)
        result->Append(data_[i]);
    return result;
}

template<typename T>
T &ArraySequence<T>::operator[](int index) {
    return data_[index];
}

template<typename T>
const T &ArraySequence<T>::operator[](int index) const {
    return const_cast<ArraySequence<T> *>(this)->operator[](index);
}

template<typename T>
Sequence<T> *ArraySequence<T>::Map(std::function<T(const T &)> function) const {
    auto *result = new ArraySequence<T>();
    for (int i = 0; i < GetLength(); i++)
        result->Append(function(data_.Get(i)));
    return result;
}

template<typename T>
T ArraySequence<T>::Reduce(std::function<T(const T &, const T &)> function, const T &initial) const {
    T result = initial;
    for (int i = 0; i < GetLength(); i++)
        result = function(result, data_.Get(i));
    return result;
}

template<typename T>
Sequence<T> *ArraySequence<T>::Where(std::function<bool(const T &)> predicate) const {
    auto *result = new ArraySequence<T>();
    for (int i = 0; i < GetLength(); i++)
        if (predicate(data_.Get(i))) result->Append(data_.Get(i));
    return result;
}

template<typename T>
Sequence<T> *ArraySequence<T>::Concat(Sequence<T> *other) const {
    auto *result = new ArraySequence<T>();
    for (int i = 0; i < GetLength(); i++) result->Append(data_.Get(i));
    for (int i = 0; i < other->GetLength(); i++) result->Append(other->Get(i));
    return result;
}

template<typename T>
void ArraySequence<T>::Slice(int start, int count, Sequence<T> *insertSequence) {
    int length = GetLength();
    if (start < 0) start = length + start;
    if (start < 0 || start >= length)
        throw IndexOutOFBoundsException("Start index out of range");
    if (count < 0 || start + count > length)
        throw IndexOutOFBoundsException("Invalid count");

    DynamicArray<T> newData;
    for (int i = 0; i < start; i++) newData.push_back(data_.Get(i));
    if (insertSequence != nullptr)
        for (int i = 0; i < insertSequence->GetLength(); i++)
            newData.push_back(insertSequence->Get(i));
    for (int i = start + count; i < length; i++)
        newData.push_back(data_.Get(i));
    data_ = std::move(newData);
}

template<typename T>
void merge(ArraySequence<T>& vec, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
    ArraySequence<T> leftVec(n1), rightVec(n2);
    for (i = 0; i < n1; i++) leftVec[i] = vec[left + i];
    for (j = 0; j < n2; j++) rightVec[j] = vec[mid + 1 + j];
    i = 0; j = 0; k = left;
    while (i < n1 && j < n2) {
        if (leftVec[i] <= rightVec[j]) vec[k++] = leftVec[i++];
        else vec[k++] = rightVec[j++];
    }
    while (i < n1) vec[k++] = leftVec[i++];
    while (j < n2) vec[k++] = rightVec[j++];
}

template<typename T>
void mergeSort(ArraySequence<T>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

template<typename T>
void ArraySequence<T>::Sort() {
    if (GetLength() <= 1) return;
    mergeSort(*this, 0, GetLength() - 1);
}

template<typename T>
int ArraySequence<T>::FindSubsequence(const Sequence<T>& pattern) const {
    int plen = pattern.GetLength();
    for (int i = 0; i <= GetLength() - plen; ++i) {
        bool ok = true;
        for (int j = 0; j < plen; ++j)
            if (pattern[j] != Get(i + j)) { ok = false; break; }
        if (ok) return i;
    }
    return -1;
}

template<typename T>
void ArraySequence<T>::Merge(const Sequence<T>& other) {
    ArraySequence<T> res;
    int i = 0, j = 0;
    int lenThis = GetLength();
    int lenOther = other.GetLength();
    while (i < lenThis && j < lenOther) {
        if (Get(i) <= other.Get(j)) res.Append(Get(i++));
        else res.Append(other.Get(j++));
    }
    while (i < lenThis) res.Append(Get(i++));
    while (j < lenOther) res.Append(other.Get(j++));
    *this = std::move(res);
}

template<typename T>
void ArraySequence<T>::printSeq() const {
    std::cout << "[";
    for (int i = 0; i < GetLength(); ++i) {
        std::cout << data_.Get(i);
        if (i < GetLength() - 1) std::cout << ", ";
    }
    std::cout << "]";
}


template<typename T>
class ArraySequence<T>::ArraySequenceIterator {
private:
    const ArraySequence<T>* seq_;
    int index_;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    ArraySequenceIterator(const ArraySequence<T>* seq = nullptr, int index = 0)
        : seq_(seq), index_(index) {}

    reference operator*() const { return (*seq_)[index_]; }
    pointer operator->() const { return &(operator*()); }
    ArraySequenceIterator& operator++() { ++index_; return *this; }
    ArraySequenceIterator operator++(int) { ArraySequenceIterator tmp = *this; ++(*this); return tmp; }
    ArraySequenceIterator& operator--() { --index_; return *this; }
    ArraySequenceIterator operator--(int) { ArraySequenceIterator tmp = *this; --(*this); return tmp; }
    bool operator==(const ArraySequenceIterator& other) const { return seq_ == other.seq_ && index_ == other.index_; }
    bool operator!=(const ArraySequenceIterator& other) const { return !(*this == other); }
};

template<typename T>
typename ArraySequence<T>::const_iterator ArraySequence<T>::begin() const {
    return const_iterator(this, 0);
}

template<typename T>
typename ArraySequence<T>::iterator ArraySequence<T>::begin() {
    return iterator(this, 0);
}

template<typename T>
typename ArraySequence<T>::const_iterator ArraySequence<T>::end() const {
    return const_iterator(this, GetLength());
}

template<typename T>
typename ArraySequence<T>::iterator ArraySequence<T>::end() {
    return iterator(this, GetLength());
}

#endif // ARRAYSEQUENCE_H
