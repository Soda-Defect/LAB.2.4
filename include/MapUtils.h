#ifndef MAPUTILS_H
#define MAPUTILS_H
#include <cmath>
#include <string>

inline char Upper(char &c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}

inline char Lower(char &c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

inline std::string ToCesarСipher(std::string &item, int shag) {
    std::string result = "";
    for (int i = 0; i < item.size(); i++) {
        if (item[i] >= 'A' && item[i] <= 'Z') {
            result += (char) (((item[i] - 'A') + shag) % 26 + 65);
        } else if (item[i] >= 'a' && item[i] <= 'z') {
            result += (char) (((item[i] - 'a') + shag) % 26 + 97);
        } else {
            result += item[i];
        }
    }
    return result;
}

namespace functions {
namespace maper {
namespace IntsDouble {
template<typename T,typename M>
auto increser = [](const T &item, M inc) {
    return item + inc;
};


template<typename T, typename D>
T division(T value, D divisor) {
    return value / divisor;
}

template<typename T, typename M>
T multiplication(const T &item, M mul) {
    return item * mul;
}

template<typename T>
T sinus(const T &item) {
    return std::sin(item);
}

template<typename T>
T cosinus(const T &item) {
    return std::cos(item);
}
}

namespace chars {
inline auto upper(char& ch)->char{
    return Upper(ch);
}


inline auto lower(char& ch)->char{
    return Lower(ch);
}
}

namespace strings {
inline auto CaMeLcAsE(std::string &s) {
    std::string result = "";
    for (int j = 0; j < s.size(); j++) {
        if (j % 2 == 0) {
            result += Upper(s[j]);
        } else {
            result += Lower(s[j]);
        }
    }
    return result;
}
inline auto ToCesarCase(std::string &s) {
    return ToCesarСipher(s, 4);
}
inline auto UpperCase(std::string &s) {
    std::string result = "";
    for (int j = 0; j < s.size(); j++) {
        result += Upper(s[j]);
    }
    return result;
}
inline auto LowerCase(std::string &s) {
    std::string result = "";
    for (int j = 0; j < s.size(); j++) {
        result += Lower(s[j]);
    }
    return result;
}
}
}
namespace reduce {

inline double average(int& result, int element) {
    return (result + element) / 2;
}

inline int mnogochlen(int& result, int element) {
    return 2*result*result+5*element;
}


}
namespace where {
template<typename T>
bool is_even(T element) {
    static_assert(std::is_integral_v<T>, "is_even supports only integral types");
    return element % 2 == 0;
}
template<typename T>
bool is_odd(T element) {
    static_assert(std::is_integral_v<T>, "is_odd supports only integral types");
    return element % 2 == 1;
}
template<typename T>
bool isAnswer(T element) {
    return 12*element*element*element-4*element+3==0;
}
}

}

#endif // MAPUTILS_H