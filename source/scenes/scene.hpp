#pragma once

#include <functional>
#include <variant>

namespace mgm::scenes {

struct DefaultSceneArgs {};

using SceneArgs = std::variant<DefaultSceneArgs>;

extern SceneArgs g_scene_args;

template <typename T>
concept IsScene = requires(T c, SceneArgs& args) {
    requires c.load(args);
    requires c.free();
    requires c.update();
};

struct Scene {
    std::function<void(SceneArgs&)> load;
    std::function<void()> free;
    std::function<void()> update;
};

}  // namespace mgm