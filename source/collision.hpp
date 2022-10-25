#pragma once

#include <concepts>
#include <optional>

#include "components.hpp"

namespace mgm {

inline bool point_in_rect(const Position& target, const Position& source_pos) {
    return target.x > source_pos.x &&
           target.x < source_pos.x + source_pos.width &&
           target.y > source_pos.y &&
           target.y < source_pos.y + source_pos.height;
}

inline bool rect_overlap(const Position& a, const Position& b) {
    bool first = a.x < b.x + b.width;
    bool second = a.x + a.width > b.x;
    bool third = a.y < b.y + b.height;
    bool forth = a.y + a.height > b.y;
    return first && second && third && forth;
}

struct CollisionEvent {
    entt::entity entity;
    CollisionTypes collision_type;
};

// template <typename T>
// concept IsCollisionEventContainer = T::EVENT_COUNT;

template <typename T>
concept IsCollisionEventContainer = requires(T a, u32 id, u32 event_idx,
                                             CollisionEvent event) {
    a.clear();
    { a.get_next_idx() } -> std::convertible_to<u32>;
    a.add_collision_event(id, event_idx, event);
    {
        a.get_collision_event(id, event_idx)
        } -> std::convertible_to<
            std::optional<std::reference_wrapper<const CollisionEvent>>>;
    T::EVENT_COUNT;
};

template <int TOTAL_COUNT, int T_EVENT_COUNT>
class CollisionEventContainer {
   public:
    using CollisionEvents =
        std::array<std::optional<CollisionEvent>, T_EVENT_COUNT>;

   private:
    u32 m_top_idx;
    std::array<CollisionEvents, TOTAL_COUNT> m_collision_events;

   public:
    static const u32 EVENT_COUNT = T_EVENT_COUNT;

    CollisionEventContainer() { clear(); }

    void clear() {
        for (u32 i = 0; i <= m_top_idx; i++) {
            auto& events = m_collision_events[i];
            for (u32 j = 0; j < T_EVENT_COUNT && events[j].has_value(); j++) {
                events[j] = std::nullopt;
            }
        }
        m_top_idx = 0;
    }

    u32 get_next_idx() {
        auto result = m_top_idx;
        m_top_idx++;
        return result;
    }

    std::optional<std::reference_wrapper<const CollisionEvent>>
    get_collision_event(u32 id, u32 event_idx) const {
        return m_collision_events[id][event_idx];
    }

    void add_collision_event(u32 id, u32 event_idx, CollisionEvent event) {
        m_collision_events[id][event_idx] = event;
    }
};

}  // namespace mgm