#pragma once

#include <tonc_types.h>

#include "fixed/fixed.hpp"

namespace mgm {

using DefaultFixed = math::Fixed<int>;

struct Position {
    DefaultFixed x;
    DefaultFixed y;
};

struct Velocity {
    DefaultFixed dx;
    DefaultFixed dy;
};

struct Object {
    int id;
    int size;
    int tile_offset;
};

}  // namespace mgm