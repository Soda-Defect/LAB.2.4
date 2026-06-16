#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include "ArraySequence.h"
#include <functional>
#include <stdexcept>

template<typename T, typename Compare = std::less<T>>
class BinaryHeap {
public:
    BinaryHeap() = default;

    void Insert(const T& value);
    T ExtractMin();
    ArraySequence<T> ToArray() const;
    int GetSize() const;

private:
    ArraySequence<T> data_;
    Compare compare_;

    void siftUp(int index);
    void siftDown(int index);
    void swap(int i, int j);
};


template<typename T, typename Compare>
void BinaryHeap<T, Compare>::Insert(const T& value) {
    data_.Append(value);
    siftUp(data_.GetLength() - 1);
}

template<typename T, typename Compare>
T BinaryHeap<T, Compare>::ExtractMin() {
    if (data_.GetLength() == 0) throw std::out_of_range("Heap is empty");
    T result = data_.Get(0);
    int last = data_.GetLength() - 1;
    if (last > 0) {
        data_.Set(0, data_.Get(last));
        data_.RemoveAt(last);
        siftDown(0);
    } else {
        data_.RemoveAt(0);
    }
    return result;
}

template<typename T, typename Compare>
ArraySequence<T> BinaryHeap<T, Compare>::ToArray() const {
    return data_;
}

template<typename T, typename Compare>
int BinaryHeap<T, Compare>::GetSize() const {
    return data_.GetLength();
}

template<typename T, typename Compare>
void BinaryHeap<T, Compare>::siftUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!compare_(data_.Get(index), data_.Get(parent))) break;
        swap(index, parent);
        index = parent;
    }
}

template<typename T, typename Compare>
void BinaryHeap<T, Compare>::siftDown(int index) {
    int size = data_.GetLength();
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        if (left < size && compare_(data_.Get(left), data_.Get(smallest))) smallest = left;
        if (right < size && compare_(data_.Get(right), data_.Get(smallest))) smallest = right;
        if (smallest == index) break;
        swap(index, smallest);
        index = smallest;
    }
}

template<typename T, typename Compare>
void BinaryHeap<T, Compare>::swap(int i, int j) {
    T tmp = data_.Get(i);
    data_.Set(i, data_.Get(j));
    data_.Set(j, tmp);
}

#endif // BINARYHEAP_H
