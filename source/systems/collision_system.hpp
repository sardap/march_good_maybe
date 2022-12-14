#pragma once

#include "../collision.hpp"
#include "../components.hpp"
#include "../entt.hpp"

namespace mgm::systems {

template <IsCollisionEventContainer T>
void step_collision_system(entt::registry& registry, T& col_events_con) {
    auto view = registry.view<const Position, Collision>();

    col_events_con.clear();

    for (auto entity : view) {
        auto [target_pos, target_col] =
            view.get<const Position, Collision>(entity);

        u32 event_idx = 0;

        for (auto other_entity : view) {
            if (entity == other_entity) {
                continue;
            }

            auto [other_pos, other_col] =
                view.get<const Position, Collision>(other_entity);

            if (rect_overlap(target_pos, other_pos)) {
                col_events_con.add_collision_event(
                    target_col.collision_events_key, event_idx,
                    {other_entity, other_col.collision_type});
                event_idx++;
            }
        }
    }
}

}  // namespace mgm::systems