#pragma once

#include <tonc_types.h>

namespace mgm {

extern u64 g_frame_count;

inline void reset_frame_counter() { g_frame_count = 0; }

inline void step_frame_counter() { g_frame_count++; }

inline u64 get_frame_count() { return g_frame_count; }

}  // namespace mgm