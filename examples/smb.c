#include <unes.h>

void sprite0(uint8_t scanline) {

}

int main(int argc, const char** argv) {
    unes_init();

    uint8_t tile_data[8192];

    FILE* file = fopen("smb.chr", "rb");
    fread(&tile_data[0], sizeof(uint8_t), 8192, file);
    fclose(file);

    unes_set_tile_data(tile_data, 8192);

    unes_set_palette(0, (Palette){0x00, 0x29, 0x1A, 0x0F});
    unes_set_palette(1, (Palette){0x00, 0x36, 0x17, 0x0F});
    unes_set_palette(2, (Palette){0x00, 0x30, 0x21, 0x0F});
    unes_set_palette(3, (Palette){0x00, 0x27, 0x17, 0x0F});
    unes_set_palette(4, (Palette){0x00, 0x16, 0x27, 0x18});
    unes_set_palette(5, (Palette){0x00, 0x1A, 0x30, 0x27});
    unes_set_palette(6, (Palette){0x00, 0x16, 0x30, 0x27});
    unes_set_palette(7, (Palette){0x00, 0x0F, 0x36, 0x17});

    unes_set_sprite0_handler(sprite0);

    while (unes_render()) {
        
    }

    unes_finish();
    return 0;
}
