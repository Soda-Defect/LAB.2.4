#ifndef CARDINALITY_H
#define CARDINALITY_H

#include <string>
#include <stdexcept>

class Cardinality {
public:

    enum Type {
        kFinite,
        kCountableInfinity,
        kContinuum
    };


    Cardinality() : type_(kFinite), value_(0) {}
    Cardinality(size_t n) : type_(kFinite), value_(n) {}
    static Cardinality Countable() { return Cardinality(kCountableInfinity, 0); }
    static Cardinality Continuum() { return Cardinality(kContinuum, 0); }

    // Геттеры
    Type GetType() const { return type_; }
    size_t GetValue() const { return value_; } // для kFinite


    std::string ToString() const {
        switch (type_) {
        case kFinite:
            return std::to_string(value_);
        case kCountableInfinity:
            return "ℵ₀";
        case kContinuum:
            return "𝔠 (континуум)";
        }
        return "?";
    }


    bool operator==(const Cardinality& other) const {
        if (type_ != other.type_) return false;
        if (type_ == kFinite) return value_ == other.value_;
        return true;
    }
    bool operator!=(const Cardinality& other) const { return !(*this == other); }
    bool operator<(const Cardinality& other) const {
        if (type_ != other.type_) return type_ < other.type_;
        if (type_ == kFinite) return value_ < other.value_;
        return false;
    }
    bool operator<=(const Cardinality& other) const { return (*this < other) || (*this == other); }
    bool operator>(const Cardinality& other) const { return other < *this; }
    bool operator>=(const Cardinality& other) const { return !(*this < other); }


    Cardinality operator+(const Cardinality& other) const {
        if (type_ == kCountableInfinity || other.type_ == kCountableInfinity)
            return Cardinality::Countable();
        if (type_ == kContinuum || other.type_ == kContinuum)
            return Cardinality::Continuum();
        return Cardinality(value_ + other.value_);
    }


    Cardinality operator*(const Cardinality& other) const {
        if (type_ == kFinite && other.type_ == kFinite)
            return Cardinality(value_ * other.value_);
        if ((type_ == kCountableInfinity || other.type_ == kCountableInfinity) &&
            type_ != kContinuum && other.type_ != kContinuum)
            return Cardinality::Countable();
        return Cardinality::Continuum();
    }

private:
    Cardinality(Type t, size_t v) : type_(t), value_(v) {}

    Type type_;
    size_t value_;
};

#endif // CARDINALITY_H
