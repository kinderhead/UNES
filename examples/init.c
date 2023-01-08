#include <unes.h>

int main(int argc, char** argv) {
    unes_init();
    unes_ppu_enable();

    // Initialize tile data and palettes
    // unes_set_tile_data(tile_data, sizeof(tile_data));
    // unes_set_palette(0, (Palette){0x00, 0x01, 0x02, 0x03});

    while (unes_render()) {
        // Game code here
    }

    unes_finish();
}
