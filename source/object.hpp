#pragma once

#include <tonc_types.h>

#include "components.hpp"

namespace mgm {

#define OAM_LENGTH 128
extern OBJ_ATTR g_oam[OAM_LENGTH];

void create_red_square_unit(Object& obj);

}  // namespace mgm