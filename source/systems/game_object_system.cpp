#include "game_object_system.hpp"

#include <tonc.h>

#include "../components.hpp"
#include "../object.hpp"

namespace mgm::systems {

void step_objects(entt::registry& registry) {
    auto view = registry.view<const Object>();

    obj_hide_multi(g_oam, g_oam_length);

    for (auto entity : view) {
        auto& obj = view.get<const Object>(entity);

        OBJ_ATTR* game_object = &g_oam[obj.id];

        u16 a0 = ATTR0_4BPP | ATTR0_SQUARE;
        u16 a1 = ATTR1_SIZE_16x16 | (obj.x_flip ? ATTR1_HFLIP : 0) |
                 (obj.y_flip ? ATTR1_VFLIP : 0);
        u16 a2 = ATTR2_PRIO(0) | ATTR2_ID(obj.tile_offset);

        obj_set_attr(game_object, a0, a1, a2);

        if (obj.show) {
            obj_unhide(game_object, 0);
        }

        obj_set_pos(game_object, static_cast<int>(obj.x),
                    static_cast<int>(obj.y));
    }

    oam_copy(oam_mem, g_oam, g_oam_length);
}

}  // namespace mgm::systems
