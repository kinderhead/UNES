#include <stdio.h>

#include <unes.h>

int main(int argc, char** argv) {
    unes_init();
    
    uint8_t* tile_data[8192];

    FILE* file = fopen("smb.chr", "rb");
    fread(tile_data, sizeof(uint8_t), 8192, file);
    fclose(file);

    unes_set_tile_data(tile_data, 8192);

    unes_set_background_color(0x22);
    unes_set_palette(0, (Palette){0x00, 0x29, 0x1A, 0x0F});
    unes_set_palette(1, (Palette){0x00, 0x36, 0x17, 0x0F});
    unes_set_palette(2, (Palette){0x00, 0x30, 0x21, 0x0F});
    unes_set_palette(3, (Palette){0x00, 0x27, 0x17, 0x0F});

    unes_fill_bg_alt(256 + 0x24, 0);
    
    while (unes_render()) {
        
    }

    unes_finish();
}