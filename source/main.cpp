#include <tonc.h>

#include "ecs/ecs.hpp"
#include "entt.hpp"
#include "frame_counter.hpp"
#include "scenes/controller.hpp"
#include "scenes/main_menu_scene.hpp"

struct ComOne {
    bool foo;
    int bar[50];
};

struct ComTwo {
    bool foo;
};

struct ComThree {
    bool foo;
};

int main(void) {
    using namespace mgm;

    constexpr int ENTITY_COUNT = 50;
    using Registry =
        ecs::Registry<ENTITY_COUNT,
                      ecs::CompetentPool<ComOne, 50, ENTITY_COUNT>,
                      ecs::CompetentPool<ComTwo, 50, ENTITY_COUNT>,
                      ecs::CompetentPool<ComThree, 50, ENTITY_COUNT>>;

    Registry registry;
    ecs::EntityID entity_one = registry.create();
    ecs::EntityID entity_two = registry.create();
    registry.emplace(entity_one, ComOne{.foo = true, .bar = {}});
    registry.emplace(entity_one, ComTwo{.foo = true});

    registry.emplace(entity_two, ComOne{.foo = true, .bar = {}});
    registry.emplace(entity_two, ComTwo{.foo = true});

    {
        auto [com_one, com_two] = registry.get<ComOne, ComTwo>(entity_one);

        com_two.foo = false;
        com_two.foo = false;
    }

    {
        auto [com_one] = registry.get<ComOne>(entity_two);

        com_one.foo = false;
    }

    for (auto view = registry.view<ComOne, ComTwo>(); !view.complete();
         view++) {
        auto [com_one, com_two] = view.get();
        com_one.foo = true;
    }

    registry.destroy(entity_one);

    for (auto view = registry.view<ComOne, ComTwo>(); !view.complete();
         view++) {
        auto [com_one, com_two] = view.get();
        com_one.foo = true;
    }

    reset_frame_counter();

    scenes::change_scene(&scenes::g_main_menu_scene);

    for (;;) {
        step_frame_counter();
        scenes::step_scene();
    }
}
