#pragma once

#include "../collision.hpp"
#include "../ecs/ecs.hpp"
#include "../gfx_utils.hpp"
#include "../object.hpp"
#include "../scrolling_background.hpp"
#include "scene.hpp"

namespace mgm::scenes {

constexpr int MARCH_GAME_ENTITY_COUNT = 50;
constexpr int MARCH_GAME_UNIT_COUNT = 25;
constexpr int MARCH_GAME_CAMERA_COUNT = 1;

struct MarchGameData {
    using Registry = ecs::Registry<
        MARCH_GAME_ENTITY_COUNT,
        ecs::CompetentPool<Position, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_ENTITY_COUNT>,
        ecs::CompetentPool<Velocity, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_ENTITY_COUNT>,
        ecs::CompetentPool<Object, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_ENTITY_COUNT>,
        ecs::CompetentPool<Collision, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_UNIT_COUNT>,
        ecs::CompetentPool<VisibleOnlyInCam, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_UNIT_COUNT>,
        ecs::CompetentPool<Camera, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_UNIT_COUNT>,
        ecs::CompetentPool<Unit, MARCH_GAME_ENTITY_COUNT,
                           MARCH_GAME_UNIT_COUNT>>;
    Registry registry;
    TilesAllocator<50> tiles_allocator;
    PalletteAllocator<256> pallette_allocator;
    ObjectAllocator<120> object_allocator;
    u16 ground_map[SB_SIZE_SHORT];
    ScrollingBackground<Background::ONE, BackgroundType::REG_32x32> bg_ground;
    int sky_x;
};

extern Scene g_march_game_scene;

}  // namespace mgm::scenes