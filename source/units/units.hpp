#pragma once

#include <gfx/GfxMarchGameUnitsFrontLineMan.h>
#include <gfx/GfxMarchGameUnitsLightFrontLineMan.h>
#include <gfx/GfxMarchGameUnitsRangedMan.h>
#include <gfx/MarchGameUnitsShared.h>
#include <tonc.h>

#include "../components.hpp"
#include "../entt.hpp"
#include "../object.hpp"

namespace mgm::units {

// https://stackoverflow.com/questions/57751484/are-types-allowed-to-be-specified-for-return-type-requirement-in-a-requires-expr
// https://stackoverflow.com/questions/69686934/concept-checking-on-struct-members
template <typename T>
concept IsUnitCreationSet = requires(T a) {
    requires IsAllocatorSet<T>;
};

template <IsAllocatorSet UCS>
void create_base_unit(UCS& ucs, const entt::entity& entity, int start_x,
                      int start_y) {
    entt::registry& registry = ucs.registry;

    registry.emplace<Position>(entity, start_x, start_y);
    registry.emplace<Velocity>(entity, 0, 0);
}

template <IsAllocatorSet UCS>
void create_front_line_unit(UCS& ucs, const entt::entity& entity, int start_x,
                            int start_y) {
    entt::registry& registry = ucs.registry;

    create_base_unit(ucs, entity, start_x, start_y);

    auto& obj = registry.emplace<Object>(entity);

    obj.id = ucs.object_allocator.allocate(1);
    obj.size = ATTR1_SIZE_16x16;

    obj.tile_offset = ucs.tiles_allocator.allocate(
        get_tile_count_4bpp(GfxMarchGameUnitsFrontLineManTilesLen));
    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsFrontLineManTiles);

    static const float speed = 0.0f;
    registry.emplace<FrontLineMan>(entity, speed);
}

template <IsAllocatorSet UCS>
void create_light_front_line_man_unit(UCS& ucs, const entt::entity& entity,
                                      int start_x, int start_y) {
    entt::registry& registry = ucs.registry;

    create_base_unit(ucs, entity, start_x, start_y);

    auto& obj = registry.emplace<Object>(entity);

    obj.id = ucs.object_allocator.allocate(1);
    obj.size = ATTR1_SIZE_16x16;

    obj.tile_offset = ucs.tiles_allocator.allocate(
        get_tile_count_4bpp(GfxMarchGameUnitsLightFrontLineManTilesLen));
    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsLightFrontLineManTiles);

    static const float speed = 0.0f;
    registry.emplace<LightFrontLine>(entity, speed);
}

template <IsAllocatorSet UCS>
void create_ranged_man_unit(UCS& ucs, const entt::entity& entity, int start_x,
                            int start_y) {
    entt::registry& registry = ucs.registry;

    create_base_unit(ucs, entity, start_x, start_y);

    auto& obj = registry.emplace<Object>(entity);

    obj.id = ucs.object_allocator.allocate(1);
    obj.size = ATTR1_SIZE_16x16;

    obj.tile_offset = ucs.tiles_allocator.allocate(
        get_tile_count_4bpp(GfxMarchGameUnitsRangedManTilesLen));
    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsRangedManTiles);

    static const float speed = 0.0f;
    registry.emplace<RangedMan>(entity, speed);
}

}  // namespace mgm::units