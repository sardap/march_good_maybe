#include "march_game_scene.hpp"

#include <gfx/GfxMarchGameBackgroundGround.h>
#include <gfx/GfxMarchGameBackgroundSky.h>
#include <gfx/MarchGameBackgroundShared.h>
#include <gfx/MarchGameUnitsShared.h>
#include <tonc.h>

#include "../components.hpp"
#include "../ecs/ecs.hpp"
#include "../fixed/math.hpp"
#include "../frame_counter.hpp"
#include "../systems/systems.hpp"
#include "../units/units.hpp"
#include "controller.hpp"

namespace mgm::scenes {

namespace mgs {

enum class CBB { SHARED = 0, TEXT = 3 };

enum class SBB { SKY = 24, GROUND = 26, TEXT = 28 };

enum class Priority { TEXT_LAYER, GROUND_LAYER, SKY_LAYER };

};  // namespace mgs

static const Background SKY_BACKGROUND = Background::ZERO;
static const Background TEXT_BACKGROUND = Background::TWO;

static void generate_units_for_team(MarchGameData &data, const Teams team,
                                    int count, int start_x) {
    for (int i = 0; i < count; ++i) {
        int x = start_x;
        int y = (i * 20) + 40 + qran_range(2, 5);
        switch (qran_range(0, 0)) {
            case 0:
                units::create_front_line_unit(data, team, x, y);
            case 1:
                break;
                units::create_light_front_line_man_unit(data, team, x, y);
                break;
            case 2:
                units::create_ranged_man_unit(data, team, x, y);
                break;
        }
    }
}

static void generate_all_units(MarchGameData &data) {
    generate_units_for_team(data, Teams::RED, 6, 5);
    generate_units_for_team(data, Teams::BLUE, 3, 300);
}

static void load(SceneArgs &args) {
    irq_init(NULL);
    irq_enable(II_VBLANK);

    REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;

    // Sky background
    init_background(SKY_BACKGROUND, mgs::Priority::SKY_LAYER, mgs::CBB::SHARED,
                    mgs::SBB::SKY, BackgroundType::REG_32x32);
    set_background_scrolling(SKY_BACKGROUND, 0, 0);

    GRIT_CPY(pal_bg_mem, MarchGameBackgroundSharedPal);
    GRIT_CPY(tile_mem[static_cast<int>(mgs::CBB::SHARED)],
             MarchGameBackgroundSharedTiles);
    GRIT_CPY(se_mem[static_cast<int>(mgs::SBB::SKY)],
             GfxMarchGameBackgroundSkyMap);

    tte_init_se(static_cast<int>(TEXT_BACKGROUND),  // Background number (BG 0)
                BG_CBB(0) |
                    BG_PRIO(static_cast<int>(mgs::Priority::TEXT_LAYER)) |
                    BG_SBB(static_cast<int>(
                        TEXT_BACKGROUND)),  // BG control (for REG_BGxCNT)
                400,                        // Tile offset (special cattr)
                CLR_BLACK,                  // Ink color
                14,                         // BitUnpack offset (on-pixel = 15)
                NULL,                       // Default font (sys8)
                NULL);                      // Default renderer (se_drawg_s)

    g_scene_data = MarchGameData{};
    MarchGameData &data = std::get<MarchGameData>(g_scene_data);

    data.sky_x = 0;

    // Create units
    GRIT_CPY(pal_obj_mem, MarchGameUnitsSharedPal);
    generate_all_units(data);

    data.bg_ground.init(data.registry, GfxMarchGameBackgroundGroundMap,
                        GfxMarchGameBackgroundGroundMapLen / 64,
                        mgs::Priority::GROUND_LAYER, mgs::CBB::SHARED,
                        mgs::SBB::GROUND);
}

static void free() {}

static void update() {
    VBlankIntrWait();
    key_poll();

    auto &data = std::get<MarchGameData>(g_scene_data);

    static const DefaultFixed cam_speed = 8;

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

    systems::step_vel_system(data.registry);
    systems::step_collision_system(data.registry);
    systems::step_unit_logic_system(data.registry);
    systems::step_object_visible_system(data.registry,
                                        data.bg_ground.get_camera_id());
    systems::step_objects(data.registry);
}

Scene g_march_game_scene = {.load = load, .free = free, .update = update};

}  // namespace mgm::scenes
