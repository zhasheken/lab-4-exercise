#ifndef P2A_H
#define P2A_H
#include <cassert>  // assert
#include <cstdint>  // uint64_t

inline uint64_t expand(uint64_t input, uint32_t scale) {
    assert(scale >= 1);
    uint64_t result = 0;
    for (uint64_t i = 0; i < 64; i++) {
        uint64_t insert_at = i * scale;
        if (insert_at >= 64) {
            break;
        }
        if ((input >> i) & 1) {  // bitshift down and mask so that i is the only bit left
            // if bit at i is set
            // take the bit, insert it at its destination after the additional 0s
            result |= (1ull << insert_at);
        }
    }
    return result;
}

inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z){
    // expand each coordinate to 3x its size and insert the set bits into the correct positions for a 3D Morton code
    uint64_t result_x = expand(x, 3);
    uint64_t result_y = expand(y, 3);
    uint64_t result_z = expand(z, 3);

    // combine the expanded coordinates, offset to ensure they dont overlap
    return result_x | (result_y << 1) | (result_z << 2);
}
#endif  // P2A_H