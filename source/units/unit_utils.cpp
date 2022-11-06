#include "unit_utils.hpp"

namespace mgm::units {

void apply_damage(Unit& unit, int damage) { unit.health -= damage; }

}  // namespace mgm::units