#pragma once

#include <tonc_types.h>

#include <functional>

#include "scene.hpp"

namespace mgm::scenes {

namespace main_menu {

struct Option {
    const std::function<void()> action;
    const u16* map;
    const uint map_len;
};

}  // namespace main_menu

struct MainMenuData {
    const main_menu::Option* active_option;
};

extern Scene g_main_menu_scene;

}  // namespace mgm::scenes