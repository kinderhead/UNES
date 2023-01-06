#include "gfx.h"
#include <stdio.h>

#include <unes.h>

const uint8_t tile_data[] = {
    0b01010101,0b00010101,
    0b11010101,0b00010100,
    0b01010101,0b00010101,
    0b11010101,0b00010100,
    0b01010101,0b00010101,
    0b11010101,0b00010100,
    0b01010101,0b00010101,
    0b11010101,0b00010100,
};

int main(int argc, char** argv) {
    unes_init();
    
    while (unes_render()) {
        
    }

    unes_finish();
}
