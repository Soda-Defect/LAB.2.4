#ifndef STREAMSYNCHRONIZER_H
#define STREAMSYNCHRONIZER_H

#include "LazySequence.h"
#include "TimeValue.h"
#include <vector>
#include <optional>
#include <algorithm>
#include <limits>

template<typename T>
class StreamSynchronizer {
public:
    static LazySequence<ArraySequence<std::optional<double>>> Synchronize(
        const std::vector<TimeLazySequence<T>>& streams, double tau);
};

template<typename T>
LazySequence<ArraySequence<std::optional<double>>> StreamSynchronizer<T>::Synchronize(
    const std::vector<TimeLazySequence<T>>& streams, double tau)
{
    std::vector<ArraySequence<TimedValue<T>>> allData;
    for (const auto& seq : streams) {
        ArraySequence<TimedValue<T>> data;
        size_t idx = 0;
        try {
            while (true) {
                data.Append(seq.Get(idx++));
            }
        } catch (const IndexOutOFBoundsException&) {}
        allData.push_back(data);
    }

    std::vector<size_t> positions(streams.size(), 0);
    ArraySequence<ArraySequence<std::optional<double>>> result;

    while (true) {
        double minTime = std::numeric_limits<double>::max();
        bool hasAny = false;
        for (size_t i = 0; i < streams.size(); ++i) {
            if (positions[i] < static_cast<size_t>(allData[i].GetLength())) {
                double t = allData[i].Get(static_cast<int>(positions[i])).timestamp;
                if (t < minTime) minTime = t;
                hasAny = true;
            }
        }
        if (!hasAny) break;

        ArraySequence<std::optional<double>> event;
        event.Append(minTime);

        for (size_t i = 0; i < streams.size(); ++i) {
            if (positions[i] < static_cast<size_t>(allData[i].GetLength())) {
                auto tv = allData[i].Get(static_cast<int>(positions[i]));
                if (tv.timestamp <= minTime + tau) {
                    event.Append(tv.value);
                    positions[i]++;
                } else {
                    event.Append(std::nullopt);
                }
            } else {
                event.Append(std::nullopt);
            }
        }

        double sum = 0.0;
        int count = 0;
        for (size_t i = 0; i < streams.size(); ++i) {
            if (positions[i] > 0 && positions[i]-1 < static_cast<size_t>(allData[i].GetLength())) {
                auto tv = allData[i].Get(static_cast<int>(positions[i]-1));
                sum += tv.timestamp;
                count++;
            }
        }
        double t_est = (count > 0 ? (sum / count) : minTime) - tau / 2.0;
        event.InsertAt(0, t_est);
        event.RemoveAt(1);
        result.Append(event);
    }
    return LazySequence<ArraySequence<std::optional<double>>>(result);
}

#endif // STREAMSYNCHRONIZER_H
