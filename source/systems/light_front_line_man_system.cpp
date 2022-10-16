#include "light_front_line_man_system.hpp"

#include "../components.hpp"

namespace mgm::systems {

void step_light_front_line_man(entt::registry& registry) {
    auto view = registry.view<LightFrontLine, Velocity>();

    for (auto entity : view) {
        auto& flm = view.get<LightFrontLine>(entity);
        auto& vel = view.get<Velocity>(entity);

        switch (flm.state) {
            case LightFrontLine::States::JUST_SPAWNED:
                vel.dx = flm.speed;
                vel.dy = 0;
                flm.state = LightFrontLine::States::MOVING;
                break;
            case LightFrontLine::States::MOVING:
                break;
            case LightFrontLine::States::FIGHTING:
                break;
        }
    }
}

}  // namespace mgm::systems