#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H


#include <iostream>
#include <string>
#include "SequenceException.h"

template<class T>
class DynamicArray {
public:
    DynamicArray();
    DynamicArray(T *items, int count);
    explicit DynamicArray(int size);
    DynamicArray(int size, T defaultValue);
    DynamicArray(const DynamicArray<T> &other);
    DynamicArray(DynamicArray<T> &&other) noexcept;
    ~DynamicArray();

    DynamicArray<T> &operator=(const DynamicArray<T> &other);
    DynamicArray &operator=(DynamicArray<T> &&other) noexcept;

    T Get(int index) const;
    T Get(int index);
    void Set(int idx, const T& value);
    T &At(int pos);
    T &At(int pos) const;
    T &operator[](int index);
    T &operator[](int index) const;

    int GetSize() const;
    int GetCapacity() const;
    bool is_empty() const;
    T &Back() const;

    void Resize(int new_size);
    void Reserve(int new_cap);
    void ShrinkToFit();
    void push_back(T value);
    T pop_back();
    void erase(int idx);
    bool insert(int idx, T value);
    void clear();

    void printDynamicArray() const;

private:
    T *data_;
    int size_;
    int capacity_;

    void ensureCapacity(int needed);
};



template <typename T>
DynamicArray<T>::DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}

template <typename T>
DynamicArray<T>::DynamicArray(T *items, int count) : data_(nullptr), size_(0), capacity_(0) {
    if (count < 0) throw IndexOutOFBoundsException("Количество не может быть отрицательным");
    if (count == 0) {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        return;
    }
    data_ = new T[count];
    size_ = count;
    capacity_ = count;
    for (int i = 0; i < count; ++i) data_[i] = items[i];
}

template <typename T>
DynamicArray<T>::DynamicArray(int size) : data_(nullptr), size_(0), capacity_(0) {
    if (size < 0) throw SizeException("Размер не может быть меньше 0");
    if (size == 0) {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        return;
    }
    data_ = new T[size]();
    size_ = size;
    capacity_ = size;
}

template <typename T>
DynamicArray<T>::DynamicArray(int size, T defaultValue) : data_(nullptr), size_(0), capacity_(0) {
    if (size < 0) throw SizeException("Размер не может быть отрицательным");
    if (size == 0) {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        return;
    }
    data_ = new T[size];
    size_ = size;
    capacity_ = size;
    for (int i = 0; i < size_; ++i) data_[i] = defaultValue;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T> &other)
    : data_(nullptr), size_(other.size_), capacity_(other.size_) {
    if (size_ > 0) {
        data_ = new T[size_];
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T> &&other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] data_;
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(const DynamicArray<T> &other) {
    if (this != &other) {
        delete[] data_;
        size_ = other.size_;
        capacity_ = other.size_;
        if (size_ > 0) {
            data_ = new T[size_];
            for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
        } else {
            data_ = nullptr;
        }
    }
    return *this;
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(DynamicArray<T> &&other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

template <typename T>
T DynamicArray<T>::Get(int index) const {
    return const_cast<DynamicArray*>(this)->At(index);
}

template <typename T>
T DynamicArray<T>::Get(int index) {
    return At(index);
}

template <typename T>
void DynamicArray<T>::Set(int idx, const T& value) {
    if (idx < 0 || idx >= size_) throw IndexOutOFBoundsException("Невалидный индекс");
    data_[idx] = value;
}

template <typename T>
T &DynamicArray<T>::At(int pos) {
    if (pos < 0 || pos >= size_) throw IndexOutOFBoundsException("Невалидный индекс");
    return data_[pos];
}

template <typename T>
T &DynamicArray<T>::At(int pos) const {
    return const_cast<DynamicArray*>(this)->At(pos);
}

template <typename T>
T &DynamicArray<T>::operator[](int index) {
    if (size_ == 0) throw IndexOutOFBoundsException("Массив пуст");
    return data_[index];
}

template <typename T>
T &DynamicArray<T>::operator[](int index) const {
    return const_cast<DynamicArray*>(this)->operator[](index);
}

template <typename T>
int DynamicArray<T>::GetSize() const {
    return size_;
}

template <typename T>
int DynamicArray<T>::GetCapacity() const {
    return capacity_;
}

template <typename T>
bool DynamicArray<T>::is_empty() const {
    return size_ == 0;
}

template <typename T>
T &DynamicArray<T>::Back() const {
    if (size_ == 0) throw IndexOutOFBoundsException("Массив пуст");
    return data_[size_ - 1];
}

template <typename T>
void DynamicArray<T>::Resize(int new_size) {
    if (new_size < 0) throw SizeException("Размер не может быть отрицательным");
    if (new_size == size_) return;

    if (new_size > capacity_) {
        Reserve(new_size);
    }

    if (new_size > size_) {

        for (int i = size_; i < new_size; ++i) data_[i] = T();
    }
    size_ = new_size;
}

template <typename T>
void DynamicArray<T>::Reserve(int new_cap) {
    if (new_cap <= capacity_) return;
    T *new_data = new T[new_cap];
    for (int i = 0; i < size_; ++i) new_data[i] = data_[i];
    delete[] data_;
    data_ = new_data;
    capacity_ = new_cap;
}

template <typename T>
void DynamicArray<T>::ShrinkToFit() {
    if (size_ == capacity_) return;
    if (size_ == 0) {
        delete[] data_;
        data_ = nullptr;
        capacity_ = 0;
        return;
    }
    T *new_data = new T[size_];
    for (int i = 0; i < size_; ++i) new_data[i] = data_[i];
    delete[] data_;
    data_ = new_data;
    capacity_ = size_;
}

template <typename T>
void DynamicArray<T>::push_back(T value) {
    if (size_ >= capacity_) {
        int new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
        Reserve(new_cap);
    }
    data_[size_++] = value;
}

template <typename T>
T DynamicArray<T>::pop_back() {
    if (size_ == 0) throw IndexOutOFBoundsException("Пустой массив нечего удалять");
    T result = Back();
    --size_;
    return result;
}

template <typename T>
void DynamicArray<T>::erase(int idx) {
    if (idx < 0 || idx >= size_) return;
    for (int i = idx; i < size_ - 1; ++i) data_[i] = data_[i+1];
    --size_;
}

template <typename T>
bool DynamicArray<T>::insert(int idx, T value) {
    if (idx < 0 || idx > size_) return false;
    if (size_ >= capacity_) {
        int new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
        Reserve(new_cap);
    }
    for (int i = size_; i > idx; --i) data_[i] = data_[i-1];
    data_[idx] = value;
    ++size_;
    return true;
}

template <typename T>
void DynamicArray<T>::clear() {
    delete[] data_;
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template <typename T>
void DynamicArray<T>::printDynamicArray() const {
    for (int i = 0; i < size_; ++i) std::cout << data_[i] << " ";
}

#endif // DYNAMICARRAY_H
