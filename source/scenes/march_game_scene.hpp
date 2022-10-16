#pragma once

#include "../entt.hpp"
#include "../gfx_utils.hpp"
#include "../object.hpp"
#include "../scrolling_background.hpp"
#include "scene.hpp"

namespace mgm::scenes {

struct MarchGameData {
    entt::registry registry;
    TilesAllocator<50> tiles_allocator;
    PalletteAllocator<256> pallette_allocator;
    ObjectAllocator<120> object_allocator;
    u16 ground_map[SB_SIZE_SHORT];
    ScrollingBackground<Background::ONE, BackgroundType::REG_32x32> bg_ground;
    int sky_x;
};

extern Scene g_march_game_scene;

}  // namespace mgm::scenes