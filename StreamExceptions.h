#ifndef STREAMEXCEPTIONS_H
#define STREAMEXCEPTIONS_H
#pragma once
#include <stdexcept>
#include <string>

class EndOfStream : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class StreamError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class NotSupported : public std::logic_error {
public:
    using std::logic_error::logic_error;
};
#endif // STREAMEXCEPTIONS_H
