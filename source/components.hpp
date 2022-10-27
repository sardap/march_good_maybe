#pragma once

#include <tonc_types.h>

#include <optional>

#include "entt.hpp"
#include "fixed/fixed.hpp"

namespace mgm {

using DefaultFixed = math::Fixed<int>;

struct Position {
    DefaultFixed x;
    DefaultFixed y;
    DefaultFixed width;
    DefaultFixed height;
};

struct Velocity {
    DefaultFixed dx;
    DefaultFixed dy;
};

struct Object {
    int x;
    int y;
    int id;
    int size;
    int tile_offset;
    int pal_offset;
    bool show;
};

enum class CollisionTypes {
    CAMERA,
    LINE_MAN,
    LINE_LIGHT_MAN,
    RANGED_MAN,

};

struct Collision {
    CollisionTypes collision_type;
    u32 collision_events_key;
};

struct VisibleOnlyInCam {};

struct Camera {};

struct FrontLineMan {
    enum class States { JUST_SPAWNED, MOVING, FIGHTING };

    DefaultFixed speed;
    States state;
};

struct LightFrontLine {
    enum class States { JUST_SPAWNED, MOVING, FIGHTING };

    DefaultFixed speed;
    States state;
};

struct RangedMan {
    enum class States { JUST_SPAWNED, MOVING, FIGHTING };

    DefaultFixed speed;
    States state;
};

}  // namespace mgm