#pragma once

#include <tonc_core.h>

#include "fixed.hpp"

namespace mgm::math {

template <typename T>
inline Fixed<T> pow(Fixed<T> x, int y) {
    Fixed<T> result = Fixed<T>(1);
    while (y > 0) {
        if (y % 2 == 0)  // y is even
        {
            x *= x;
            y = y / 2;
        } else  // y isn't even
        {
            result = result * x;
            y = y - 1;
        }
    }
    return result;
}

template <typename T>
inline Fixed<T> rand_fixed() {
    return Fixed<T>(qran());
}

template <typename T>
inline Fixed<T> rand_fixed_range(Fixed<T> min, Fixed<T> max) {
    return Fixed<T>(qran_range(min.raw_value(), max.raw_value()));
}

}  // namespace mgm::math