#include <tonc.h>

#include "main_menu_scene.hpp"

namespace mgm::scenes {

static const int BLANK_SCENE_SHARED_CBB = 0;
static const int BLANK_SCENE_BACKGROUND_SBB = 24;
static const int BLANK_SCENE_TEXT_SBB = 26;
static const int BLANK_SCENE_BACKGROUND_LAYER = 1;
static const int BLANK_SCENE_BACKGROUND_LAYER_PRIO = 0;

static void load(SceneArgs& args) {
    irq_init(NULL);
    irq_enable(II_VBLANK);

    REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_MODE0 | DCNT_BG0;
    REG_BG0CNT = BG_PRIO(BLANK_SCENE_BACKGROUND_LAYER_PRIO) | BG_4BPP |
                 BG_CBB(BLANK_SCENE_SHARED_CBB) |
                 BG_SBB(BLANK_SCENE_BACKGROUND_SBB) | BG_REG_32x32;

    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
}

static void free() {}

static void update() {
    VBlankIntrWait();
    key_poll();
}

Scene g_blank_scene = {.load = load, .free = free, .update = update};

}  // namespace mgm::scenes
