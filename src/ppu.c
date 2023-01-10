#include "ppu.h"
#include <stdlib.h>

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

inline static void _unes_render_tile(uint32_t out[8][8], Tile tile) {
    uint8_t* raw_tile = unes_get_tile_data(tile.tile);
    for (int y = 0; y < 8; y++)
    {
        for (int x = 7; x >= 0; x--)
        {
            if (tile.palette >= PALETTE_COUNT) {printf("Invalid palette %d\n", (int)tile.palette); continue;}
            
            uint8_t index = ((raw_tile[y]>>x)&1) | (((raw_tile[y+8]>>x)&1)<<1);
            Color color;
            if (index == 0) {
                color = ppu->universal_bg_color;
            } else {
                color = DEFAULT_PALETTE[ppu->palettes[tile.palette][index]];
            }

            // So it copies nicely into the final buffer
            out[y][7-x] = _unes_get_raw_color(color.r, color.g, color.b);
        }
    }
}

void unes_set_scroll(uint16_t scrollx, uint16_t scrolly) {
    ppu->scrollx = scrollx % (TOTAL_BACKGROUND_WIDTH*8);
    ppu->scrolly = scrolly % (TOTAL_BACKGROUND_HEIGHT*8);
}

void unes_get_scroll(uint16_t* scrollx, uint16_t* scrolly)
{
    *scrollx = ppu->scrollx;
    *scrolly = ppu->scrolly;
}

void unes_set_tile_data(uint8_t* data, size_t size) {
    ppu->tile_data = data;
    ppu->tile_data_size = size;
}

void unes_set_palettes(uint8_t start, uint8_t* palettes, size_t num) {
    if (num + start >= PALETTE_COUNT) return;
    memcpy(&ppu->palettes[start], palettes, sizeof(Palette) * num);
}

void unes_set_scanline_interrupt(unes_scanline_interrupt irq) {
    ppu->scanline_irq = irq;
}

void unes_set_scanline_interrupt_counter(uint8_t counter) {
    ppu->scanline_irq_counter = counter;
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
        ppu->nametables[i][y].tile = data[i];
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
    // This is painfully unoptimized :(
    // Possibly could add a cache system

    SDL_RenderClear(ppu->renderer);
    if (ppu->tile_data != NULL && ppu->ppu_enabled) {
        memset(ppu->raw_screen, 0, sizeof(ppu->raw_screen));

        for (int y = 0; y < TOTAL_BACKGROUND_HEIGHT*8; y+=8)
        {
            for (int x = 0; x < TOTAL_BACKGROUND_WIDTH*8; x+=8)
            {
                uint32_t tile[8][8] = {0};
                _unes_render_tile(tile, ppu->nametables[x/8][y/8]);
                for (int i = 0; i < 8; i++)
                {
                    memcpy(&ppu->whole_screen[y+i][x], &tile[i], sizeof(tile[0]));
                }
            }
        }
        
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            if (ppu->scrollx >= (TOTAL_BACKGROUND_WIDTH*8)-SCREEN_WIDTH && ppu->scrolly >= (TOTAL_BACKGROUND_HEIGHT*8)-SCREEN_HEIGHT) {
                
            } else if (ppu->scrollx >= (TOTAL_BACKGROUND_WIDTH*8)-SCREEN_WIDTH) {
                
            } else if (ppu->scrolly >= (TOTAL_BACKGROUND_HEIGHT*8)-SCREEN_HEIGHT) {

            } else {
                memcpy(&ppu->raw_screen[y][0], &ppu->whole_screen[y+ppu->scrolly][ppu->scrollx], sizeof(ppu->raw_screen[0]));
            }
            if (ppu->scanline_irq_counter != 0) {
                if (ppu->scanline_irq_counter-- == 0) {
                    (*ppu->scanline_irq)(y);
                }
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

    ppu->fps_start = SDL_GetPerformanceCounter();

    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0) {
        switch (ev.type) {
            case SDL_QUIT:
                return false;
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
