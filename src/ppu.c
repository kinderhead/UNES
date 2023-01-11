#include "ppu.h"
#include "input.h"
#include <stdlib.h>

#define RAW_BG_COLOR _unes_get_raw_color(ppu->universal_bg_color.r, ppu->universal_bg_color.g, ppu->universal_bg_color.b)

static _UNES_PPU* ppu;
static SDL_PixelFormat* pixel_format;

void _UNES_PPU_init() {
    SDL_Init(SDL_INIT_EVERYTHING);

    ppu = malloc(sizeof(_UNES_PPU));
    memset(ppu, 0, sizeof(_UNES_PPU));
    ppu->tile_data = NULL;
    ppu->ppu_enabled = false;
    CHECK_NULL(ppu->window, SDL_CreateWindow("UNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
    CHECK_NULL(ppu->renderer, SDL_CreateRenderer(ppu->window, -1, SDL_RENDERER_ACCELERATED));
    CHECK_NULL(ppu->tex, SDL_CreateTexture(ppu->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT));
    CHECK_NULL(pixel_format, SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888));

    // Hide sprites
    for (int i = 0; i < SPRITE_COUNT; i++)
    {
        ppu->oam[i].enabled = false;
    }

    SDL_RenderSetLogicalSize(ppu->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_GL_SetSwapInterval(0);

    ppu->fps_start = SDL_GetPerformanceCounter();
}

void _UNES_PPU_free()
{
    SDL_DestroyTexture(ppu->tex);
    SDL_DestroyRenderer(ppu->renderer);
    SDL_DestroyWindow(ppu->window);
    SDL_FreeFormat(pixel_format);

    free(ppu);

    SDL_Quit();
}

void unes_ppu_enable()
{
    ppu->ppu_enabled = true;
}

void unes_ppu_disable()
{
    ppu->ppu_enabled = false;
}

void unes_set_sprite_size(SPRITE_SIZE size)
{
    ppu->sprite_size = size;
}

inline static uint32_t _unes_get_raw_color(int r, int g, int b) {
    // Don't question it
    return SDL_MapRGBA(pixel_format, r, b, g, 255);
}

inline static bool _unes_valid_tile(size_t index) {
    return (index * SIZEOF_TILE) < ppu->tile_data_size;
}

inline static bool _unes_valid_sprite(uint16_t index) {
    return index < SPRITE_COUNT;
}

inline static _UNES_ATTR _unes_parse_attr(uint8_t attr) {
    _UNES_ATTR ret =  {
        (attr >> 0) & 0x3,
        (attr >> 2) & 0x3,
        (attr >> 4) & 0x3,
        (attr >> 6) & 0x3,
    };
    return ret;
}

inline static void _unes_render_row(uint32_t* out, uint16_t index, Palette palette, uint8_t row, uint32_t bg_color) {
    uint8_t* raw_tile = unes_get_tile_data(index);
    for (int x = 7; x >= 0; x--)
    {        
        uint8_t index = ((raw_tile[row]>>x)&1) | (((raw_tile[row+8]>>x)&1)<<1);
        uint32_t raw_color = 0;
        if (index == 0) {
            raw_color = bg_color;
        } else {
            Color color = DEFAULT_PALETTE[palette[index]];
            raw_color = _unes_get_raw_color(color.r, color.g, color.b);
        }

        // So it copies nicely into the final buffer
        out[7-x] = raw_color;
    }
}

inline static void _unes_render_sprite(uint32_t out[8][8], Sprite sprite) {
    for (int y = 0; y < 8; y++)
    {
        _unes_render_row(&out[y][0], sprite.tile, ppu->palettes[sprite.palette+PALETTE_COUNT], y, 0);
    }
}

void unes_get_bounding_box(uint16_t tile, uint8_t* l, uint8_t* r, uint8_t* u, uint8_t* d) {
    uint8_t* data = unes_get_tile_data(tile);
    
    uint8_t bound_l = 0;
    uint8_t bound_r = 7;
    uint8_t bound_u = 0;
    uint8_t bound_d = 7;

    for (int i = 0; i < 8; i++)
    {
        uint8_t index = ((data[i]>>bound_l)&1) | (((data[i+8]>>bound_l)&1)<<1);
        if (index != 0) {
            break;
        }
        bound_l++;
    }

    for (int i = 7; i >= 0; i--)
    {
        uint8_t index = ((data[i]>>bound_r)&1) | (((data[i+8]>>bound_r)&1)<<1);
        if (index != 0) {
            break;
        }
        bound_r--;
    }
    
    for (int i = 0; i < 8; i++)
    {
        uint8_t index = ((data[bound_u]>>i)&1) | (((data[bound_u+8]>>i)&1)<<1);
        if (index != 0) {
            break;
        }
        bound_u++;
    }

    for (int i = 7; i >= 0; i--)
    {
        uint8_t index = ((data[bound_d]>>i)&1) | (((data[bound_d+8]>>i)&1)<<1);
        if (index != 0) {
            break;
        }
        bound_d--;
    }

    *l = bound_l;
    *r = bound_r;
    *u = bound_u;
    *d = bound_d;
}

inline static int _unes_h_flip(uint16_t tile) {
    uint8_t bound_l = 0;
    uint8_t bound_r = 0;
    uint8_t bound_u = 0;
    uint8_t bound_d = 0;
    unes_get_bounding_box(tile, &bound_l, &bound_r, &bound_u, &bound_d);
    return (int)bound_l - (7-(int)bound_u);
}

inline static int _unes_v_flip(uint16_t tile) {
    uint8_t bound_l = 0;
    uint8_t bound_r = 0;
    uint8_t bound_u = 0;
    uint8_t bound_d = 0;
    unes_get_bounding_box(tile, &bound_l, &bound_r, &bound_u, &bound_d);
    return (int)bound_u - (7-(int)bound_d);
}

inline static void _unes_render_sprite_to_raw_screen(const Sprite sprite) {
    uint32_t data[8][8];
    _unes_render_sprite(data, sprite);

    // TODO: Clean this up
    if (sprite.v_flip) {
        int y = 0;
        int vcoef = 0; //_unes_v_flip(sprite.tile);
        for (int i = 7; i >= 0; i--)
        {
            if (sprite.h_flip) {
                int x = 0;
                for (int e = 7; e >= 0; e--)
                {
                    if (y+sprite.y < SCREEN_HEIGHT && x+sprite.x < SCREEN_WIDTH) {
                        uint32_t pixel = data[i][e];
                        if (pixel == 0) {x++; continue;}
                        if (sprite.priority && ppu->temp_raw_screen[y+sprite.y][x+sprite.x] != RAW_BG_COLOR) {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = ppu->temp_raw_screen[y+sprite.y][x+sprite.x];
                        } else {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = pixel;
                        }
                    }
                    x++;
                }
            } else {
                for (int x = 0; x < 8; x++)
                {
                    if (y+sprite.y < SCREEN_HEIGHT && x+sprite.x < SCREEN_WIDTH) {
                        uint32_t pixel = data[i][x];
                        if (pixel == 0) continue;
                        if (sprite.priority && ppu->temp_raw_screen[y+sprite.y][x+sprite.x] != RAW_BG_COLOR) {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = ppu->temp_raw_screen[y+sprite.y][x+sprite.x];
                        } else {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = pixel;
                        }
                    }
                }
            }
            y++;
        }
    } else {
        for (int y = 7; y >= 0; y--)
        {
            if (sprite.h_flip) {
                int x = 0;
                for (int e = 7; e >= 0; e--)
                {
                    if (y+sprite.y < SCREEN_HEIGHT && x+sprite.x < SCREEN_WIDTH) {
                        uint32_t pixel = data[y][e];
                        if (pixel == 0) {x++; continue;}
                        if (sprite.priority && ppu->temp_raw_screen[y+sprite.y][x+sprite.x] != RAW_BG_COLOR) {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = ppu->temp_raw_screen[y+sprite.y][x+sprite.x];
                        } else {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = pixel;
                        }
                    }
                    x++;
                }
            } else {
                for (int x = 0; x < 8; x++)
                {
                    if (y+sprite.y < SCREEN_HEIGHT && x+sprite.x < SCREEN_WIDTH) {
                        uint32_t pixel = data[y][x];
                        if (pixel == 0) continue;
                        if (sprite.priority && ppu->temp_raw_screen[y+sprite.y][x+sprite.x] != RAW_BG_COLOR) {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = ppu->temp_raw_screen[y+sprite.y][x+sprite.x];
                        } else {
                            ppu->raw_screen[y+sprite.y][x+sprite.x] = pixel;
                        }
                    }
                }
            }
        }
    }
}

inline static uint8_t _unes_sprite0_scanline() {
    uint8_t* raw_tile = unes_get_tile_data(ppu->oam[0].tile);
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            uint8_t index = ((raw_tile[y]>>x)&1) | (((raw_tile[y+8]>>x)&1)<<1);
            if (index != 0) {
                return ppu->oam[0].y + y;
            }
        }
    }
    return 0;
}

void unes_set_scroll(int scrollx, int scrolly) {
    ppu->scrollx = scrollx % (TOTAL_BACKGROUND_WIDTH*8);
    ppu->scrolly = scrolly % (TOTAL_BACKGROUND_HEIGHT*8);
}

void unes_get_scroll(uint16_t* scrollx, uint16_t* scrolly)
{
    if (scrollx != NULL) {
        *scrollx = ppu->scrollx;
    }
    if (scrolly != NULL) {
        *scrolly = ppu->scrolly;
    }
}

void unes_set_tile_data(uint8_t* data, size_t size) {
    ppu->tile_data = data;
    ppu->tile_data_size = size;
}

void unes_set_palettes(uint8_t start, uint8_t* palettes, size_t num) {
    if (num + start >= PALETTE_COUNT*2) return;
    memcpy(&ppu->palettes[start], palettes, sizeof(Palette) * num);
}

void unes_set_scanline_interrupt(unes_scanline_interrupt irq) {
    ppu->scanline_irq = irq;
}

void unes_set_scanline_interrupt_counter(uint8_t counter) {
    ppu->scanline_irq_counter = counter;
}

void unes_set_sprite0_handler(unes_sprite0_hit hit)
{
    ppu->sprite0_hit = hit;
}

uint8_t* unes_get_tile_data(size_t index) {
    if (!_unes_valid_tile(index)) return NULL;
    return &ppu->tile_data[index * SIZEOF_TILE];
}

Tile* unes_get_bg_tile(uint8_t x, uint8_t y) {
    if (x >= TOTAL_BACKGROUND_WIDTH || y >= TOTAL_BACKGROUND_HEIGHT) return NULL;
    return &ppu->nametables[x][y];
}

void unes_set_bg_tile(Tile tile, uint8_t x, uint8_t y) {
    ppu->nametables[x][y] = tile;
}

void unes_set_bg_tile_index(uint16_t index, uint8_t x, uint8_t y)
{
    ppu->nametables[x][y].tile = index;
}

void unes_set_bg_tile_palette(uint8_t index, uint8_t x, uint8_t y)
{
    ppu->nametables[x][y].palette = index;
}

void unes_fill_bg(Tile tile)
{
    for (int x = 0; x < TOTAL_BACKGROUND_WIDTH; x++)
    {
        for (int y = 0; y < TOTAL_BACKGROUND_HEIGHT; y++)
        {
            ppu->nametables[x][y] = tile;
        }
    }
}

void unes_legacy_set_map(uint8_t *data, size_t size, uint8_t x_offset, uint8_t y_offset, uint8_t row_size)
{
    int y = y_offset;
    for (int i = 0; i < size; i++)
    {
        if (i%row_size == 0) y++;
        if (i >= TOTAL_BACKGROUND_WIDTH || y >= TOTAL_BACKGROUND_HEIGHT) printf("Invalid location %d, %d\n", i, y);
        // Add 256 because tiles use the second block of 256 tiles in the NES
        ppu->nametables[i][y].tile = data[i]+256;
    }
}

void unes_legacy_set_nametable(uint8_t *data, uint8_t x_offset, uint8_t y_offset)
{
    unes_legacy_set_map(data, 960, x_offset, y_offset, 32);
    uint8_t* attributes = &data[960];

    int x = x_offset;
    int y = y_offset;
    for (int i = 0; i < 64; i++)
    {
        if (i%8 == 0) {
            x = x_offset;
            y += 2;
        } else {
            x += 2;
        }
        if (x >= TOTAL_BACKGROUND_WIDTH || y >= TOTAL_BACKGROUND_HEIGHT) printf("Invalid location %d, %d\n", x, y);

        _UNES_ATTR attr = _unes_parse_attr(attributes[i]);
        ppu->nametables[x][y].palette = attr.top_left;
        ppu->nametables[x + 1][y].palette = attr.top_right;
        if (y + 1 < TOTAL_BACKGROUND_HEIGHT) {
            ppu->nametables[x][y + 1].palette = attr.bottom_left;
            ppu->nametables[x + 1][y + 1].palette = attr.bottom_right;
        }
    }
}

Sprite *unes_get_sprite(uint16_t index)
{
    if (!_unes_valid_sprite(index)) return NULL;
    return &ppu->oam[index];
}

void unes_set_background_color(uint8_t index)
{
    ppu->universal_bg_color = DEFAULT_PALETTE[index];
}

bool unes_render()
{
    SDL_RenderClear(ppu->renderer);
    if (ppu->tile_data != NULL && ppu->ppu_enabled) {
        memset(ppu->raw_screen, 0, sizeof(ppu->raw_screen));
        
        uint8_t sprite0 = _unes_sprite0_scanline();
        uint32_t row[SCREEN_WIDTH+8];
        uint32_t interim_tile[8];
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            memset(row, 0, SCREEN_WIDTH+8);

            for (int i = 0; i < NAMETABLE_WIDTH+1; i++)
            {
                Tile tile = ppu->nametables[(i + (ppu->scrollx/8))%TOTAL_BACKGROUND_WIDTH][((y + ppu->scrolly)/8)%TOTAL_BACKGROUND_HEIGHT];
                if (tile.palette >= PALETTE_COUNT/2) {printf("Invalid palette %d\n", (int)tile.palette); continue;}
                _unes_render_row(&interim_tile[0], tile.tile, ppu->palettes[tile.palette], (y + ppu->scrolly) % 8, _unes_get_raw_color(ppu->universal_bg_color.r, ppu->universal_bg_color.g, ppu->universal_bg_color.b));
                memcpy(&row[i*8], interim_tile, sizeof(interim_tile));
            }
            // Basically H-blank here
            memcpy(&ppu->raw_screen[y], &row[ppu->scrollx%8], SCREEN_WIDTH*sizeof(uint32_t));

            if (ppu->scanline_irq_counter != 0) {
                ppu->scanline_irq_counter--;
                if (ppu->scanline_irq_counter == 0) {
                    (*ppu->scanline_irq)(y);
                }
            }

            if (y == sprite0 && ppu->sprite0_hit != NULL) {
                (*ppu->sprite0_hit)(y);
            }
        }

        memcpy(ppu->temp_raw_screen, ppu->raw_screen, sizeof(ppu->raw_screen));

        for (int s = SPRITE_COUNT-1; s >= 0; s--)
        {
            Sprite sprite = ppu->oam[s];
            if (sprite.enabled) {
                _unes_render_sprite_to_raw_screen(sprite);
            }
        }

        void* pixels = NULL;
        int pitch = 0;
        SDL_LockTexture(ppu->tex, NULL, &pixels, &pitch);
        
        if (pixels == NULL) {
            printf("%s\n", SDL_GetError());
            exit(1);
        }

        memcpy(pixels, ppu->raw_screen, sizeof(ppu->raw_screen));
        SDL_UnlockTexture(ppu->tex);
    }

    SDL_RenderCopy(ppu->renderer, ppu->tex, NULL, NULL);
    SDL_RenderPresent(ppu->renderer);

    ppu->fps_end = SDL_GetPerformanceCounter();

    double elapsed = (ppu->fps_end - ppu->fps_start) / (double) SDL_GetPerformanceFrequency() * 1000;
    if (16.66666 - elapsed > 0) {
        SDL_Delay(floor(16.66666 - elapsed));
    } else {
        printf("WARNING: Lagging, FPS: %.2f\n", 1.0/(elapsed/1000));
    }
    // printf("FPS: %.2f\n", 1.0/(elapsed/1000));

    ppu->fps_start = SDL_GetPerformanceCounter();

    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0) {
        switch (ev.type) {
            case SDL_QUIT:
                return false;
            default:
                _unes_input_update(ev);
                break;
        }
    }

    return true;
}

void unesplus_set_background_color(Color color)
{
    #ifndef UNESPLUS
    printf("UNESPLUS is disabled\n");
    return;
    #endif

    ppu->universal_bg_color = color;
}

Color DEFAULT_PALETTE[64] = {
    {124,124,124},{0,0,252},{0,0,188},{68,40,188},{148,0,132},{168,0,32},
    {168,16,0},{136,20,0},{80,48,0},{0,120,0},{0,104,0},{0,88,0},{0,64,88},
    {0,0,0},{0,0,0},{0,0,0},{188,188,188},{0,120,248},{0,88,248},{104,68,252},
    {216,0,204},{228,0,88},{248,56,0},{228,92,16},{172,124,0},{0,184,0},
    {0,168,0},{0,168,68},{0,136,136},{0,0,0},{0,0,0},{0,0,0},{248,248,248},
    {60,188,252},{104,136,252},{152,120,248},{248,120,248},{248,88,152},
    {248,120,88},{252,160,68},{248,184,0},{184,248,24},{88,216,84},{88,248,152},
    {0,232,216},{120,120,120},{0,0,0},{0,0,0},{252,252,252},{164,228,252},
    {184,184,248},{216,184,248},{248,184,248},{248,164,192},{240,208,176},
    {252,224,168},{248,216,120},{216,248,120},{184,248,184},{184,248,216},
    {0,252,252},{248,216,248},{0,0,0},{0,0,0}
};
