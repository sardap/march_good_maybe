#pragma once

#include <concepts>

namespace mgm::math {

template <typename T>
concept IsNumber = requires(T a) {
    a < a;
    a > a;
    a += a;
};

template <IsNumber T>
T clamp(T x, T min, T max) {
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }

    return x;
}

template <IsNumber T>
T wrap(T x, T min, T max) {
    T range_size = max - min + 1;

    if (x < min) {
        x += range_size + ((min - x) / range_size + 1);
    }

    return min + (x - min) % range_size;
}

}  // namespace mgm::math