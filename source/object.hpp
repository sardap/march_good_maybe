#pragma once

#include "allocator.hpp"
#include "components.hpp"
#include "gfx_utils.hpp"

namespace mgm {

constexpr int g_oam_length = 128;
extern OBJ_ATTR g_oam[g_oam_length];

template <int OBJ_COUNT>
concept IsValidObjCount = (OBJ_COUNT < g_oam_length);

template <int OBJ_COUNT>
requires IsValidObjCount<OBJ_COUNT>
class ObjectAllocator : public Allocator<OBJ_COUNT> {
};

template <typename T>
concept IsAllocatorSet = requires(T a) {
    { a.tiles_allocator } -> std::convertible_to<IsAllocator>;
    { a.pallette_allocator } -> std::convertible_to<IsAllocator>;
};

}  // namespace mgm