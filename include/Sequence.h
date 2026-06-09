#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <functional>


template<typename T>
class Sequence {
public:
    virtual ~Sequence() = default;

    virtual int GetLength() const = 0;

    virtual T Get(int index) const = 0;

    virtual void Set(int index, const T &value) = 0;

    virtual void Append(const T &item) = 0;

    virtual void Prepend(const T &item) = 0;

    virtual void InsertAt(const T &item, int index) = 0;

    virtual void RemoveAt(int index) = 0;

    virtual T GetFirst() const = 0;

    virtual T GetLast() const = 0;

    virtual Sequence<T> *GetSubsequence(int startIndex, int endIndex) const = 0;

    virtual T &operator[](int index) = 0;

    virtual const T &operator[](int index) const = 0;

    virtual Sequence<T> *Map(std::function<T(const T &)> function) const = 0;

    virtual T Reduce(std::function<T(const T &, const T &)> function, const T &initial) const = 0;

    virtual Sequence<T> *Where(std::function<bool(const T &)> predicate) const = 0;

    virtual Sequence<T> *Concat(Sequence<T> *other) const = 0;

    virtual void Slice(int start, int count, Sequence<T> *insertSequence = nullptr) = 0;
};

#endif // SEQUENCE_H