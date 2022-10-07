#pragma once

#include "../entt.hpp"
#include "scene.hpp"

namespace mgm::scenes {

struct MarchGameData {
    entt::registry registry;
};

extern Scene g_march_game_scene;

}  // namespace mgm::scenes