#include "march_game_scene.hpp"

#include <gfx/GfxMarchGameBackgroundGround.h>
#include <gfx/GfxMarchGameBackgroundSky.h>
#include <gfx/MarchGameBackgroundShared.h>
#include <tonc.h>

#include "../components.hpp"
#include "../fixed/math.hpp"
#include "../frame_counter.hpp"
#include "../object.hpp"
#include "../systems/game_object_system.hpp"
#include "controller.hpp"

namespace mgm::scenes {

static const int MGS_SHARED_CBB = 0;
static const int MGS_SKY_BACKGROUND_SBB = 24;
static const int MGS_GROUND_BACKGROUND_SBB = 26;
static const int MGS_SKY_BACKGROUND_LAYER_PRIO = 1;
static const int MGS_GROUND_BACKGROUND_LAYER_PRIO = 0;

static void load(SceneArgs& args) {
    irq_init(NULL);
    irq_enable(II_VBLANK);

    REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;

    REG_BG0CNT = BG_PRIO(MGS_SKY_BACKGROUND_LAYER_PRIO) | BG_4BPP |
                 BG_CBB(MGS_SHARED_CBB) | BG_SBB(MGS_SKY_BACKGROUND_SBB) |
                 BG_REG_32x32;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    REG_BG1CNT = BG_PRIO(MGS_GROUND_BACKGROUND_LAYER_PRIO) | BG_4BPP |
                 BG_CBB(MGS_SHARED_CBB) | BG_SBB(MGS_GROUND_BACKGROUND_SBB) |
                 BG_REG_32x32;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;

    GRIT_CPY(pal_bg_mem, MarchGameBackgroundSharedPal);
    GRIT_CPY(tile_mem[MGS_SHARED_CBB], MarchGameBackgroundSharedTiles);
    GRIT_CPY(se_mem[MGS_SKY_BACKGROUND_SBB], GfxMarchGameBackgroundSkyMap);
    memcpy16(se_mem[MGS_GROUND_BACKGROUND_SBB], GfxMarchGameBackgroundGroundMap,
             2048);

    g_scene_data = MarchGameData{};
    auto& data = std::get<MarchGameData>(g_scene_data);

    for (int i = 0; i < 6; ++i) {
        const auto entity = data.registry.create();
        data.registry.emplace<Position>(entity, 0,
                                        (i * 20) + 40 + qran_range(2, 5));
        data.registry.emplace<Velocity>(entity, 0, 0);
        data.registry.emplace<Object>(entity);
        create_red_square_unit(data.registry.get<Object>(entity));
    }
}

static void free() {}

static int tmp = 0;

static void update() {
    VBlankIntrWait();
    key_poll();

    auto& data = std::get<MarchGameData>(g_scene_data);

    if (get_frame_count() % 1000000 == 0) {
        tmp = wrap(tmp + 256, 0, GfxMarchGameBackgroundGroundMapLen / 2);
        memcpy16(se_mem[MGS_GROUND_BACKGROUND_SBB],
                 GfxMarchGameBackgroundGroundMap + tmp, 2048);
    }

    auto view = data.registry.view<Position, Velocity>();

    for (auto entity : view) {
        auto& vel = view.get<Velocity>(entity);
        auto& pos = view.get<Position>(entity);
        pos.x += vel.dx;
        pos.y += vel.dy;
    }

    systems::step_objects(data.registry);
}

Scene g_march_game_scene = {.load = load, .free = free, .update = update};

}  // namespace mgm::scenes
