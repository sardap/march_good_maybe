#pragma once

#include "../collision.hpp"
#include "../entt.hpp"

namespace mgm::systems {

template <IsCollisionEventContainer T>
bool in_camera(entt::entity cam_id, const Collision& col,
               const T& collision_events_container) {
    for (u32 i = 0; i < T::EVENT_COUNT; i++) {
        auto event = collision_events_container.get_collision_event(
            col.collision_events_idx, i);
        if (!event.has_value()) {
            break;
        }
        if (event->get().entity == cam_id) {
            return true;
        }
    }

    return false;
}

template <IsCollisionEventContainer T>
void step_object_visible_system(entt::registry& registry, entt::entity cam_id,
                                const T& collision_events) {
    auto view =
        registry
            .view<const Collision, const Position, Object, VisibleOnlyInCam>();

    auto& cam_pos = registry.get<const Position>(cam_id);

    for (auto entity : view) {
        auto [col, pos, obj] =
            view.get<const Collision, const Position, Object>(entity);

        if (!in_camera(cam_id, col, collision_events)) {
            continue;
        }

        // Get cam id and transform into screen postion
        obj.x = static_cast<int>(pos.x - cam_pos.x);
        obj.y = static_cast<int>(pos.y - cam_pos.y);
    }
}

}  // namespace mgm::systems