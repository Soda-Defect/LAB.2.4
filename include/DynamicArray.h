#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include "Exceptions.h"

template<class T>
class DynamicArray{
private:
    T *data;
    int size;
public:
    DynamicArray();
    DynamicArray(T *items, int count);
    explicit DynamicArray(int count);
    DynamicArray(int count, T defaultValue);
    DynamicArray(const DynamicArray<T> &other);
    DynamicArray(DynamicArray<T> &&other) noexcept;
    ~DynamicArray();

    DynamicArray<T> &operator=(const DynamicArray<T> &other);
    DynamicArray &operator=(DynamicArray<T> &&other) noexcept;

    T Get(int index) const;
    T Get(int index);
    int GetSize() const;

    void Set(int idx, const T& value);
    void Resize(int new_size);

    T &At(int pos);
    T &At(int pos) const;
    T &operator[](int index);
    T &operator[](int index) const;

    bool is_empty() const;
    T &Back() const;

    void push_back(T value);
    T pop_back();
    void erase(int idx);
    bool insert(int idx, T value);
    void clear();


    void printDynamicArray() const;

};

template <typename T>
DynamicArray<T>::DynamicArray() : data(nullptr), size(0) {}

template <typename T>
DynamicArray<T>::DynamicArray(T *items, int count) : data(nullptr), size(0) {
    if(count < 0){
        throw IndexOutOFBoundsException("Count cannot be negative");
    }

    if(count == 0){
        data = nullptr;
        size = 0;
        return;
    }

    data = new T[count];
    size = count;
    for(int i = 0; i < count; i++){
        data[i] = items[i];
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(int count) : data(nullptr), size(0) {
    if(count < 0){
        throw SizeException("Cannot create DynamicArray with size < 0");
    }

    if(count == 0){
        data = nullptr;
        size = 0;
        return;
    }

    data = new T[count]();
    size = count;
}

template <typename T>
DynamicArray<T>::DynamicArray(int count, T defaultValue) : data(nullptr), size(0) {
    if(count < 0){
        throw SizeException("Cannot create DynamicArray with size < 0");
    }

    if(count == 0){
        data = nullptr;
        size = 0;
        return;
    }

    data = new T[count];
    size = count;
    for(int i = 0; i < size; i++){
        data[i] = defaultValue;
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T> &other) : data(nullptr), size(other.size) {
    if (size > 0) {
        data = new T[size];
        std::copy_n(other.data, size, data);
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T> &&other) noexcept : data(other.data), size(other.size) {
    other.data = nullptr;
    other.size = 0;
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete [] data;
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(const DynamicArray<T> &other) {
    if (this != &other) {
        delete[] data;
        size = other.size;
        if (size > 0) {
            data = new T[size];
            std::copy_n(other.data, size, data);
        } else {
            data = nullptr;
        }
    }
    return *this;
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(DynamicArray<T> &&other) noexcept {
    if (this != &other) {
        delete[] data;
        data = other.data;
        size = other.size;
        other.data = nullptr;
        other.size = 0;
    }
    return *this;
}

template <typename T>
T &DynamicArray<T>::At(int pos) {
    if (pos < 0 || pos >= size){
        throw IndexOutOFBoundsException("Index out of bounds");
    }
    return data[pos];
}

template <typename T>
T &DynamicArray<T>::At(int pos) const {
    return const_cast<DynamicArray*>(this)->At(pos);
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
int DynamicArray<T>::GetSize() const {
    return size;
}

template <typename T>
void DynamicArray<T>::Set(int idx, const T& value) {
    if (idx < 0 || idx >= size){
        throw IndexOutOFBoundsException("Index out of bounds");
    }
    data[idx] = value;
}

template <typename T>
void DynamicArray<T>::Resize(int new_size) {
    if (new_size < 0){
        throw SizeException("New size cannot be negative");
    }

    if (new_size == size){
        return;
    }

    T *new_data = (new_size > 0) ? new T[new_size]() : nullptr;
    int copy_count = (new_size < size) ? new_size : size;
    for (int i = 0; i < copy_count; i++){
        new_data[i] = data[i];
    }

    delete[] data;
    data = new_data;
    size = new_size;
}

template <typename T>
T &DynamicArray<T>::operator[](int index) {
    if (size == 0){
        throw IndexOutOFBoundsException("Array is empty");
    }
    if (index < 0 || index >= size) {
        throw IndexOutOFBoundsException("Index out of bounds");
    }
    return data[index];
}

template <typename T>
T &DynamicArray<T>::operator[](int index) const {
    return const_cast<DynamicArray*>(this)->operator[](index);
}

template <typename T>
bool DynamicArray<T>::is_empty() const {
    return size == 0;
}

template <typename T>
T &DynamicArray<T>::Back() const {
    if (size == 0){
        throw IndexOutOFBoundsException("Array is empty");
    }
    return data[size - 1];
}

template <typename T>
bool DynamicArray<T>::insert(int idx, T value) {
    if (idx < 0 || idx > size){
        return false;
    }

    T *new_data = new T[size + 1];
    for (int i = 0; i < idx; i++){
        new_data[i] = data[i];
    }
    new_data[idx] = value;
    for (int i = idx; i < size; i++){
        new_data[i + 1] = data[i];
    }

    delete[] data;
    data = new_data;
    ++size;
    return true;
}

template <typename T>
void DynamicArray<T>::push_back(T value) {
    insert(size, value);
}

template <typename T>
void DynamicArray<T>::clear() {
    delete[] data;
    data = nullptr;
    size = 0;
}

template <typename T>
void DynamicArray<T>::erase(int idx) {
    if (idx < 0 || idx >= size){
        return;
    }
    if (size == 1) {
        clear();
        return;
    }
    T *new_data = new T[size - 1];
    for (int i = 0; i < idx; i++){
        new_data[i] = data[i];
    }
    for (int i = idx + 1; i < size; i++){
        new_data[i - 1] = data[i];
    }
    delete[] data;
    data = new_data;
    --size;
}

template <typename T>
T DynamicArray<T>::pop_back() {
    if (size == 0){
        throw IndexOutOFBoundsException("Cannot pop from empty array");
    }
    T result = Back();
    erase(size - 1);
    return result;
}

template <typename T>
void DynamicArray<T>::printDynamicArray() const {
    for (int i = 0; i < size; i++){
        std::cout << data[i] << " ";
    }
}

#endif // DYNAMICARRAY_H