#pragma once

#include "../components.hpp"
#include "../ecs/ecs.hpp"

namespace mgm::systems {

template <ecs::IsRegistry T>
void step_vel_system(T& registry) {
    for (auto view = registry.template view<Position, Velocity>();
         !view.complete(); view++) {
        auto [pos, vel] = view.get();

        pos.x += vel.dx;
        pos.y += vel.dy;
    }
}

}  // namespace mgm::systems