#pragma once

#include "background.hpp"
#include "components.hpp"
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
    int m_scroll_x;
    int m_cam_x;
    bool m_dirty;
    bool m_scrolling_dirty;
    int m_sbb;

    void update_background() {
        u32 cam_x_tile = math::clamp(static_cast<u32>(m_cam_x) / 8u, 0u,
                                     m_source_width_tiles);
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

    void init(const u16* source_map, u32 source_width_tiles, int priority,
              int cbb, int sbb) {
        init_background(BACKGROUND, priority, cbb, sbb, BACKGROUND_TYPE);
        set_background_scrolling(BACKGROUND, 0, 0);
        m_source_width_tiles = source_width_tiles;
        m_cam_x = 0;
        m_sbb = sbb;
        m_source_map = source_map;
        update_background();
    }

    void step() {
        if (m_dirty) {
            m_dirty = false;
            update_background();
        }

        if (m_scrolling_dirty) {
            m_scrolling_dirty = false;
            set_background_scrolling(BACKGROUND, static_cast<int>(m_scroll_x),
                                     0);
        }
    }

    void add_to_cam_x(int other) {
        int new_cam_x = clamp(m_cam_x + other, 0, 256 * 8);
        int delta = (new_cam_x - m_cam_x);
        m_cam_x += delta;
        m_scroll_x += delta;
        if (m_scroll_x >= 8) {
            m_dirty = true;
            m_scroll_x = 0;
        }
        m_scrolling_dirty = true;
    }
};

}  // namespace mgm