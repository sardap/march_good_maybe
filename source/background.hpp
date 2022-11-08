#pragma once

#include <tonc.h>

namespace mgm {

enum class BackgroundType {
    REG_32x32,
    REG_64x32,
    REG_32x64,
    REG_64x64,
    AFF_16x16,
    AFF_32x32,
    AFF_64x64,
    AFF_128x128
};

inline int background_type_to_int(BackgroundType t) {
    switch (t) {
        case BackgroundType::REG_32x32:
            return BG_REG_32x32;
        case BackgroundType::REG_64x32:
            return BG_REG_64x32;
        case BackgroundType::REG_32x64:
            return BG_REG_32x64;
        case BackgroundType::REG_64x64:
            return BG_REG_64x64;
        case BackgroundType::AFF_16x16:
            return BG_AFF_16x16;
        case BackgroundType::AFF_32x32:
            return BG_AFF_32x32;
        case BackgroundType::AFF_64x64:
            return BG_AFF_64x64;
        case BackgroundType::AFF_128x128:
            return BG_AFF_128x128;
    }
    return BG_REG_32x32;
}

enum class Background { ZERO, ONE, TWO, THREE };

inline vu16* background_to_reg(Background n) {
    switch (n) {
        case Background::ZERO:
            return &REG_BG0CNT;
        case Background::ONE:
            return &REG_BG1CNT;
        case Background::TWO:
            return &REG_BG2CNT;
        case Background::THREE:
            return &REG_BG3CNT;
    }

    return &REG_BG0CNT;
}

inline vu16* background_to_v_reg(Background n) {
    switch (n) {
        case Background::ZERO:
            return &REG_BG0VOFS;
        case Background::ONE:
            return &REG_BG1VOFS;
        case Background::TWO:
            return &REG_BG2VOFS;
        case Background::THREE:
            return &REG_BG3VOFS;
    }

    return &REG_BG0VOFS;
}

template <typename T_PRIORITY, typename T_CBB, typename T_SBB>
void init_background(Background number, T_PRIORITY priority, T_CBB cbb,
                     T_SBB sbb, BackgroundType type) {
    auto reg = background_to_reg(number);
    int bg_reg = background_type_to_int(type);

    *reg = static_cast<u16>(BG_PRIO(static_cast<int>(priority)) | BG_4BPP |
                            BG_CBB(static_cast<int>(cbb)) |
                            BG_SBB(static_cast<int>(sbb)) | bg_reg);
}

inline void set_background_x(Background number, int x) {
    switch (number) {
        case Background::ZERO:
            REG_BG0HOFS = x;
            break;
        case Background::ONE:
            REG_BG1HOFS = x;
            break;
        case Background::TWO:
            REG_BG2HOFS = x;
            break;
        case Background::THREE:
            REG_BG3HOFS = x;
            break;
    }
}

inline void set_background_y(Background number, int y) {
    auto reg = background_to_v_reg(number);
    *reg = static_cast<u16>(y);
}

inline void set_background_scrolling(Background number, int x, int y) {
    set_background_x(number, x);
    set_background_y(number, y);
}

}  // namespace mgm