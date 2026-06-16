#ifndef SEQUENCEEXCEPTION_H
#define SEQUENCEEXCEPTION_H
#include <exception>
#include <string>

enum class ErrorCode {
    None = 0,
    IndexOutOfRange,
    EmptyContainer,
    InvalidSize,
    InvalidCount,

};


class SequenceException : public std::exception {
private:
    ErrorCode code_;
    std::string message_;
public:
    SequenceException(ErrorCode code, const std::string& msg)
        : code_(code), message_(msg) {}

    ErrorCode code() const noexcept { return code_; }
    const char* what() const noexcept override { return message_.c_str(); }
};


class IndexOutOFBoundsException : public SequenceException {
public:
    IndexOutOFBoundsException(const std::string& msg)
        : SequenceException(ErrorCode::IndexOutOfRange, msg) {}
};

class SizeException : public SequenceException {
public:
    SizeException(const std::string& msg)
        : SequenceException(ErrorCode::EmptyContainer, msg) {}
};
#endif // SEQUENCEEXCEPTION_H
