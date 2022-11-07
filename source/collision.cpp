#include "collision.hpp"

namespace mgm {

bool point_in_rect(const Position& target, const Position& source_pos) {
    return target.x > source_pos.x &&
           target.x < source_pos.x + source_pos.width &&
           target.y > source_pos.y &&
           target.y < source_pos.y + source_pos.height;
}

bool rect_overlap(const Position& a, const Position& b) {
    bool first = a.x < b.x + b.width;
    bool second = a.x + a.width > b.x;
    bool third = a.y < b.y + b.height;
    bool forth = a.y + a.height > b.y;
    return first && second && third && forth;
}

void add_collision_event(Collision& collision, int event_idx,
                         CollisionEvent event) {
        collision.events[event_idx] = event;
}

void reset_collision_events(Collision& collision) {
    for (u32 i = 0; i < collision.events.size(); i++) {
        collision.events[i] = std::nullopt;
    }
}

}  // namespace mgm