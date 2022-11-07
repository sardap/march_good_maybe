#pragma once

#include "background.hpp"
#include "components.hpp"
#include "ecs/ecs.hpp"
#include "gfx_utils.hpp"
#include "math/math.hpp"

namespace mgm {

template <Background BACKGROUND, BackgroundType BACKGROUND_TYPE>
class ScrollingBackground {
   private:
    static const u32 BG_WIDTH = 32;

    u32 m_source_width_tiles;
    const u16* m_source_map;
    u16 m_map[SB_SIZE_SHORT];
    DefaultFixed m_scroll_x;
    bool m_tile_dirty;
    bool m_scrolling_dirty;
    int m_sbb;
    ecs::EntityID m_cam_id;

    template <ecs::IsRegistry T>
    Position& get_camera(T& registry) {
        Position& result = registry.template get_single<Position>(m_cam_id);
        return result;
    }

    template <ecs::IsRegistry T>
    void update_background(T& registry) {
        Position& pos = get_camera(registry);

        u32 cam_x_tile =
            math::clamp(static_cast<u32>(pos.x) / 8u, 0u, m_source_width_tiles);
        for (u32 x = 0; x < 32; x++) {
            for (u32 y = 0; y < 32; y++) {
                u32 global_idx = ((x + cam_x_tile) + y * m_source_width_tiles);
                u32 local_idx = x + y * BG_WIDTH;
                auto other = m_source_map[global_idx];
                m_map[local_idx] = other;
            }
        }

        memcpy16(&se_mem[m_sbb], m_map, SB_SIZE_SHORT);
    }

   public:
    ScrollingBackground() {}

    template <ecs::IsRegistry T>
    void init(T& registry, const u16* source_map, u32 source_width_tiles,
              int priority, int cbb, int sbb) {
        const auto entity = registry.create();
        m_cam_id = entity;

        registry.template emplace<Collision>(
            m_cam_id,
            Collision{.collision_type = CollisionTypes::CAMERA, .events = {}});
        registry.template emplace<Position>(m_cam_id,
                                            Position{.x = 0,
                                                     .y = 0,
                                                     .width = SCREEN_WIDTH,
                                                     .height = SCREEN_HEIGHT});
        registry.template emplace<Camera>(m_cam_id, Camera{});

        init_background(BACKGROUND, priority, cbb, sbb, BACKGROUND_TYPE);
        set_background_scrolling(BACKGROUND, 0, 0);
        m_source_width_tiles = source_width_tiles;
        m_sbb = sbb;
        m_source_map = source_map;

        m_scrolling_dirty = false;
        m_tile_dirty = true;

        update_background(registry);
    }

    template <ecs::IsRegistry T>
    void step(T& registry) {
        if (m_tile_dirty) {
            m_tile_dirty = false;
            update_background(registry);
        }

        if (m_scrolling_dirty) {
            m_scrolling_dirty = false;
            set_background_scrolling(BACKGROUND, static_cast<int>(m_scroll_x),
                                     0);
        }
    }

    template <ecs::IsRegistry T>
    void add_to_cam_x(T& registry, DefaultFixed val) {
        const DefaultFixed min_cam_x = 0;
        const DefaultFixed max_cam_x =
            DefaultFixed(static_cast<int>(m_source_width_tiles));

        auto& camera = get_camera(registry);
        DefaultFixed new_cam_x =
            math::clamp(camera.x + val, min_cam_x, max_cam_x);
        DefaultFixed delta = (new_cam_x - camera.x);
        camera.x += delta;
        m_scroll_x += delta;
        if (m_scroll_x >= 8) {
            m_tile_dirty = true;
            m_scroll_x = 0;
        }
        m_scrolling_dirty = true;
    }

    ecs::EntityID get_camera_id() { return m_cam_id; }
};

}  // namespace mgm