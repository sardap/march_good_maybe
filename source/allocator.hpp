#pragma once

#include <bitset>

namespace mgm {

template <typename T>
concept IsAllocator = requires(T a, int offset, int count) {
    { a.allocate(count) } -> std::convertible_to<int>;
    a.free(offset, count);
};

template <int SIZE>
class Allocator {
   private:
    std::bitset<SIZE> m_tiles;

   public:
    Allocator() {
        for (int i = 0; i < SIZE; i++) {
            m_tiles[i] = false;
        }
    }

    int allocate(int count) {
        int offset = 0;
        for (int i = 0; i < SIZE - count; i++) {
            if (m_tiles[i]) {
                offset = i;
            }

            if (i - offset == count) {
                break;
            }
        }

        for (int i = offset; i <= offset + count; i++) {
            m_tiles[i] = true;
        }

        return offset;
    }

    void free(int offset, int count) {
        for (int i = offset; i < offset + count; i++) {
            m_tiles[i] = false;
        }
    }
};

}  // namespace mgm