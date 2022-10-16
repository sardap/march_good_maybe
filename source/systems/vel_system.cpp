#include "../components.hpp"
#include "../entt.hpp"

namespace mgm::systems {

void step_vel_system(entt::registry& registry) {
    auto view = registry.view<Position, Velocity>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);

        pos.x += vel.dx;
        pos.y += vel.dy;
    }
}

}  // namespace mgm::systems