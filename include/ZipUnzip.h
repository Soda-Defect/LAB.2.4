#ifndef ZIPUNZIP_H
#define ZIPUNZIP_H
#include <tuple>
#include <algorithm>
#include "ArraySequence.h"

template<typename T>
Sequence<std::tuple<T, T> > *ZipSequences(Sequence<T> *a, Sequence<T> *b) {
    int maxLength = std::max(a->GetLength(), b->GetLength());
    auto *result = new ArraySequence<std::tuple<T, T> >();
    for (int i = 0; i < maxLength; i++) {
        T left = (i < a->GetLength()) ? a->Get(i) : T();
        T right = (i < b->GetLength()) ? b->Get(i) : T();
        result->Append(std::make_tuple(left, right));
    }
    return result;
}

template<typename T>
std::pair<Sequence<T> *, Sequence<T> *> UnzipSequences(Sequence<std::tuple<T, T> > *zipped) {
    auto *firstSeq = new ArraySequence<T>();
    auto *secondSeq = new ArraySequence<T>();
    for (int i = 0; i < zipped->GetLength(); i++) {
        auto t = zipped->Get(i);
        firstSeq->Append(std::get<0>(t));
        secondSeq->Append(std::get<1>(t));
    }
    return {firstSeq, secondSeq};
}
#endif // ZIPUNZIP_H