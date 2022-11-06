#pragma once

#include <tonc_types.h>

#include <functional>
#include <optional>
#include <variant>

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
    bool x_flip;
    bool y_flip;
};

enum class CollisionTypes {
    CAMERA,
    UNIT,
};

struct Collision {
    CollisionTypes collision_type;
    u32 collision_events_key;
};

struct VisibleOnlyInCam {};

struct Camera {};

enum class Teams { RED, BLUE };

enum class UnitTypes { FRONT_LINE_MAN, LIGHT_FRONT_LINE_MAN, RANGED_MAN };

enum class LogicType { MELEE_DUMB, RANGED_DUMB };

inline LogicType unit_type_to_logic_type(UnitTypes type) {
    switch (type) {
        using enum UnitTypes;
        case FRONT_LINE_MAN:
        case LIGHT_FRONT_LINE_MAN:
            return LogicType::MELEE_DUMB;
        case RANGED_MAN:
            return LogicType::RANGED_DUMB;
    }

    return LogicType::MELEE_DUMB;
}

struct Unit {
    UnitTypes type;
    Teams team;
    DefaultFixed speed;
    int health;

    struct MeleeDumb {
        enum class State { JUST_SPAWNED, FORWARD, FIGHTING };
        State state;
        int damage;
        std::optional<entt::entity> attacking;
        int attack_cooldown;
        int attack_cooldown_remaining;
    };

    struct RangedDumb {
        enum class State { JUST_SPAWNED, FORWARD, FIGHTING };
        State state;
    };

    std::variant<MeleeDumb, RangedDumb> specific;
};

}  // namespace mgm