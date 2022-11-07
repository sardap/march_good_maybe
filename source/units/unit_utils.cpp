#include "unit_utils.hpp"

namespace mgm::units {

static constexpr int UNIT_MAX_HEALTH = 200;

void apply_damage(Unit& unit, int damage) {
    unit.health = clamp(unit.health - damage, 0, UNIT_MAX_HEALTH);
}

}  // namespace mgm::units