#ifndef LAZYSEQUENCE_H
#define LAZYSEQUENCE_H

#include "ArraySequence.h"
#include "SequenceException.h"
#include "Cardinality.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

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
public:
    explicit ArraySeqGenerator(const ArraySequence<T>& data);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;

private:
    ArraySequence<T> data_;
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
    AppendGenerator(std::shared_ptr<IGenerator<T>> base,
                    const ArraySequence<T>& tail);
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
    PrependGenerator(const ArraySequence<T>& head,
                     std::shared_ptr<IGenerator<T>> tail);
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
    ConcatGenerator(std::shared_ptr<IGenerator<T>> first,
                    std::shared_ptr<IGenerator<T>> second);
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
    MapGenerator(std::shared_ptr<IGenerator<T>> source,
                 std::function<R(const T&)> func);
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
    FilterGenerator(std::shared_ptr<IGenerator<T>> source,
                    std::function<bool(const T&)> pred);
    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;

private:
    std::shared_ptr<IGenerator<T>> source_;
    std::function<bool(const T&)> pred_;

    mutable std::vector<T> matches_;
    mutable size_t scanned_ = 0;
    mutable bool exhausted_ = false;

    void scanUntil(size_t resultIndex) const;
    void scanAllFinite() const;
};

template<typename A, typename B>
class ZipGenerator : public IGenerator<std::pair<A, B>> {
public:
    ZipGenerator(std::shared_ptr<IGenerator<A>> first,
                 std::shared_ptr<IGenerator<B>> second);
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
    InsertGenerator(std::shared_ptr<IGenerator<T>> base,
                    size_t pos,
                    const T& elem);

    InsertGenerator(std::shared_ptr<IGenerator<T>> base,
                    size_t pos,
                    std::shared_ptr<IGenerator<T>> inserted);

    T generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;

private:
    std::shared_ptr<IGenerator<T>> base_;
    size_t pos_;
    std::shared_ptr<IGenerator<T>> inserted_;

    void validatePosition() const;
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

    LazySequence(std::function<T(size_t, const ArraySequence<T>&)> rule,
                 const ArraySequence<T>& seed);
    explicit LazySequence(std::shared_ptr<IGenerator<T>> gen);

    T GetFirst() const;
    T GetLast() const;

    T Get(size_t index) const;
    T Get(int index) const;

    bool IsEndless() const;
    std::shared_ptr<IGenerator<T>> GetGenerator() const;
    size_t ComputedCount() const;

    LazySequence<T> Append(const T& item) const;
    LazySequence<T> Append(const ArraySequence<T>& items) const;
    LazySequence<T> Prepend(const T& item) const;
    LazySequence<T> Prepend(const ArraySequence<T>& items) const;

    LazySequence<T> InsertAt(size_t index, const T& item) const;
    LazySequence<T> InsertAt(size_t index, const LazySequence<T>& seq) const;
    LazySequence<T> InsertAt(int index, const T& item) const;
    LazySequence<T> InsertAt(int index, const LazySequence<T>& seq) const;

    LazySequence<T> Concat(const LazySequence<T>& other) const;

    template<typename R>
    LazySequence<R> Map(std::function<R(const T&)> func) const;

    LazySequence<T> Where(std::function<bool(const T&)> pred) const;

    template<typename U>
    LazySequence<std::pair<T, U>> Zip(const LazySequence<U>& other) const;

    T Reduce(std::function<T(const T&, const T&)> binop,
             T init,
             size_t maxCount = 0) const;

    LazySequence<T> Take(size_t n) const;

    class Iterator;
    Iterator begin() const;
    Iterator end() const;

    Cardinality GetCardinality() const {
        if (generator_->isEndless())
            return Cardinality::Countable();
        return Cardinality(generator_->knownCount());
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
    using pointer = void;
    using reference = T;

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
    explicit FirstProjectionGenerator(
        std::shared_ptr<IGenerator<std::pair<A, B>>> source);
    A generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;

private:
    std::shared_ptr<IGenerator<std::pair<A, B>>> source_;
};

template<typename A, typename B>
class SecondProjectionGenerator : public IGenerator<B> {
public:
    explicit SecondProjectionGenerator(
        std::shared_ptr<IGenerator<std::pair<A, B>>> source);
    B generate(size_t index) override;
    bool isEndless() const override;
    size_t knownCount() const override;

private:
    std::shared_ptr<IGenerator<std::pair<A, B>>> source_;
};

template<typename A, typename B>
std::pair<LazySequence<A>, LazySequence<B>>
Unzip(const LazySequence<std::pair<A, B>>& seq);

template<typename T>
T EmptyGenerator<T>::generate(size_t) {
    throw IndexOutOFBoundsException("Empty sequence");
}

template<typename T>
bool EmptyGenerator<T>::isEndless() const {
    return false;
}

template<typename T>
size_t EmptyGenerator<T>::knownCount() const {
    return 0;
}

template<typename T>
ArraySeqGenerator<T>::ArraySeqGenerator(const ArraySequence<T>& data)
    : data_(data) {}

template<typename T>
T ArraySeqGenerator<T>::generate(size_t index) {
    if (index >= static_cast<size_t>(data_.GetLength()))
        throw IndexOutOFBoundsException(
            "ArraySeqGenerator: index out of range");
    return data_.Get(static_cast<int>(index));
}

template<typename T>
bool ArraySeqGenerator<T>::isEndless() const {
    return false;
}

template<typename T>
size_t ArraySeqGenerator<T>::knownCount() const {
    return static_cast<size_t>(data_.GetLength());
}

template<typename T>
Generator<T>::Generator(Rule rule, const ArraySequence<T>& seed)
    : rule_(std::move(rule)), computed_(seed) {
    if (!rule_)
        throw std::invalid_argument("Generator rule is empty");
}

template<typename T>
T Generator<T>::generate(size_t index) {
    if (index > static_cast<size_t>(std::numeric_limits<int>::max()))
        throw IndexOutOFBoundsException("Generator: index is too large");

    if (index < static_cast<size_t>(computed_.GetLength()))
        return computed_.Get(static_cast<int>(index));

    while (static_cast<size_t>(computed_.GetLength()) <= index) {
        const size_t current = static_cast<size_t>(computed_.GetLength());
        computed_.Append(rule_(current, computed_));
    }

    return computed_.Get(static_cast<int>(index));
}

template<typename T>
bool Generator<T>::isEndless() const {
    return true;
}

template<typename T>
size_t Generator<T>::knownCount() const {
    return static_cast<size_t>(computed_.GetLength());
}

template<typename T>
AppendGenerator<T>::AppendGenerator(std::shared_ptr<IGenerator<T>> base,
                                    const ArraySequence<T>& tail)
    : base_(std::move(base)), tail_(tail) {
    if (!base_)
        throw std::invalid_argument("AppendGenerator: null base generator");

    if (base_->isEndless())
        throw std::logic_error("Cannot append to endless sequence");
}

template<typename T>
T AppendGenerator<T>::generate(size_t index) {
    try {
        return base_->generate(index);
    } catch (const IndexOutOFBoundsException&) {
        const size_t baseSize = base_->knownCount();
        const size_t tailSize = static_cast<size_t>(tail_.GetLength());

        if (index >= baseSize && index < baseSize + tailSize)
            return tail_.Get(static_cast<int>(index - baseSize));

        throw IndexOutOFBoundsException(
            "AppendGenerator: index out of range");
    }
}

template<typename T>
bool AppendGenerator<T>::isEndless() const {
    return false;
}

template<typename T>
size_t AppendGenerator<T>::knownCount() const {
    return base_->knownCount() + static_cast<size_t>(tail_.GetLength());
}

template<typename T>
PrependGenerator<T>::PrependGenerator(const ArraySequence<T>& head,
                                      std::shared_ptr<IGenerator<T>> tail)
    : head_(head), tail_(std::move(tail)) {
    if (!tail_)
        throw std::invalid_argument("PrependGenerator: null tail generator");
}

template<typename T>
T PrependGenerator<T>::generate(size_t index) {
    const size_t headSize = static_cast<size_t>(head_.GetLength());

    if (index < headSize)
        return head_.Get(static_cast<int>(index));

    return tail_->generate(index - headSize);
}

template<typename T>
bool PrependGenerator<T>::isEndless() const {
    return tail_->isEndless();
}

template<typename T>
size_t PrependGenerator<T>::knownCount() const {
    const size_t headSize = static_cast<size_t>(head_.GetLength());
    return headSize + tail_->knownCount();
}

template<typename T>
ConcatGenerator<T>::ConcatGenerator(std::shared_ptr<IGenerator<T>> first,
                                    std::shared_ptr<IGenerator<T>> second)
    : first_(std::move(first)), second_(std::move(second)) {
    if (!first_ || !second_)
        throw std::invalid_argument("ConcatGenerator: null generator");
}

template<typename T>
T ConcatGenerator<T>::generate(size_t index) {
    if (first_->isEndless())
        return first_->generate(index);
    try {
        return first_->generate(index);
    } catch (const IndexOutOFBoundsException&) {
        const size_t firstSize = first_->knownCount();
        if (index < firstSize)
            throw;
        return second_->generate(index - firstSize);
    }
}

template<typename T>
bool ConcatGenerator<T>::isEndless() const {
    return first_->isEndless() || second_->isEndless();
}

template<typename T>
size_t ConcatGenerator<T>::knownCount() const {
    if (first_->isEndless())
        return first_->knownCount();

    const size_t firstSize = first_->knownCount();

    if (second_->isEndless())
        return firstSize + second_->knownCount();

    return firstSize + second_->knownCount();
}

template<typename T, typename R>
MapGenerator<T, R>::MapGenerator(std::shared_ptr<IGenerator<T>> source,
                                 std::function<R(const T&)> func)
    : source_(std::move(source)), func_(std::move(func)) {
    if (!source_)
        throw std::invalid_argument("MapGenerator: null source generator");
    if (!func_)
        throw std::invalid_argument("MapGenerator: empty function");
}

template<typename T, typename R>
R MapGenerator<T, R>::generate(size_t index) {
    return func_(source_->generate(index));
}

template<typename T, typename R>
bool MapGenerator<T, R>::isEndless() const {
    return source_->isEndless();
}

template<typename T, typename R>
size_t MapGenerator<T, R>::knownCount() const {
    return source_->knownCount();
}

template<typename T>
FilterGenerator<T>::FilterGenerator(std::shared_ptr<IGenerator<T>> source,
                                    std::function<bool(const T&)> pred)
    : source_(std::move(source)), pred_(std::move(pred)) {
    if (!source_)
        throw std::invalid_argument("FilterGenerator: null source generator");
    if (!pred_)
        throw std::invalid_argument("FilterGenerator: empty predicate");
}

template<typename T>
void FilterGenerator<T>::scanUntil(size_t resultIndex) const {
    if (resultIndex < matches_.size())
        return;

    while (matches_.size() <= resultIndex) {
        if (exhausted_)
            throw IndexOutOFBoundsException(
                "FilterGenerator: index out of range");

        try {
            T value = source_->generate(scanned_);
            ++scanned_;
            if (pred_(value))
                matches_.push_back(std::move(value));
        } catch (const IndexOutOFBoundsException&) {
            exhausted_ = true;
            throw IndexOutOFBoundsException(
                "FilterGenerator: index out of range");
        }
    }
}

template<typename T>
void FilterGenerator<T>::scanAllFinite() const {
    if (source_->isEndless() || exhausted_)
        return;

    const size_t sourceCount = source_->knownCount();

    while (scanned_ < sourceCount) {
        T value = source_->generate(scanned_);
        ++scanned_;
        if (pred_(value))
            matches_.push_back(std::move(value));
    }

    exhausted_ = true;
}

template<typename T>
T FilterGenerator<T>::generate(size_t index) {
    scanUntil(index);
    return matches_[index];
}

template<typename T>
bool FilterGenerator<T>::isEndless() const {
    return source_->isEndless();
}

template<typename T>
size_t FilterGenerator<T>::knownCount() const {
    if (source_->isEndless())
        return matches_.size();

    scanAllFinite();
    return matches_.size();
}

template<typename A, typename B>
ZipGenerator<A, B>::ZipGenerator(std::shared_ptr<IGenerator<A>> first,
                                 std::shared_ptr<IGenerator<B>> second)
    : first_(std::move(first)), second_(std::move(second)) {
    if (!first_ || !second_)
        throw std::invalid_argument("ZipGenerator: null generator");
}

template<typename A, typename B>
std::pair<A, B> ZipGenerator<A, B>::generate(size_t index) {
    return {first_->generate(index), second_->generate(index)};
}

template<typename A, typename B>
bool ZipGenerator<A, B>::isEndless() const {
    return first_->isEndless() && second_->isEndless();
}

template<typename A, typename B>
size_t ZipGenerator<A, B>::knownCount() const {
    if (!first_->isEndless() && !second_->isEndless())
        return std::min(first_->knownCount(), second_->knownCount());

    if (!first_->isEndless())
        return first_->knownCount();

    if (!second_->isEndless())
        return second_->knownCount();

    return std::min(first_->knownCount(), second_->knownCount());
}

template<typename T>
InsertGenerator<T>::InsertGenerator(std::shared_ptr<IGenerator<T>> base,
                                    size_t pos,
                                    const T& elem)
    : base_(std::move(base)), pos_(pos) {
    ArraySequence<T> single;
    single.Append(elem);
    inserted_ = std::make_shared<ArraySeqGenerator<T>>(single);
    validatePosition();
}

template<typename T>
InsertGenerator<T>::InsertGenerator(
    std::shared_ptr<IGenerator<T>> base,
    size_t pos,
    std::shared_ptr<IGenerator<T>> inserted)
    : base_(std::move(base)), pos_(pos), inserted_(std::move(inserted)) {
    validatePosition();
}

template<typename T>
void InsertGenerator<T>::validatePosition() const {
    if (!base_ || !inserted_)
        throw std::invalid_argument("InsertGenerator: null generator");

    if (!base_->isEndless()) {
        const size_t baseSize = base_->knownCount();
        if (pos_ > baseSize)
            throw IndexOutOFBoundsException(
                "InsertGenerator: insertion index out of range");
    }
}

template<typename T>
T InsertGenerator<T>::generate(size_t index) {
    if (index < pos_)
        return base_->generate(index);

    if (inserted_->isEndless())
        return inserted_->generate(index - pos_);

    try {
        return inserted_->generate(index - pos_);
    } catch (const IndexOutOFBoundsException&) {
        const size_t insertedLen = inserted_->knownCount();
        return base_->generate(index - insertedLen);
    }
}

template<typename T>
bool InsertGenerator<T>::isEndless() const {
    return base_->isEndless() || inserted_->isEndless();
}

template<typename T>
size_t InsertGenerator<T>::knownCount() const {
    if (isEndless()) {
        if (inserted_->isEndless())
            return pos_ + inserted_->knownCount();
        return base_->knownCount() + inserted_->knownCount();
    }

    return base_->knownCount() + inserted_->knownCount();
}

template<typename T>
TakeGenerator<T>::TakeGenerator(std::shared_ptr<IGenerator<T>> src, size_t n)
    : src_(std::move(src)), n_(n) {
    if (!src_)
        throw std::invalid_argument("TakeGenerator: null source generator");
}

template<typename T>
T TakeGenerator<T>::generate(size_t index) {
    if (index >= n_)
        throw IndexOutOFBoundsException("Take: index out of range");

    return src_->generate(index);
}

template<typename T>
bool TakeGenerator<T>::isEndless() const {
    return false;
}

template<typename T>
size_t TakeGenerator<T>::knownCount() const {
    if (src_->isEndless())
        return n_;

    return std::min(n_, src_->knownCount());
}

template<typename T>
LazySequence<T>::LazySequence()
    : generator_(std::make_shared<EmptyGenerator<T>>()) {}

template<typename T>
LazySequence<T>::LazySequence(const ArraySequence<T>& items)
    : generator_(std::make_shared<ArraySeqGenerator<T>>(items)) {}

template<typename T>
LazySequence<T>::LazySequence(
    std::function<T(size_t, const ArraySequence<T>&)> rule,
    const ArraySequence<T>& seed)
    : generator_(std::make_shared<Generator<T>>(std::move(rule), seed)) {}

template<typename T>
LazySequence<T>::LazySequence(std::shared_ptr<IGenerator<T>> gen)
    : generator_(std::move(gen)) {
    if (!generator_)
        throw std::invalid_argument("LazySequence: null generator");
}

template<typename T>
T LazySequence<T>::GetFirst() const {
    return Get(static_cast<size_t>(0));
}

template<typename T>
T LazySequence<T>::GetLast() const {
    if (generator_->isEndless())
        throw std::logic_error("GetLast on endless sequence");

    const size_t size = generator_->knownCount();
    if (size == 0)
        throw IndexOutOFBoundsException("Empty sequence");

    return Get(size - 1);
}

template<typename T>
T LazySequence<T>::Get(size_t index) const {
    ensureComputed(index);
    return memo_[index];
}

template<typename T>
T LazySequence<T>::Get(int index) const {
    if (index < 0)
        throw IndexOutOFBoundsException("LazySequence: negative index");
    return Get(static_cast<size_t>(index));
}

template<typename T>
bool LazySequence<T>::IsEndless() const {
    return generator_->isEndless();
}

template<typename T>
std::shared_ptr<IGenerator<T>> LazySequence<T>::GetGenerator() const {
    return generator_;
}

template<typename T>
size_t LazySequence<T>::ComputedCount() const {
    return memo_.size();
}

template<typename T>
void LazySequence<T>::ensureComputed(size_t index) const {
    if (index > static_cast<size_t>(std::numeric_limits<int>::max()))
        throw IndexOutOFBoundsException("LazySequence: index is too large");

    while (memo_.size() <= index) {
        const size_t nextIndex = memo_.size();
        memo_.push_back(generator_->generate(nextIndex));
    }
}

template<typename T>
LazySequence<T> LazySequence<T>::Append(const T& item) const {
    ArraySequence<T> tail;
    tail.Append(item);
    return LazySequence<T>(
        std::make_shared<AppendGenerator<T>>(generator_, tail));
}

template<typename T>
LazySequence<T> LazySequence<T>::Append(const ArraySequence<T>& items) const {
    return LazySequence<T>(
        std::make_shared<AppendGenerator<T>>(generator_, items));
}

template<typename T>
LazySequence<T> LazySequence<T>::Prepend(const T& item) const {
    ArraySequence<T> head;
    head.Append(item);
    return LazySequence<T>(
        std::make_shared<PrependGenerator<T>>(head, generator_));
}

template<typename T>
LazySequence<T> LazySequence<T>::Prepend(const ArraySequence<T>& items) const {
    return LazySequence<T>(
        std::make_shared<PrependGenerator<T>>(items, generator_));
}

template<typename T>
LazySequence<T> LazySequence<T>::InsertAt(size_t index, const T& item) const {
    return LazySequence<T>(
        std::make_shared<InsertGenerator<T>>(generator_, index, item));
}

template<typename T>
LazySequence<T> LazySequence<T>::InsertAt(size_t index,
                                          const LazySequence<T>& seq) const {
    return LazySequence<T>(
        std::make_shared<InsertGenerator<T>>(generator_, index,
                                             seq.generator_));
}

template<typename T>
LazySequence<T> LazySequence<T>::InsertAt(int index, const T& item) const {
    if (index < 0)
        throw IndexOutOFBoundsException("LazySequence: negative insertion index");
    return InsertAt(static_cast<size_t>(index), item);
}

template<typename T>
LazySequence<T> LazySequence<T>::InsertAt(int index,
                                          const LazySequence<T>& seq) const {
    if (index < 0)
        throw IndexOutOFBoundsException("LazySequence: negative insertion index");
    return InsertAt(static_cast<size_t>(index), seq);
}

template<typename T>
LazySequence<T> LazySequence<T>::Concat(const LazySequence<T>& other) const {
    return LazySequence<T>(
        std::make_shared<ConcatGenerator<T>>(generator_, other.generator_));
}

template<typename T>
template<typename R>
LazySequence<R> LazySequence<T>::Map(
    std::function<R(const T&)> func) const {
    return LazySequence<R>(
        std::make_shared<MapGenerator<T, R>>(generator_, std::move(func)));
}

template<typename T>
LazySequence<T> LazySequence<T>::Where(
    std::function<bool(const T&)> pred) const {
    return LazySequence<T>(
        std::make_shared<FilterGenerator<T>>(generator_, std::move(pred)));
}

template<typename T>
template<typename U>
LazySequence<std::pair<T, U>>
LazySequence<T>::Zip(const LazySequence<U>& other) const {
    return LazySequence<std::pair<T, U>>(
        std::make_shared<ZipGenerator<T, U>>(generator_, other.generator_));
}

template<typename T>
T LazySequence<T>::Reduce(std::function<T(const T&, const T&)> binop,
                          T init,
                          size_t maxCount) const {
    if (!binop)
        throw std::invalid_argument("Reduce: empty binary operation");

    if (generator_->isEndless() && maxCount == 0)
        throw std::logic_error(
            "Reduce on endless sequence without maxCount");

    T result = init;
    size_t index = 0;

    while (maxCount == 0 || index < maxCount) {
        try {
            result = binop(result, Get(index));
            ++index;
        } catch (const IndexOutOFBoundsException&) {
            break;
        }
    }

    return result;
}

template<typename T>
LazySequence<T> LazySequence<T>::Take(size_t n) const {
    return LazySequence<T>(
        std::make_shared<TakeGenerator<T>>(generator_, n));
}

template<typename T>
typename LazySequence<T>::Iterator LazySequence<T>::begin() const {
    return Iterator(this, 0);
}

template<typename T>
typename LazySequence<T>::Iterator LazySequence<T>::end() const {
    if (IsEndless())
        throw std::logic_error("Cannot iterate endless sequence");

    return Iterator(this, generator_->knownCount());
}

template<typename T>
LazySequence<T>::Iterator::Iterator()
    : seq_(nullptr), index_(0) {}

template<typename T>
LazySequence<T>::Iterator::Iterator(const LazySequence* seq, size_t idx)
    : seq_(seq), index_(idx) {}

template<typename T>
typename LazySequence<T>::Iterator::reference
LazySequence<T>::Iterator::operator*() const {
    if (!seq_)
        throw std::logic_error("Dereferencing an invalid LazySequence iterator");
    return seq_->Get(index_);
}

template<typename T>
typename LazySequence<T>::Iterator&
LazySequence<T>::Iterator::operator++() {
    ++index_;
    return *this;
}

template<typename T>
typename LazySequence<T>::Iterator
LazySequence<T>::Iterator::operator++(int) {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

template<typename T>
bool LazySequence<T>::Iterator::operator==(const Iterator& other) const {
    if (seq_ == nullptr || other.seq_ == nullptr)
        return seq_ == other.seq_ && index_ == other.index_;

    if (seq_ != other.seq_)
        return false;

    if (seq_->IsEndless())
        return index_ == other.index_;

    const size_t size = seq_->generator_->knownCount();
    const bool thisEnd = index_ >= size;
    const bool otherEnd = other.index_ >= size;

    if (thisEnd || otherEnd)
        return thisEnd && otherEnd;

    return index_ == other.index_;
}

template<typename T>
bool LazySequence<T>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

template<typename A, typename B>
FirstProjectionGenerator<A, B>::FirstProjectionGenerator(
    std::shared_ptr<IGenerator<std::pair<A, B>>> source)
    : source_(std::move(source)) {
    if (!source_)
        throw std::invalid_argument(
            "FirstProjectionGenerator: null source generator");
}

template<typename A, typename B>
A FirstProjectionGenerator<A, B>::generate(size_t index) {
    return source_->generate(index).first;
}

template<typename A, typename B>
bool FirstProjectionGenerator<A, B>::isEndless() const {
    return source_->isEndless();
}

template<typename A, typename B>
size_t FirstProjectionGenerator<A, B>::knownCount() const {
    return source_->knownCount();
}

template<typename A, typename B>
SecondProjectionGenerator<A, B>::SecondProjectionGenerator(
    std::shared_ptr<IGenerator<std::pair<A, B>>> source)
    : source_(std::move(source)) {
    if (!source_)
        throw std::invalid_argument(
            "SecondProjectionGenerator: null source generator");
}

template<typename A, typename B>
B SecondProjectionGenerator<A, B>::generate(size_t index) {
    return source_->generate(index).second;
}

template<typename A, typename B>
bool SecondProjectionGenerator<A, B>::isEndless() const {
    return source_->isEndless();
}

template<typename A, typename B>
size_t SecondProjectionGenerator<A, B>::knownCount() const {
    return source_->knownCount();
}

template<typename A, typename B>
std::pair<LazySequence<A>, LazySequence<B>>
Unzip(const LazySequence<std::pair<A, B>>& seq) {
    auto gen = seq.GetGenerator();
    auto firstGen =
        std::make_shared<FirstProjectionGenerator<A, B>>(gen);
    auto secondGen =
        std::make_shared<SecondProjectionGenerator<A, B>>(gen);

    return {LazySequence<A>(firstGen), LazySequence<B>(secondGen)};
}

#endif // LAZYSEQUENCE_H
