#pragma once

#include "../collision.hpp"
#include "../components.hpp"
#include "../ecs/ecs.hpp"

namespace mgm::systems {

template <ecs::IsRegistry T>
void step_collision_system(T& registry) {
    for (auto view_target = registry.template view<Position, Collision>();
         !view_target.complete(); view_target++) {
        auto target_coms = view_target.get();
        Position& target_pos = std::get<Position&>(target_coms);
        Collision& target_col = std::get<Collision&>(target_coms);

        reset_collision_events(target_col);

        ecs::EntityID target_entity = view_target.get_entity_id();

        u32 event_idx = 0;

        for (auto view_other = registry.template view<Position, Collision>();
             !view_other.complete(); view_other++) {
            ecs::EntityID other_entity = view_other.get_entity_id();

            if (target_entity == other_entity) {
                continue;
            }

            auto other_coms = view_other.get();
            Position& other_pos = std::get<Position&>(other_coms);
            Collision& other_col = std::get<Collision&>(other_coms);

            if (rect_overlap(target_pos, other_pos)) {
                add_collision_event(
                    target_col, event_idx,
                    CollisionEvent{.entity = other_entity,
                                   .collision_type = other_col.collision_type});
                event_idx++;
            }
        }
    }
}

}  // namespace mgm::systems