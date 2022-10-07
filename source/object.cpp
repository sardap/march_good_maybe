#include "object.hpp"

#include <gfx/GfxMarchGameUnitsRedSquare.h>
#include <gfx/GfxMarchGameUnitsYellowSquare.h>
#include <tonc.h>

namespace mgm {

static int top_id = 0;

OBJ_ATTR g_oam[OAM_LENGTH] = {};

void create_red_square_unit(Object& obj) {
    obj.id = top_id;
    obj.size = ATTR1_SIZE_16x16;
    obj.tile_offset = 0;
    GRIT_CPY(&tile_mem_obj[0][0], GfxMarchGameUnitsRedSquareTiles);
    GRIT_CPY(pal_obj_mem, GfxMarchGameUnitsRedSquarePal);
    top_id++;
}

}  // namespace mgm
