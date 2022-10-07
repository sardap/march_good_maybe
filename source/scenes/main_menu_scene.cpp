#include "main_menu_scene.hpp"

#include <gfx/GfxMainMenuBackground.h>
#include <gfx/MainMenuBackgroundShared.h>
#include <tonc.h>

#include "controller.hpp"
#include "march_game_scene.hpp"

namespace mgm::scenes {

static const int MMS_SHARED_CBB = 0;
static const int MMS_BACKGROUND_SBB = 24;
static const int MMS_TEXT_SBB = 26;
static const int MMS_BACKGROUND_LAYER = 1;
static const int MMS_BACKGROUND_LAYER_PRIO = 0;

static void start_main_game();

static const int options_length = 1;
static const main_menu::Option options[options_length] = {
    {.action = start_main_game,
     .map = GfxMainMenuBackgroundMap,
     .map_len = GfxMainMenuBackgroundMapLen}};

static void load(SceneArgs& args) {
    g_scene_data = MainMenuData{.active_option = &options[0]};

    irq_init(NULL);
    irq_enable(II_VBLANK);

    REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_MODE0 | DCNT_BG0;
    REG_BG0CNT = BG_PRIO(MMS_BACKGROUND_LAYER_PRIO) | BG_4BPP |
                 BG_CBB(MMS_SHARED_CBB) | BG_SBB(MMS_BACKGROUND_SBB) |
                 BG_REG_32x32;

    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    GRIT_CPY(pal_bg_mem, MainMenuBackgroundSharedPal);
    GRIT_CPY(tile_mem[MMS_SHARED_CBB], GfxMainMenuBackgroundTiles);
    GRIT_CPY(se_mem[MMS_BACKGROUND_SBB], GfxMainMenuBackgroundMap);
}

static void free() {}

static void update() {
    VBlankIntrWait();
    key_poll();

    MainMenuData& data = std::get<MainMenuData>(g_scene_data);

    if (key_hit(KEY_A)) {
        data.active_option->action();
        return;
    }

    qran();
}

static void start_main_game() { change_scene(&g_march_game_scene); }

Scene g_main_menu_scene = {.load = load, .free = free, .update = update};

}  // namespace mgm::scenes
