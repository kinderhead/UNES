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

    unes_set_background_color(0x22);
    unes_set_palette(0, (Palette){0x00, 0x29, 0x1A, 0x0F});
    unes_set_palette(1, (Palette){0x00, 0x36, 0x17, 0x0F});
    unes_set_palette(2, (Palette){0x00, 0x30, 0x21, 0x0F});
    unes_set_palette(3, (Palette){0x00, 0x27, 0x17, 0x0F});
    unes_set_palette(4, (Palette){0x00, 0x16, 0x27, 0x18});
    unes_set_palette(5, (Palette){0x00, 0x1A, 0x30, 0x27});
    unes_set_palette(6, (Palette){0x00, 0x16, 0x30, 0x27});
    unes_set_palette(7, (Palette){0x00, 0x0F, 0x36, 0x17});

    Sprite* top_left = unes_get_sprite(1);
    Sprite* top_right = unes_get_sprite(2);
    Sprite* bottom_left = unes_get_sprite(3);
    Sprite* bottom_right = unes_get_sprite(4);

    uint8_t x = 8;
    uint8_t y = 8;

    top_left->tile = 0x3A;
    top_left->enabled = true;
    top_right->tile = 0x37;
    top_right->enabled = true;
    bottom_left->tile = 0x4F;
    bottom_left->enabled = true;
    bottom_right->tile = 0x4F;
    bottom_right->enabled = true;
    bottom_right->h_flip = true;

    unes_set_bg_tile_alt(256 + 0x45, 1, 1, 1);
    unes_set_bg_tile_alt(256 + 0x45, 1, 2, 1);
    unes_set_bg_tile_alt(256 + 0x47, 1, 1, 2);
    unes_set_bg_tile_alt(256 + 0x47, 1, 2, 2);
    
    while (unes_render()) {
        if (unes_is_pressed(BUTTONS_DOWN)) y += 8;
        if (unes_is_pressed(BUTTONS_UP)) y -= 8;
        if (unes_is_pressed(BUTTONS_RIGHT)) x += 8;
        if (unes_is_pressed(BUTTONS_LEFT)) x -= 8;

        top_left->x = x;
        top_left->y = y;
        top_right->x = x+8;
        top_right->y = y;
        bottom_left->x = x;
        bottom_left->y = y+8;
        bottom_right->x = x+8;
        bottom_right->y = y+8;
    }

    unes_finish();
    return 0;
}
