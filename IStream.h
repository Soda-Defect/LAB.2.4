#ifndef ISTREAM_H
#define ISTREAM_H
#pragma once
#include "StreamExceptions.h"

template<typename T>
class IStream {
public:
    virtual ~IStream() = default;
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual bool IsOpen() const = 0;
    virtual bool IsEndOfStream()  = 0;
};

template<typename T>
class IReadableStream : public virtual IStream<T> {
public:
    virtual T Read() = 0;
    virtual size_t GetPosition() const = 0;
    virtual bool IsCanSeek() const = 0;
    virtual size_t Seek(size_t index) = 0;
    virtual bool IsCanGoBack() const = 0;
};

template<typename T>
class IWritableStream : public virtual IStream<T> {
public:
    virtual void Write(const T& value) = 0;
    virtual void Flush() = 0;
};

template<typename T>
class IReadWriteStream : public IReadableStream<T>, public IWritableStream<T> {
};
#endif // ISTREAM_H
