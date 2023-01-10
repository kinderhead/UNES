#include <stdio.h>

#include <unes.h>

int main(int argc, char** argv) {
    unes_init();
    unes_ppu_enable();
    unes_apu_enable();
    
    uint8_t tile_data[8192];

    FILE* file = fopen("smb.chr", "rb");
    fread(&tile_data[0], sizeof(uint8_t), 8192, file);
    fclose(file);

    unes_set_tile_data(tile_data, 8192);

    unes_fill_bg_alt(256 + 0x24, 0);

    unes_set_bg_tile_alt(50, 0, 3, 3);
    unes_set_bg_tile_alt(51, 0, 4, 3);
    unes_set_bg_tile_alt(66, 0, 3, 4);
    unes_set_bg_tile_alt(67, 0, 4, 4);

    unes_set_background_color(0x22);
    unes_set_palette(0, (Palette){0x00, 0x29, 0x1A, 0x0F});
    unes_set_palette(1, (Palette){0x00, 0x36, 0x17, 0x0F});
    unes_set_palette(2, (Palette){0x00, 0x30, 0x21, 0x0F});
    unes_set_palette(3, (Palette){0x00, 0x27, 0x17, 0x0F});

    //unes_fill_bg_alt(256 + 0x24, 0);
    
    float freq = 0;
    while (unes_render()) {
        unes_set_pulse_data(SQUARE1, freq, 3000, DUTY_50);
        freq += 1;
    }

    unes_finish();
}
