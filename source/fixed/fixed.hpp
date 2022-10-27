#pragma once

#include <tonc_math.h>
#include <tonc_types.h>

#include <concepts>

namespace mgm::math {

template <typename T>
class Fixed {
   public:
    Fixed(int val) : m_val(int2fx(val)) {}
    Fixed(float val) : m_val(float2fx(val)) {}
    Fixed(double val) : Fixed(static_cast<float>(val)) {}
    Fixed(const Fixed<T>& other) : m_val(other.m_val) {}
    Fixed() : m_val(0) {}

    static Fixed<T> raw_initialise(int val) {
        Fixed<T> result;
        result.m_val = val;
        return result;
    }

    explicit operator u32() const { return fx2uint(m_val); }
    explicit operator int() const { return fx2int(m_val); }
    explicit operator long int() const { return fx2int(m_val); }
    explicit operator float() const { return fx2float(m_val); }

    Fixed<T>& operator=(Fixed<T>& other) noexcept {
        if (this == &other) return *this;

        m_val = other.m_val;
        return *this;
    }

    Fixed<T>& operator=(const Fixed<T>& other) noexcept {
        m_val = other.m_val;
        return *this;
    }

    Fixed<T>& operator=(const u32 other) noexcept {
        m_val = int2fx(static_cast<int>(other));
        return *this;
    }

    Fixed<T> operator+(const Fixed<T>& other) const {
        return raw_initialise(m_val + other.m_val);
    }

    Fixed<T> operator++(int) noexcept {
        m_val += int2fx(1);
        return *this;
    }

    Fixed<T>& operator+=(const Fixed<T>& other) noexcept {
        m_val += other.m_val;
        return *this;
    }

    Fixed<T>& operator+=(const int other) noexcept {
        m_val += int2fx(other);
        return *this;
    }

    Fixed<T> operator-(const Fixed<T>& other) const {
        return raw_initialise(m_val - other.m_val);
    }

    Fixed<T> operator-() const { return raw_initialise(-m_val); }

    Fixed<T>& operator-=(const Fixed<T>& other) noexcept {
        m_val -= other.m_val;
        return *this;
    }

    Fixed<T> operator*(const Fixed<T>& other) const {
        return raw_initialise(fxmul(m_val, other.m_val));
    }

    Fixed<T>& operator*=(const Fixed<T>& other) noexcept {
        m_val = m_val * other.m_val;
        return *this;
    }

    Fixed<T> operator/(const Fixed<T>& other) const {
        return raw_initialise(fxdiv(m_val, other.m_val));
    }

    Fixed<T>& operator/=(const Fixed<T>& other) noexcept {
        m_val = m_val / other.m_val;
        return *this;
    }

    bool operator>(const Fixed<T>& other) const { return m_val > other.m_val; }
    bool operator>=(const Fixed<T>& other) const {
        return m_val >= other.m_val;
    }
    bool operator<(const Fixed<T>& other) const { return m_val < other.m_val; }
    bool operator<=(const Fixed<T>& other) const {
        return m_val <= other.m_val;
    }
    bool operator==(const Fixed<T>& other) const {
        return m_val == other.m_val;
    }
    bool operator!=(const Fixed<T>& other) const {
        return m_val != other.m_val;
    }

    int raw_value() const { return m_val; }

   private:
    int m_val;
};

}  // namespace mgm::math
