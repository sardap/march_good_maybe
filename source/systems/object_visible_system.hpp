#pragma once

#include "../collision.hpp"
#include "../ecs/ecs.hpp"

namespace mgm::systems {

bool in_camera(ecs::EntityID cam_id, const Collision& col) {
    for (auto event : col.events) {
        if (!event.has_value()) {
            break;
        }
        if (event->entity == cam_id) {
            return true;
        }
    }

    return false;
}

template <ecs::IsRegistry T>
void step_object_visible_system(T& registry, ecs::EntityID cam_id) {
    const Position& cam_pos = registry.template get_single<Position>(cam_id);

    for (auto view = registry.template view<Collision, Position, Object,
                                            VisibleOnlyInCam>();
         !view.complete(); view++) {
        auto coms = view.get();
        const Collision& col = std::get<Collision&>(coms);
        const Position& pos = std::get<Position&>(coms);
        Object& obj = std::get<Object&>(coms);

        obj.show = false;

        if (!in_camera(cam_id, col)) {
            continue;
        }

        obj.show = true;

        // Get cam id and transform into screen postion
        obj.x = static_cast<int>(pos.x - cam_pos.x);
        obj.y = static_cast<int>(pos.y - cam_pos.y);
    }
}

}  // namespace mgm::systems