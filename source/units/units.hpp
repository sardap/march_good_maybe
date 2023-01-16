#pragma once

#include <gfx/GfxMarchGameUnitsFrontLineMan.h>
#include <gfx/GfxMarchGameUnitsLightFrontLineMan.h>
#include <gfx/GfxMarchGameUnitsRangedMan.h>
#include <gfx/MarchGameUnitsShared.h>
#include <tonc.h>

#include "../components.hpp"
#include "../object.hpp"

namespace mgm::units {

// https://stackoverflow.com/questions/57751484/are-types-allowed-to-be-specified-for-return-type-requirement-in-a-requires-expr
// https://stackoverflow.com/questions/69686934/concept-checking-on-struct-members
template <typename T>
concept IsUnitCreationSet = requires(T a) {
    { a.tiles_allocator } -> std::convertible_to<IsAllocator>;
    { a.pallette_allocator } -> std::convertible_to<IsAllocator>;
};

void fix_sprite(Object& obj, const Teams team) {
    obj.x_flip = team == Teams::BLUE;
}

template <IsUnitCreationSet UCS>
ecs::EntityID create_base_unit(UCS& ucs, int start_x, int start_y) {
    const auto& entity = ucs.registry.create();

    ucs.registry.template emplace<Position>(
        entity,
        Position{.x = start_x, .y = start_y, .width = 16, .height = 16});
    ucs.registry.template emplace<Collision>(
        entity,
        Collision{.collision_type = CollisionTypes::UNIT, .events = {}});
    ucs.registry.template emplace<Velocity>(entity, Velocity{.dx = 0, .dy = 0});
    ucs.registry.template emplace<VisibleOnlyInCam>(entity, VisibleOnlyInCam{});

    return entity;
}

template <IsUnitCreationSet UCS>
void create_front_line_unit(UCS& ucs, Teams team, int start_x, int start_y) {
    const auto& entity = create_base_unit(ucs, start_x, start_y);

    Object& obj = ucs.registry.template emplace<Object>(
        entity,
        Object{.id = ucs.object_allocator.allocate(1),
               .size = ATTR1_SIZE_16x16,
               .tile_offset = ucs.tiles_allocator.allocate(get_tile_count_4bpp(
                   GfxMarchGameUnitsFrontLineManTilesLen))});

    GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
             GfxMarchGameUnitsFrontLineManTiles);

    fix_sprite(obj, team);

    ucs.registry.template emplace<Unit>(
        entity, Unit{.type = UnitTypes::FRONT_LINE_MAN,
                     .team = team,
                     .speed = 2.5f,
                     .health = 100,
                     .specific = Unit::MeleeDumb{
                         .state = Unit::MeleeDumb::State::JUST_SPAWNED,
                         .damage = 10,
                         .attacking = std::nullopt,
                         .attack_cooldown = 60,
                         .attack_cooldown_remaining = 0}});
}

// template <IsUnitCreationSet UCS>
// void create_light_front_line_man_unit(UCS& ucs, Teams team, int start_x,
//                                       int start_y) {
//     const auto& entity = create_base_unit(ucs, start_x, start_y);

//     Object& obj = ucs.registry.template emplace<Object>(
//         entity,
//         Object{.id = ucs.object_allocator.allocate(1),
//                .size = ATTR1_SIZE_16x16,
//                .tile_offset =
//                ucs.tiles_allocator.allocate(get_tile_count_4bpp(
//                    GfxMarchGameUnitsLightFrontLineManTilesLen))});

//     GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
//              GfxMarchGameUnitsLightFrontLineManTiles);

//     fix_sprite(obj, team);

//     ucs.registry.template emplace<Unit>(
//         entity, Unit{.type = UnitTypes::LIGHT_FRONT_LINE_MAN,
//                      .team = team,
//                      .speed = 2.5f,
//                      .health = 100,
//                      .specific = Unit::MeleeDumb{
//                          .state = Unit::MeleeDumb::State::JUST_SPAWNED,
//                          .damage = 10,
//                          .attacking = std::nullopt,
//                          .attack_cooldown = 60,
//                          .attack_cooldown_remaining = 0}});
// }

// template <IsUnitCreationSet UCS>
// void create_ranged_man_unit(UCS& ucs, Teams team, int start_x, int start_y) {
//     const auto& entity = create_base_unit(ucs, start_x, start_y);

//     Object& obj = ucs.registry.template emplace<Object>(
//         entity,
//         Object{.id = ucs.object_allocator.allocate(1),
//                .size = ATTR1_SIZE_16x16,
//                .tile_offset = ucs.tiles_allocator.allocate(
//                    get_tile_count_4bpp(GfxMarchGameUnitsRangedManTilesLen))});

//     GRIT_CPY(&tile_mem_obj[0][obj.tile_offset],
//              GfxMarchGameUnitsRangedManTiles);

//     fix_sprite(obj, team);

//     ucs.registry.template emplace<Unit>(
//         entity, Unit{.type = UnitTypes::RANGED_MAN,
//                      .team = team,
//                      .speed = 2.5f,
//                      .health = 100,
//                      .specific = Unit::RangedDumb{
//                          .state = Unit::RangedDumb::State::JUST_SPAWNED}});
// }

}  // namespace mgm::units