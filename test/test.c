#include "gfx.h"
#include <stdio.h>

#include <unes.h>

uint8_t tile_data[] = {
    0b00011000,
    0b00100100,
    0b01000010,
    0b10011001,
    0b10011001,
    0b01000010,
    0b00100100,
    0b00011000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000,
};

int main(int argc, char** argv) {
    unes_init();
    unes_set_tile_data(tile_data, sizeof(tile_data));
    unes_set_palette(0, (Palette){1, 2, 3, 4});
    
    while (unes_render()) {
        
    }

    unes_finish();
}
