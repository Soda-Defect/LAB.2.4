#ifndef TIMEVALUE_H
#define TIMEVALUE_H

#include <optional>
#include "LazySequence.h"

template<typename T>
struct TimedValue {
    double timestamp;
    T value;

    bool operator==(const TimedValue& other) const;
    bool operator!=(const TimedValue& other) const;
    bool operator<(const TimedValue& other) const;
    bool operator<=(const TimedValue& other) const;
    bool operator>(const TimedValue& other) const;
    bool operator>=(const TimedValue& other) const;
};

template<typename T>
bool TimedValue<T>::operator==(const TimedValue& other) const {
    return timestamp == other.timestamp && value == other.value;
}
template<typename T>
bool TimedValue<T>::operator!=(const TimedValue& other) const { return !(*this == other); }
template<typename T>
bool TimedValue<T>::operator<(const TimedValue& other) const { return timestamp < other.timestamp; }
template<typename T>
bool TimedValue<T>::operator<=(const TimedValue& other) const { return timestamp <= other.timestamp; }
template<typename T>
bool TimedValue<T>::operator>(const TimedValue& other) const { return timestamp > other.timestamp; }
template<typename T>
bool TimedValue<T>::operator>=(const TimedValue& other) const { return timestamp >= other.timestamp; }

template<typename T>
using TimeLazySequence = LazySequence<TimedValue<T>>;

#endif // TIMEVALUE_H
