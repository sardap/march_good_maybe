#include "ranged_man_system.hpp"

#include "../components.hpp"

namespace mgm::systems {

void step_ranged_man(entt::registry& registry) {
    auto view = registry.view<RangedMan, Velocity>();

    for (auto entity : view) {
        auto [rm, vel] = view.get<RangedMan, Velocity>(entity);

        switch (rm.state) {
            case RangedMan::States::JUST_SPAWNED:
                vel.dx = rm.speed;
                vel.dy = 0;
                rm.state = RangedMan::States::MOVING;
                break;
            case RangedMan::States::MOVING:
                break;
            case RangedMan::States::FIGHTING:
                break;
        }
    }
}

}  // namespace mgm::systems