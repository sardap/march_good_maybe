#include "controller.hpp"

namespace mgm::scenes {

SceneDataSet g_scene_data = BlankSceneData{};

static Scene* s_active_scene = nullptr;
static Scene* s_next_scene = nullptr;

void change_scene(Scene* scene) { s_next_scene = scene; }

void step_scene() {
    if (s_next_scene) {
        if (s_active_scene) s_active_scene->free();
        s_active_scene = s_next_scene;
        s_active_scene->load(g_scene_args);
        s_next_scene = nullptr;
    }
    s_active_scene->update();
}

}  // namespace mgm::scenes
