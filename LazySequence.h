#ifndef LAZYSEQUENCE_H
#define LAZYSEQUENCE_H

#include "ArraySequence.h"
#include <memory>
#include <functional>
#include <stdexcept>
#include <vector>
#include "SequenceException.h"
#include "Cardinality.h"


template<typename T>
class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual T generate(size_t index) = 0;
    virtual bool isEndless() const = 0;
    virtual size_t knownCount() const = 0;
};


template<typename T>
class EmptyGenerator : public IGenerator<T> {
public:
    T generate(size_t) override;
    bool isEndless() const override;
    size_t knownCount() const override;
};


template<typename T>
class ArraySeqGenerator : public IGenerator<T> {
    ArraySequence<T> data_;
public:
    explicit ArraySeqGenerator(const ArraySequence<T>& data);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
};


template<typename T>
class Generator : public IGenerator<T> {
public:
    using Rule = std::function<T(size_t, const ArraySequence<T>&)>;
    Generator(Rule rule, const ArraySequence<T>& seed);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    Rule rule_;
    ArraySequence<T> computed_;
};


template<typename T>
class AppendGenerator : public IGenerator<T> {
public:
    AppendGenerator(std::shared_ptr<IGenerator<T>> base, const ArraySequence<T>& tail);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<T>> base_;
    ArraySequence<T> tail_;
};


template<typename T>
class PrependGenerator : public IGenerator<T> {
public:
    PrependGenerator(const ArraySequence<T>& head, std::shared_ptr<IGenerator<T>> tail);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    ArraySequence<T> head_;
    std::shared_ptr<IGenerator<T>> tail_;
};


template<typename T>
class ConcatGenerator : public IGenerator<T> {
public:
    ConcatGenerator(std::shared_ptr<IGenerator<T>> first, std::shared_ptr<IGenerator<T>> second);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<T>> first_;
    std::shared_ptr<IGenerator<T>> second_;
};


template<typename T, typename R>
class MapGenerator : public IGenerator<R> {
public:
    MapGenerator(std::shared_ptr<IGenerator<T>> source, std::function<R(const T&)> func);
    R generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<T>> source_;
    std::function<R(const T&)> func_;
};


template<typename T>
class FilterGenerator : public IGenerator<T> {
public:
    FilterGenerator(std::shared_ptr<IGenerator<T>> source, std::function<bool(const T&)> pred);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<T>> source_;
    std::function<bool(const T&)> pred_;
    std::vector<size_t> cache_;
};


template<typename A, typename B>
class ZipGenerator : public IGenerator<std::pair<A, B>> {
public:
    ZipGenerator(std::shared_ptr<IGenerator<A>> first, std::shared_ptr<IGenerator<B>> second);
    std::pair<A, B> generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<A>> first_;
    std::shared_ptr<IGenerator<B>> second_;
};


template<typename T>
class InsertGenerator : public IGenerator<T> {
public:

    InsertGenerator(std::shared_ptr<IGenerator<T>> base, size_t pos, const T& elem)
        : base_(std::move(base)), pos_(pos) {
        ArraySequence<T> single;
        single.Append(elem);
        inserted_ = std::make_shared<ArraySeqGenerator<T>>(single);
    }


    InsertGenerator(std::shared_ptr<IGenerator<T>> base, size_t pos, std::shared_ptr<IGenerator<T>> inserted)
        : base_(std::move(base)), pos_(pos), inserted_(std::move(inserted)) {}

    T generate(size_t index) override {
        if (index < pos_) {
            return base_->generate(index);
        }

        try {
            return inserted_->generate(index - pos_);
        } catch (const IndexOutOFBoundsException&) {

            size_t insertedLen = inserted_->knownCount();
            return base_->generate(index - insertedLen);
        }
    }

    bool isEndless() const override {
        return base_->isEndless() || inserted_->isEndless();
    }

    size_t knownCount() const override {
        if (isEndless()) return 0;
        return base_->knownCount() + inserted_->knownCount();
    }

private:
    std::shared_ptr<IGenerator<T>> base_;
    size_t pos_;
    std::shared_ptr<IGenerator<T>> inserted_;
};


template<typename T>
class TakeGenerator : public IGenerator<T> {
public:
    TakeGenerator(std::shared_ptr<IGenerator<T>> src, size_t n);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<T>> src_;
    size_t n_;
};


template<typename T>
class LazySequence {
public:
    LazySequence();
    explicit LazySequence(const ArraySequence<T>& items);
    LazySequence(const LazySequence&) = default;
    LazySequence(LazySequence&&) = default;
    LazySequence& operator=(const LazySequence&) = default;
    LazySequence& operator=(LazySequence&&) = default;

    LazySequence(std::function<T(size_t, const ArraySequence<T>&)> rule, const ArraySequence<T>& seed);
    explicit LazySequence(std::shared_ptr<IGenerator<T>> gen);

    T GetFirst() const;
    T GetLast() const;
    T Get(size_t index) const;
    bool IsEndless() const;
    std::shared_ptr<IGenerator<T>> GetGenerator() const;
    size_t ComputedCount() const;

    LazySequence<T> Append(const T& item) const;
    LazySequence<T> Append(const ArraySequence<T>& items) const;
    LazySequence<T> Prepend(const T& item) const;
    LazySequence<T> Prepend(const ArraySequence<T>& items) const;
    LazySequence<T> InsertAt(size_t index, const T& item) const;
    LazySequence<T> InsertAt(size_t index, const LazySequence<T>& seq) const;
    LazySequence<T> Concat(const LazySequence<T>& other) const;
    template<typename R>
    LazySequence<R> Map(std::function<R(const T&)> func) const;
    LazySequence<T> Where(std::function<bool(const T&)> pred) const;
    template<typename U>
    LazySequence<std::pair<T, U>> Zip(const LazySequence<U>& other) const;
    T Reduce(std::function<T(const T&, const T&)> binop, T init, size_t maxCount = 0) const;
    LazySequence<T> Take(size_t n) const;

    class Iterator;
    Iterator begin() const;
    Iterator end() const;

    Cardinality GetCardinality() const {
        if (generator_->isEndless()) {
            return Cardinality::Countable();
        } else {
            return Cardinality(generator_->knownCount());
        }
    }

private:
    std::shared_ptr<IGenerator<T>> generator_;
    mutable std::vector<T> memo_;
    void ensureComputed(size_t index) const;
};

template<typename T>
class LazySequence<T>::Iterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
    Iterator();
    reference operator*() const;
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
private:
    friend class LazySequence;
    Iterator(const LazySequence* seq, size_t idx);
    const LazySequence* seq_;
    size_t index_;
};


template<typename A, typename B>
class FirstProjectionGenerator : public IGenerator<A> {
public:
    explicit FirstProjectionGenerator(std::shared_ptr<IGenerator<std::pair<A, B>>> source);
    A generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<std::pair<A, B>>> source_;
};


template<typename A, typename B>
class SecondProjectionGenerator : public IGenerator<B> {
public:
    explicit SecondProjectionGenerator(std::shared_ptr<IGenerator<std::pair<A, B>>> source);
    B generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;
private:
    std::shared_ptr<IGenerator<std::pair<A, B>>> source_;
};


template<typename A, typename B>
std::pair<LazySequence<A>, LazySequence<B>> Unzip(const LazySequence<std::pair<A, B>>& seq);


template<typename T>
T EmptyGenerator<T>::generate(size_t) { throw IndexOutOFBoundsException("Empty sequence"); }
template<typename T>
bool EmptyGenerator<T>::isEndless() const { return false; }
template<typename T>
size_t EmptyGenerator<T>::knownCount() const { return 0; }


template<typename T>
ArraySeqGenerator<T>::ArraySeqGenerator(const ArraySequence<T>& data) : data_(data) {}
template<typename T>
T ArraySeqGenerator<T>::generate(size_t index) {
    if (index >= static_cast<size_t>(data_.GetLength()))
        throw IndexOutOFBoundsException("ArraySeqGenerator: index out of range");
    return data_.Get(static_cast<int>(index));
}
template<typename T>
bool ArraySeqGenerator<T>::isEndless() const { return false; }
template<typename T>
size_t ArraySeqGenerator<T>::knownCount() const { return static_cast<size_t>(data_.GetLength()); }


template<typename T>
Generator<T>::Generator(Rule rule, const ArraySequence<T>& seed) : rule_(rule), computed_(seed) {}
template<typename T>
T Generator<T>::generate(size_t index) {
    if (index < static_cast<size_t>(computed_.GetLength()))
        return computed_.Get(static_cast<int>(index));
    for (size_t i = computed_.GetLength(); i <= index; ++i) {
        T next = rule_(i, computed_);
        computed_.Append(next);
    }
    return computed_.Get(static_cast<int>(index));
}
template<typename T>
bool Generator<T>::isEndless() const { return true; }
template<typename T>
size_t Generator<T>::knownCount() const { return computed_.GetLength(); }


template<typename T>
AppendGenerator<T>::AppendGenerator(std::shared_ptr<IGenerator<T>> base, const ArraySequence<T>& tail)
    : base_(std::move(base)), tail_(tail) {
    if (base_->isEndless())
        throw std::logic_error("Cannot append to endless sequence");
}
template<typename T>
T AppendGenerator<T>::generate(size_t index) {
    size_t baseSize = base_->knownCount();
    if (index < baseSize) return base_->generate(index);
    if (index < baseSize + static_cast<size_t>(tail_.GetLength()))
        return tail_.Get(static_cast<int>(index - baseSize));
    throw IndexOutOFBoundsException("AppendGenerator: index out of range");
}
template<typename T>
bool AppendGenerator<T>::isEndless() const { return false; }
template<typename T>
size_t AppendGenerator<T>::knownCount() const { return base_->knownCount() + tail_.GetLength(); }


template<typename T>
PrependGenerator<T>::PrependGenerator(const ArraySequence<T>& head, std::shared_ptr<IGenerator<T>> tail)
    : head_(head), tail_(std::move(tail)) {}
template<typename T>
T PrependGenerator<T>::generate(size_t index) {
    if (index < static_cast<size_t>(head_.GetLength()))
        return head_.Get(static_cast<int>(index));
    return tail_->generate(index - head_.GetLength());
}
template<typename T>
bool PrependGenerator<T>::isEndless() const { return tail_->isEndless(); }
template<typename T>
size_t PrependGenerator<T>::knownCount() const {
    if (tail_->isEndless()) return head_.GetLength();
    return head_.GetLength() + tail_->knownCount();
}


template<typename T>
ConcatGenerator<T>::ConcatGenerator(std::shared_ptr<IGenerator<T>> first, std::shared_ptr<IGenerator<T>> second)
    : first_(std::move(first)), second_(std::move(second)) {}
template<typename T>
T ConcatGenerator<T>::generate(size_t index) {
    size_t firstSize = first_->knownCount();
    if (first_->isEndless()) return first_->generate(index);
    if (index < firstSize) return first_->generate(index);
    return second_->generate(index - firstSize);
}
template<typename T>
bool ConcatGenerator<T>::isEndless() const { return first_->isEndless() || second_->isEndless(); }
template<typename T>
size_t ConcatGenerator<T>::knownCount() const {
    if (first_->isEndless()) return first_->knownCount();
    if (second_->isEndless()) return first_->knownCount();
    return first_->knownCount() + second_->knownCount();
}


template<typename T, typename R>
MapGenerator<T,R>::MapGenerator(std::shared_ptr<IGenerator<T>> source, std::function<R(const T&)> func)
    : source_(std::move(source)), func_(std::move(func)) {}
template<typename T, typename R>
R MapGenerator<T,R>::generate(size_t index) { return func_(source_->generate(index)); }
template<typename T, typename R>
bool MapGenerator<T,R>::isEndless() const { return source_->isEndless(); }
template<typename T, typename R>
size_t MapGenerator<T,R>::knownCount() const { return source_->knownCount(); }


template<typename T>
FilterGenerator<T>::FilterGenerator(std::shared_ptr<IGenerator<T>> source, std::function<bool(const T&)> pred)
    : source_(std::move(source)), pred_(std::move(pred)) {}
template<typename T>
T FilterGenerator<T>::generate(size_t index) {
    if (index < cache_.size()) return source_->generate(cache_[index]);
    size_t sourceIdx = cache_.empty() ? 0 : cache_.back() + 1;
    size_t foundCount = cache_.size();
    while (foundCount <= index) {
        T elem = source_->generate(sourceIdx);
        if (pred_(elem)) {
            cache_.push_back(sourceIdx);
            ++foundCount;
        }
        ++sourceIdx;
    }
    return source_->generate(cache_[index]);
}
template<typename T>
bool FilterGenerator<T>::isEndless() const { return source_->isEndless(); }
template<typename T>
size_t FilterGenerator<T>::knownCount() const { return cache_.size(); }


template<typename A, typename B>
ZipGenerator<A,B>::ZipGenerator(std::shared_ptr<IGenerator<A>> first, std::shared_ptr<IGenerator<B>> second)
    : first_(std::move(first)), second_(std::move(second)) {}
template<typename A, typename B>
std::pair<A,B> ZipGenerator<A,B>::generate(size_t index) {
    return {first_->generate(index), second_->generate(index)};
}
template<typename A, typename B>
bool ZipGenerator<A,B>::isEndless() const { return first_->isEndless() && second_->isEndless(); }
template<typename A, typename B>
size_t ZipGenerator<A,B>::knownCount() const {
    if (first_->isEndless() && second_->isEndless()) return 0;
    if (first_->isEndless()) return second_->knownCount();
    if (second_->isEndless()) return first_->knownCount();
    return std::min(first_->knownCount(), second_->knownCount());
}




template<typename T>
TakeGenerator<T>::TakeGenerator(std::shared_ptr<IGenerator<T>> src, size_t n) : src_(std::move(src)), n_(n) {}
template<typename T>
T TakeGenerator<T>::generate(size_t index) {
    if (index >= n_) throw IndexOutOFBoundsException("Take: index out of range");
    return src_->generate(index);
}
template<typename T>
bool TakeGenerator<T>::isEndless() const { return false; }
template<typename T>
size_t TakeGenerator<T>::knownCount() const { return n_; }


template<typename T>
LazySequence<T>::LazySequence() : generator_(std::make_shared<EmptyGenerator<T>>()) {}
template<typename T>
LazySequence<T>::LazySequence(const ArraySequence<T>& items)
    : generator_(std::make_shared<ArraySeqGenerator<T>>(items)) {}
template<typename T>
LazySequence<T>::LazySequence(std::function<T(size_t, const ArraySequence<T>&)> rule, const ArraySequence<T>& seed)
    : generator_(std::make_shared<Generator<T>>(std::move(rule), seed)) {}
template<typename T>
LazySequence<T>::LazySequence(std::shared_ptr<IGenerator<T>> gen) : generator_(std::move(gen)) {}
template<typename T>
T LazySequence<T>::GetFirst() const { return Get(0); }
template<typename T>
T LazySequence<T>::GetLast() const {
    if (generator_->isEndless()) throw std::logic_error("GetLast on endless sequence");
    size_t sz = generator_->knownCount();
    if (sz == 0) throw IndexOutOFBoundsException("Empty sequence");
    return Get(sz - 1);
}
template<typename T>
T LazySequence<T>::Get(size_t index) const { ensureComputed(index); return memo_[index]; }
template<typename T>
bool LazySequence<T>::IsEndless() const { return generator_->isEndless(); }
template<typename T>
std::shared_ptr<IGenerator<T>> LazySequence<T>::GetGenerator() const { return generator_; }
template<typename T>
size_t LazySequence<T>::ComputedCount() const { return memo_.size(); }
template<typename T>
void LazySequence<T>::ensureComputed(size_t index) const {
    if (index < memo_.size()) return;
    for (size_t i = memo_.size(); i <= index; ++i)
        memo_.push_back(generator_->generate(i));
}
template<typename T>
LazySequence<T> LazySequence<T>::Append(const T& item) const {
    ArraySequence<T> tail;
    tail.Append(item);
    auto newGen = std::make_shared<AppendGenerator<T>>(generator_, tail);
    return LazySequence<T>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::Append(const ArraySequence<T>& items) const {
    auto newGen = std::make_shared<AppendGenerator<T>>(generator_, items);
    return LazySequence<T>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::Prepend(const T& item) const {
    ArraySequence<T> head;
    head.Append(item);
    auto newGen = std::make_shared<PrependGenerator<T>>(head, generator_);
    return LazySequence<T>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::Prepend(const ArraySequence<T>& items) const {
    auto newGen = std::make_shared<PrependGenerator<T>>(items, generator_);
    return LazySequence<T>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::InsertAt(size_t index, const T& item) const {
    auto newGen = std::make_shared<InsertGenerator<T>>(generator_, index, item);
    return LazySequence<T>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::InsertAt(size_t index, const LazySequence<T>& seq) const {
    auto newGen = std::make_shared<InsertGenerator<T>>(generator_, index, seq.generator_);
    return LazySequence<T>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::Concat(const LazySequence<T>& other) const {
    auto newGen = std::make_shared<ConcatGenerator<T>>(generator_, other.generator_);
    return LazySequence<T>(newGen);
}
template<typename T>
template<typename R>
LazySequence<R> LazySequence<T>::Map(std::function<R(const T&)> func) const {
    auto newGen = std::make_shared<MapGenerator<T,R>>(generator_, std::move(func));
    return LazySequence<R>(newGen);
}
template<typename T>
LazySequence<T> LazySequence<T>::Where(std::function<bool(const T&)> pred) const {
    auto newGen = std::make_shared<FilterGenerator<T>>(generator_, std::move(pred));
    return LazySequence<T>(newGen);
}
template<typename T>
template<typename U>
LazySequence<std::pair<T,U>> LazySequence<T>::Zip(const LazySequence<U>& other) const {
    auto newGen = std::make_shared<ZipGenerator<T,U>>(generator_, other.generator_);
    return LazySequence<std::pair<T,U>>(newGen);
}
template<typename T>
T LazySequence<T>::Reduce(std::function<T(const T&, const T&)> binop, T init, size_t maxCount) const {
    T result = init;
    if (generator_->isEndless() && maxCount == 0)
        throw std::logic_error("Reduce on endless sequence without maxCount");
    size_t i = 0;
    while (true) {
        if (maxCount > 0 && i >= maxCount) break;
        try {
            T val = Get(i);
            result = binop(result, val);
            ++i;
        } catch (const IndexOutOFBoundsException&) { break; }
    }
    return result;
}
template<typename T>
LazySequence<T> LazySequence<T>::Take(size_t n) const {
    auto newGen = std::make_shared<TakeGenerator<T>>(generator_, n);
    return LazySequence<T>(newGen);
}
template<typename T>
typename LazySequence<T>::Iterator LazySequence<T>::begin() const { return Iterator(this, 0); }
template<typename T>
typename LazySequence<T>::Iterator LazySequence<T>::end() const {
    if (IsEndless()) throw std::logic_error("Cannot iterate endless sequence");
    return Iterator(this, generator_->knownCount());
}
template<typename T>
LazySequence<T>::Iterator::Iterator() : seq_(nullptr), index_(0) {}
template<typename T>
LazySequence<T>::Iterator::Iterator(const LazySequence* seq, size_t idx) : seq_(seq), index_(idx) {}
template<typename T>
typename LazySequence<T>::Iterator::reference LazySequence<T>::Iterator::operator*() const { return seq_->Get(index_); }
template<typename T>
typename LazySequence<T>::Iterator& LazySequence<T>::Iterator::operator++() { ++index_; return *this; }
template<typename T>
typename LazySequence<T>::Iterator LazySequence<T>::Iterator::operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
template<typename T>
bool LazySequence<T>::Iterator::operator==(const Iterator& other) const {
    if (seq_ == nullptr && other.seq_ == nullptr) return true;
    if (seq_ == nullptr || other.seq_ == nullptr) return false;
    if (seq_ != other.seq_) return false;
    if (seq_->IsEndless()) return index_ == other.index_;
    size_t size = seq_->generator_->knownCount();
    bool thisEnd = (index_ >= size);
    bool otherEnd = (other.index_ >= size);
    if (thisEnd && otherEnd) return true;
    if (thisEnd || otherEnd) return false;
    return index_ == other.index_;
}
template<typename T>
bool LazySequence<T>::Iterator::operator!=(const Iterator& other) const { return !(*this == other); }


template<typename A, typename B>
FirstProjectionGenerator<A,B>::FirstProjectionGenerator(std::shared_ptr<IGenerator<std::pair<A,B>>> source) : source_(source) {}
template<typename A, typename B>
A FirstProjectionGenerator<A,B>::generate(size_t index) { return source_->generate(index).first; }
template<typename A, typename B>
bool FirstProjectionGenerator<A,B>::isEndless() const { return source_->isEndless(); }
template<typename A, typename B>
size_t FirstProjectionGenerator<A,B>::knownCount() const { return source_->knownCount(); }


template<typename A, typename B>
SecondProjectionGenerator<A,B>::SecondProjectionGenerator(std::shared_ptr<IGenerator<std::pair<A,B>>> source) : source_(source) {}
template<typename A, typename B>
B SecondProjectionGenerator<A,B>::generate(size_t index) { return source_->generate(index).second; }
template<typename A, typename B>
bool SecondProjectionGenerator<A,B>::isEndless() const { return source_->isEndless(); }
template<typename A, typename B>
size_t SecondProjectionGenerator<A,B>::knownCount() const { return source_->knownCount(); }


template<typename A, typename B>
std::pair<LazySequence<A>, LazySequence<B>> Unzip(const LazySequence<std::pair<A,B>>& seq) {
    auto gen = seq.GetGenerator();
    auto firstGen = std::make_shared<FirstProjectionGenerator<A,B>>(gen);
    auto secondGen = std::make_shared<SecondProjectionGenerator<A,B>>(gen);
    return {LazySequence<A>(firstGen), LazySequence<B>(secondGen)};
}

#endif // LAZYSEQUENCE_H
