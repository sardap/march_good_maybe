#include "march_game_scene.hpp"

#include <gfx/GfxMarchGameBackgroundGround.h>
#include <gfx/GfxMarchGameBackgroundSky.h>
#include <gfx/MarchGameBackgroundShared.h>
#include <gfx/MarchGameUnitsShared.h>
#include <tonc.h>

#include "../components.hpp"
#include "../fixed/math.hpp"
#include "../frame_counter.hpp"
#include "../systems/systems.hpp"
#include "../units/units.hpp"
#include "controller.hpp"

namespace mgm::scenes {

static const int MGS_SHARED_CBB = 0;
static const int MGS_SKY_BACKGROUND_SBB = 24;
static const int MGS_GROUND_BACKGROUND_SBB = 26;
static const int MGS_SKY_BACKGROUND_LAYER_PRIO = 1;
static const int MGS_GROUND_BACKGROUND_LAYER_PRIO = 0;

static const Background SKY_BACKGROUND = Background::ZERO;

static void load(SceneArgs &args) {
    irq_init(NULL);
    irq_enable(II_VBLANK);

    REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;

    init_background(SKY_BACKGROUND, MGS_SKY_BACKGROUND_LAYER_PRIO,
                    MGS_SHARED_CBB, MGS_SKY_BACKGROUND_SBB,
                    BackgroundType::REG_32x32);
    set_background_scrolling(SKY_BACKGROUND, 0, 0);

    GRIT_CPY(pal_bg_mem, MarchGameBackgroundSharedPal);
    GRIT_CPY(tile_mem[MGS_SHARED_CBB], MarchGameBackgroundSharedTiles);
    GRIT_CPY(se_mem[MGS_SKY_BACKGROUND_SBB], GfxMarchGameBackgroundSkyMap);

    g_scene_data = MarchGameData{};
    MarchGameData &data = std::get<MarchGameData>(g_scene_data);

    data.sky_x = 0;

    // Create units
    GRIT_CPY(pal_obj_mem, MarchGameUnitsSharedPal);
    for (int i = 0; i < 6; ++i) {
        int x = 0;
        int y = (i * 20) + 40 + qran_range(2, 5);
        switch (qran_range(0, 3)) {
            case 0:
                units::create_front_line_unit(data, x, y);
                break;
            case 1:
                units::create_light_front_line_man_unit(data, x, y);
                break;
            case 2:
                units::create_ranged_man_unit(data, x, y);
                break;
        }
    }

    data.bg_ground.init(data.registry, data.collision_events_container,
                        GfxMarchGameBackgroundGroundMap, 256,
                        MGS_GROUND_BACKGROUND_LAYER_PRIO, MGS_SHARED_CBB,
                        MGS_GROUND_BACKGROUND_SBB);
}

static void free() {
    auto &data = std::get<MarchGameData>(g_scene_data);
    data.registry.clear();
}

static void update() {
    VBlankIntrWait();
    key_poll();

    auto &data = std::get<MarchGameData>(g_scene_data);

    static const DefaultFixed cam_speed = 3.5f;

    if (key_held(KEY_RIGHT)) {
        data.bg_ground.add_to_cam_x(data.registry, cam_speed);
        data.sky_x = clamp(data.sky_x + 1, 0, 10000);
    }
    if (key_held(KEY_LEFT)) {
        data.bg_ground.add_to_cam_x(data.registry, -cam_speed);
        data.sky_x = clamp(data.sky_x - 1, 0, 10000);
    }

    data.bg_ground.step(data.registry);

    set_background_x(SKY_BACKGROUND, data.sky_x);

    systems::step_light_front_line_man(data.registry);
    systems::step_front_line_man(data.registry);
    systems::step_ranged_man(data.registry);
    systems::step_vel_system(data.registry);
    systems::step_collision_system(data.registry,
                                   data.collision_events_container);
    systems::step_object_visible_system(data.registry,
                                        data.bg_ground.get_camera_id(),
                                        data.collision_events_container);
    systems::step_objects(data.registry);
}

Scene g_march_game_scene = {.load = load, .free = free, .update = update};

}  // namespace mgm::scenes
