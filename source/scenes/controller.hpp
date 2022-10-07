#pragma once

#include <variant>

#include "main_menu_scene.hpp"
#include "march_game_scene.hpp"
#include "scene.hpp"

namespace mgm::scenes {

struct BlankSceneData {};

typedef std::variant<BlankSceneData, MainMenuData, MarchGameData> SceneDataSet;

extern SceneDataSet g_scene_data;

void change_scene(Scene* scene);
void step_scene();

}  // namespace mgm::scenes
