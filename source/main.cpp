#include <tonc.h>

#include "entt.hpp"
#include "frame_counter.hpp"
#include "scenes/controller.hpp"
#include "scenes/main_menu_scene.hpp"

int main(void) {
    using namespace mgm;

    reset_frame_counter();

    scenes::change_scene(&scenes::g_main_menu_scene);

    for (;;) {
        step_frame_counter();
        scenes::step_scene();
    }
}
