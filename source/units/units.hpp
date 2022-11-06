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
    // { a.registry } -> std::convertible_to<entt::registry>;
    { a.tiles_allocator } -> std::convertible_to<IsAllocator>;
    { a.pallette_allocator } -> std::convertible_to<IsAllocator>;
    {
        a.collision_events_container
        } -> std::convertible_to<IsCollisionEventContainer>;
};

void fix_sprite(Object& obj, const Teams team) {
    obj.x_flip = team == Teams::BLUE;
}

template <IsUnitCreationSet UCS>
entt::entity create_base_unit(UCS& ucs, int start_x, int start_y) {
    entt::registry& registry = ucs.registry;

    const auto& entity = registry.create();

    registry.emplace<Position>(entity, start_x, start_y, 16, 16);
    auto col = Collision{
        .collision_type = CollisionTypes::UNIT,
        .collision_events_key = ucs.collision_events_container.get_next_key()};
    registry.emplace<Collision>(entity, col);
    registry.emplace<Velocity>(entity, 0., 0);
    registry.emplace<VisibleOnlyInCam>(entity);

    return entity;
}

template <IsUnitCreationSet UCS>
void create_front_line_unit(UCS& ucs, Teams team, int start_x, int start_y) {
    entt::registry& registry = ucs.registry;

    const auto& entity = create_base_unit(ucs, start_x, start_y);

    auto& obj = registry.emplace<Object>(entity);

    obj.id = ucs.object_allocator.allocate(1);
    obj.size = ATTR1_SIZE_16x16;

    obj.tile_offset = ucs.tiles_allocator.allocate(
        get_tile_count_4bpp(GfxMarchGameUnitsFrontLineManTilesLen));
    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsFrontLineManTiles);

    fix_sprite(obj, team);

    registry.emplace<Unit>(
        entity, UnitTypes::FRONT_LINE_MAN, team, 2.5f, 100,
        Unit::MeleeDumb{.state = Unit::MeleeDumb::State::JUST_SPAWNED,
                        .damage = 10,
                        .attacking = std::nullopt,
                        .attack_cooldown = 60,
                        .attack_cooldown_remaining = 0});
}

template <IsUnitCreationSet UCS>
void create_light_front_line_man_unit(UCS& ucs, Teams team, int start_x,
                                      int start_y) {
    entt::registry& registry = ucs.registry;

    const auto& entity = create_base_unit(ucs, start_x, start_y);

    auto& obj = registry.emplace<Object>(entity);

    obj.id = ucs.object_allocator.allocate(1);
    obj.size = ATTR1_SIZE_16x16;

    obj.tile_offset = ucs.tiles_allocator.allocate(
        get_tile_count_4bpp(GfxMarchGameUnitsLightFrontLineManTilesLen));
    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsLightFrontLineManTiles);

    fix_sprite(obj, team);

    registry.emplace<Unit>(
        entity, UnitTypes::LIGHT_FRONT_LINE_MAN, team, 2.5f, 100,
        Unit::MeleeDumb{.state = Unit::MeleeDumb::State::JUST_SPAWNED,
                        .damage = 10,
                        .attacking = std::nullopt,
                        .attack_cooldown = 60,
                        .attack_cooldown_remaining = 0});
}

template <IsUnitCreationSet UCS>
void create_ranged_man_unit(UCS& ucs, Teams team, int start_x, int start_y) {
    entt::registry& registry = ucs.registry;

    const auto& entity = create_base_unit(ucs, start_x, start_y);

    auto& obj = registry.emplace<Object>(entity);

    obj.id = ucs.object_allocator.allocate(1);
    obj.size = ATTR1_SIZE_16x16;

    obj.tile_offset = ucs.tiles_allocator.allocate(
        get_tile_count_4bpp(GfxMarchGameUnitsRangedManTilesLen));
    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsRangedManTiles);

    fix_sprite(obj, team);

    registry.emplace<Unit>(
        entity, UnitTypes::RANGED_MAN, team, 2.5f, 100,
        Unit::RangedDumb{.state = Unit::RangedDumb::State::JUST_SPAWNED});
}

}  // namespace mgm::units