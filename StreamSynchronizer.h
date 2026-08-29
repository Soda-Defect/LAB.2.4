#ifndef STREAMSYNCHRONIZER_H
#define STREAMSYNCHRONIZER_H

#include "LazySequence.h"
#include "TimeValue.h"

#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<typename T>
class StreamSynchronizationGenerator
    : public IGenerator<ArraySequence<std::optional<double>>> {
public:
    using Event = ArraySequence<std::optional<double>>;

    StreamSynchronizationGenerator(
        std::vector<TimeLazySequence<T>> streams,
        double tau)
        : streams_(std::move(streams)),
          tau_(tau),
          positions_(streams_.size(), 0),
          ended_(streams_.size(), false) {
        static_assert(std::is_convertible_v<T, double>,
                      "StreamSynchronizer requires values convertible to double");

        if (!std::isfinite(tau_) || tau_ < 0.0)
            throw std::invalid_argument(
                "StreamSynchronizer: tau must be a finite non-negative value");
    }

    Event generate(size_t index) override {
        ensureGenerated(index);
        return cache_[index];
    }

    bool isEndless() const override {
        for (const auto& stream : streams_) {
            if (stream.IsEndless())
                return true;
        }
        return false;
    }

    size_t knownCount() const override {
        if (isEndless())
            return cache_.size();

        while (!finished_) {
            try {
                cache_.push_back(generateNext());
            } catch (const IndexOutOFBoundsException&) {
                finished_ = true;
            }
        }

        return cache_.size();
    }

private:
    std::vector<TimeLazySequence<T>> streams_;
    double tau_;

    mutable std::vector<size_t> positions_;
    mutable std::vector<bool> ended_;
    mutable std::vector<Event> cache_;
    mutable bool finished_ = false;

    std::optional<TimedValue<T>> peek(size_t streamIndex) const {
        if (ended_[streamIndex])
            return std::nullopt;

        try {
            TimedValue<T> value =
                streams_[streamIndex].Get(positions_[streamIndex]);

            if (!std::isfinite(value.timestamp))
                throw std::invalid_argument(
                    "StreamSynchronizer: timestamp must be finite");

            return value;
        } catch (const IndexOutOFBoundsException&) {
            ended_[streamIndex] = true;
            return std::nullopt;
        }
    }

    Event generateNext() const {
        if (finished_)
            throw IndexOutOFBoundsException(
                "StreamSynchronizer: end of synchronized stream");

        std::vector<std::optional<TimedValue<T>>> current(streams_.size());

        double minTime = std::numeric_limits<double>::infinity();
        bool hasAny = false;

        for (size_t i = 0; i < streams_.size(); ++i) {
            current[i] = peek(i);
            if (current[i].has_value()) {
                hasAny = true;
                if (current[i]->timestamp < minTime)
                    minTime = current[i]->timestamp;
            }
        }

        if (!hasAny) {
            finished_ = true;
            throw IndexOutOFBoundsException(
                "StreamSynchronizer: end of synchronized stream");
        }

        Event event;
        event.Append(std::nullopt); 

        double timestampSum = 0.0;
        size_t timestampCount = 0;
        const double rightBorder = minTime + tau_;

        if (!std::isfinite(rightBorder))
            throw std::overflow_error(
                "StreamSynchronizer: minTime + tau overflowed");

        for (size_t i = 0; i < streams_.size(); ++i) {
            if (current[i].has_value() &&
                current[i]->timestamp <= rightBorder) {
                event.Append(std::optional<double>{
                    static_cast<double>(current[i]->value)});

                timestampSum += current[i]->timestamp;
                ++timestampCount;
                ++positions_[i];
            } else {
                event.Append(std::nullopt);
            }
        }

        if (timestampCount == 0)
            throw std::logic_error(
                "StreamSynchronizer: synchronization made no progress");

        const double tEst =
            timestampSum / static_cast<double>(timestampCount) - tau_ / 2.0;

        event.Set(0, std::optional<double>{tEst});
        return event;
    }

    void ensureGenerated(size_t index) {
        while (cache_.size() <= index) {
            if (finished_)
                throw IndexOutOFBoundsException(
                    "StreamSynchronizer: index out of range");

            try {
                cache_.push_back(generateNext());
            } catch (const IndexOutOFBoundsException&) {
                finished_ = true;
                throw IndexOutOFBoundsException(
                    "StreamSynchronizer: index out of range");
            }
        }
    }
};

template<typename T>
class StreamSynchronizer {
public:
    static LazySequence<ArraySequence<std::optional<double>>> Synchronize(
        const std::vector<TimeLazySequence<T>>& streams,
        double tau) {
        using GeneratorType = StreamSynchronizationGenerator<T>;
        using Result = ArraySequence<std::optional<double>>;

        auto generator =
            std::make_shared<GeneratorType>(streams, tau);

        return LazySequence<Result>(generator);
    }
};

#endif // STREAMSYNCHRONIZER_H
