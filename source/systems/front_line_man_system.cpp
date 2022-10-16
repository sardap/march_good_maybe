#include "front_line_man_system.hpp"

#include "../components.hpp"

namespace mgm::systems {

void step_front_line_man(entt::registry& registry) {
    auto view = registry.view<FrontLineMan, Velocity>();

    for (auto entity : view) {
        auto& flm = view.get<FrontLineMan>(entity);
        auto& vel = view.get<Velocity>(entity);

        switch (flm.state) {
            case FrontLineMan::States::JUST_SPAWNED:
                vel.dx = flm.speed;
                vel.dy = 0;
                flm.state = FrontLineMan::States::MOVING;
                break;
            case FrontLineMan::States::MOVING:
                break;
            case FrontLineMan::States::FIGHTING:
                break;
        }
    }
}

}  // namespace mgm::systems