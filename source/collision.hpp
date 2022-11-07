#pragma once

#include <array>
#include <concepts>
#include <optional>

#include "components.hpp"

namespace mgm {

bool point_in_rect(const Position& target, const Position& source_pos);

bool rect_overlap(const Position& a, const Position& b);

void add_collision_event(Collision& collision, int event_idx,
                         CollisionEvent event);

void reset_collision_events(Collision& collision);

}  // namespace mgm