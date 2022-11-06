#pragma once

#include "../collision.hpp"
#include "../components.hpp"
#include "../entt.hpp"
#include "../units/unit_utils.hpp"

namespace mgm::systems {

template <IsCollisionEventContainer T>
void setp_unit_melee_dumb(entt::registry& registry, T& col_events_con,
                          const entt::entity entity, Unit& unit,
                          const Collision& col) {
    Unit::MeleeDumb& data = std::get<Unit::MeleeDumb>(unit.specific);

    switch (data.state) {
        using enum Unit::MeleeDumb::State;
        case JUST_SPAWNED: {
            Velocity& vel = registry.get<Velocity>(entity);
            vel.dx = unit.team == Teams::RED ? unit.speed : -unit.speed;
            data.state = FORWARD;
            break;
        }
        case FORWARD:
            break;
        case FIGHTING:
            if (data.attacking.has_value()) {
                Unit& other = registry.get<Unit>(*data.attacking);
                units::apply_damage(other, data.damage);
            }
            break;
    }
}

template <IsCollisionEventContainer T>
void setp_unit_ranged_dumb(entt::registry& registry, T& col_events_con,
                           const entt::entity entity, Unit& unit,
                           const Collision& col) {
    // Unit::RangedDumb& data = std::get<Unit::RangedDumb>(unit.specific);
}

template <IsCollisionEventContainer T>
void setp_unit_logic(entt::registry& registry, T& col_events_con,
                     const entt::entity entity, Unit& unit,
                     const Collision& col) {
    switch (unit_type_to_logic_type(unit.type)) {
        using enum LogicType;
        case MELEE_DUMB:
            setp_unit_melee_dumb(registry, col_events_con, entity, unit, col);
            break;
        case RANGED_DUMB:
            setp_unit_ranged_dumb(registry, col_events_con, entity, unit, col);
            break;
    }
}

void handle_unit_on_unit_collision(entt::registry& registry,
                                   const entt::entity entity,
                                   const entt::entity other_entity, Unit& unit,
                                   const Collision& col) {
    const Unit& other_unit = registry.get<Unit>(other_entity);
    // If same team ignore collision
    if (other_unit.team == unit.team) {
        return;
    }

    Velocity& vel = registry.get<Velocity>(entity);
    vel.dx = 0;

    switch (unit_type_to_logic_type(unit.type)) {
        using enum LogicType;
        case MELEE_DUMB: {
            Unit::MeleeDumb& data = std::get<Unit::MeleeDumb>(unit.specific);
            data.state = Unit::MeleeDumb::State::FIGHTING;
            data.attacking = other_entity;
            break;
        }
        // Update this to run
        case RANGED_DUMB: {
            Unit::RangedDumb& data = std::get<Unit::RangedDumb>(unit.specific);
            data.state = Unit::RangedDumb::State::FIGHTING;
            break;
        }
    }
}

template <IsCollisionEventContainer T>
void setp_unit_collision(entt::registry& registry, T& col_events_con,
                         const entt::entity entity, Unit& unit,
                         const Collision& col) {
    // Collision logic
    for (u32 i = 0; i < T::EVENT_COUNT; i++) {
        CollisionEventInstance event =
            col_events_con.get_collision_event(col.collision_events_key, i);

        if (!event.has_value()) {
            break;
        }

        switch (event->get().collision_type) {
            case CollisionTypes::UNIT:
                handle_unit_on_unit_collision(registry, entity,
                                              event->get().entity, unit, col);
                break;
            default:
                break;
        }
    }
}

template <IsCollisionEventContainer T>
void step_unit_logic_system(entt::registry& registry, T& col_events_con) {
    auto view = registry.view<Unit, const Collision>();

    for (auto entity : view) {
        auto [unit, col] = view.get<Unit, const Collision>(entity);
        setp_unit_logic(registry, col_events_con, entity, unit, col);
        setp_unit_collision(registry, col_events_con, entity, unit, col);
    }
}

}  // namespace mgm::systems