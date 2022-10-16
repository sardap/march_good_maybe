#pragma once

#include <tonc.h>

#include "allocator.hpp"

namespace mgm {

constexpr int get_tile_count_4bpp(int len) { return len / 32; }

template <int TILES_COUNT>
concept IsValidTileCount = (TILES_COUNT < 1000);

template <int TILES_COUNT>
requires IsValidTileCount<TILES_COUNT>
class TilesAllocator : public Allocator<TILES_COUNT> {
};

template <int PAL_COUNT>
concept IsValidPalCount = (PAL_COUNT <= 256);

template <int PAL_COUNT>
requires IsValidPalCount<PAL_COUNT>
class PalletteAllocator : public Allocator<PAL_COUNT> {
};

constexpr static const u32 SB_SIZE = sizeof(SCREENBLOCK);
constexpr static const u32 SB_SIZE_SHORT = sizeof(SCREENBLOCK) / 2;

}  // namespace mgm