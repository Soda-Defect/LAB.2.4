#ifndef HEAPSORTER_H
#define HEAPSORTER_H

#include "ArraySequence.h"
#include "BinaryHeap.h"
#include "IStream.h"

template<typename T>
class HeapSorter {
public:
    static ArraySequence<T> Sort(IReadableStream<T>& stream);
};

template<typename T>
ArraySequence<T> HeapSorter<T>::Sort(IReadableStream<T>& stream) {
    BinaryHeap<T, std::less<T>> minHeap;
    stream.Open();
    while (!stream.IsEndOfStream()) {
        minHeap.Insert(stream.Read());
    }
    stream.Close();

    ArraySequence<T> sorted;
    while (minHeap.GetSize() > 0) {
        sorted.Append(minHeap.ExtractMin());
    }
    return sorted;
}

#endif // HEAPSORTER_H
