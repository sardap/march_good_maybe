#pragma once

#include "../collision.hpp"
#include "../components.hpp"
#include "../ecs/ecs.hpp"
#include "../units/unit_utils.hpp"

namespace mgm::systems {

template <ecs::IsRegistry T>
void setp_unit_melee_dumb(T& registry, const ecs::EntityID entity, Unit& unit,
                          const Collision& col) {
    Unit::MeleeDumb& data = std::get<Unit::MeleeDumb>(unit.specific);

    switch (data.state) {
        using enum Unit::MeleeDumb::State;
        case JUST_SPAWNED: {
            // Move to function
            Velocity& vel = registry.template get_single<Velocity>(entity);
            vel.dx = unit.team == Teams::RED ? unit.speed : -unit.speed;
            data.state = FORWARD;
            break;
        }
        case FORWARD:
            break;
        case FIGHTING:
            if (data.attacking.has_value()) {
                // Check if unit we are attacking is dead
                if (!registry.entity_valid(*data.attacking)) {
                    data.attacking = std::nullopt;
                    // Move to function
                    Velocity& vel =
                        registry.template get_single<Velocity>(entity);
                    vel.dx = unit.team == Teams::RED ? unit.speed : -unit.speed;
                    data.state = FORWARD;
                    return;
                }

                if (data.attack_cooldown_remaining == 0) {
                    data.attack_cooldown_remaining = data.attack_cooldown;
                    Unit& other =
                        registry.template get_single<Unit>(*data.attacking);
                    units::apply_damage(other, data.damage);
                } else {
                    data.attack_cooldown_remaining--;
                }
            }
            break;
    }
}

template <ecs::IsRegistry T>
void setp_unit_ranged_dumb(T& registry, const ecs::EntityID entity, Unit& unit,
                           const Collision& col) {
    // Unit::RangedDumb& data = std::get<Unit::RangedDumb>(unit.specific);
}

template <ecs::IsRegistry T>
void setp_unit_logic(T& registry, const ecs::EntityID entity, Unit& unit,
                     const Collision& col) {
    switch (unit_type_to_logic_type(unit.type)) {
        using enum LogicType;
        case MELEE_DUMB:
            setp_unit_melee_dumb(registry, entity, unit, col);
            break;
        case RANGED_DUMB:
            setp_unit_ranged_dumb(registry, entity, unit, col);
            break;
    }
}

template <ecs::IsRegistry T>
void handle_unit_on_unit_collision(T& registry, const ecs::EntityID entity,
                                   const ecs::EntityID other_entity, Unit& unit,
                                   const Collision& col) {
    const Unit& other_unit = registry.template get_single<Unit>(other_entity);
    // If same team ignore collision
    if (other_unit.team == unit.team) {
        return;
    }

    Velocity& vel = registry.template get_single<Velocity>(entity);
    vel.dx = 0;

    switch (unit_type_to_logic_type(unit.type)) {
        using enum LogicType;
        case MELEE_DUMB: {
            Unit::MeleeDumb& data = std::get<Unit::MeleeDumb>(unit.specific);
            // Only start fighting if already not fighting
            if (data.state != Unit::MeleeDumb::State::FIGHTING) {
                data.state = Unit::MeleeDumb::State::FIGHTING;
                data.attacking = other_entity;
                data.attack_cooldown_remaining = 0;
            }
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

template <ecs::IsRegistry T>
void setp_unit_collision(T& registry, const ecs::EntityID entity, Unit& unit,
                         const Collision& col) {
    // Collision logic
    for (const auto& event : col.events) {
        if (!event.has_value()) {
            break;
        }

        switch (event->collision_type) {
            case CollisionTypes::UNIT:
                handle_unit_on_unit_collision(registry, entity, event->entity,
                                              unit, col);
                break;
            default:
                break;
        }
    }
}

template <ecs::IsRegistry T>
void kill_unit(T& registry, ecs::EntityID entity) {
    registry.template destroy(entity);
}

template <ecs::IsRegistry T>
void step_unit_logic_system(T& registry) {
    for (auto view = registry.template view<Unit, Collision>();
         !view.complete(); view++) {
        ecs::EntityID entity = view.get_entity_id();
        auto coms = view.get();
        Unit& unit = std::get<Unit&>(coms);
        const Collision& col = std::get<Collision&>(coms);

        if (unit.health <= 0) {
            kill_unit(registry, entity);
            continue;
        }

        setp_unit_collision(registry, entity, unit, col);
        setp_unit_logic(registry, entity, unit, col);
    }
}

}  // namespace mgm::systems