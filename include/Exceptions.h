#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class IndexOutOFBoundsException : public std::exception {
public:
    IndexOutOFBoundsException(const std::string &message) : msg_(message) {
    }

    const char *what() const throw() {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class SizeException : public std::exception {
public:
    SizeException(const std::string &message) : msg_(message) {
    }

    const char *what() const throw() {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

#endif // EXCEPTIONS_H