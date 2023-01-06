#include "gfx.h"
#include <stdlib.h>

static _UNES_GFX* graphics;
static SDL_PixelFormat* pixel_format;

void _UNES_GFX_init() {
    graphics = malloc(sizeof(_UNES_GFX));
    CHECK_NULL(graphics->window, SDL_CreateWindow("UNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
    CHECK_NULL(graphics->renderer, SDL_CreateRenderer(graphics->window, -1, SDL_RENDERER_ACCELERATED));
    CHECK_NULL(graphics->tex, SDL_CreateTexture(graphics->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT));
    CHECK_NULL(pixel_format, SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888));

    SDL_RenderSetLogicalSize(graphics->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_GL_SetSwapInterval(0);

    graphics->fps_start = SDL_GetPerformanceCounter();
}

void _UNES_GFX_free()
{
    SDL_DestroyTexture(graphics->tex);
    SDL_DestroyRenderer(graphics->renderer);
    SDL_DestroyWindow(graphics->window);
    SDL_FreeFormat(pixel_format);

    free(graphics);
}

inline static uint32_t _unes_get_raw_color(int r, int g, int b) {
    return SDL_MapRGBA(pixel_format, r, g, b, 255);
}

inline static bool _unes_valid_tile(size_t index) {
    return (index * SIZEOF_TILE) < graphics->tile_data_size;
}

inline static bool _unes_valid_sprite(uint16_t index) {
    return index < SPRITE_COUNT;
}

void unes_set_scroll(uint16_t scrollx, uint16_t scrolly) {
    graphics->scrollx = scrollx;
    graphics->scrolly = scrolly;
}

void unes_set_tile_data(uint8_t *data, size_t size) {
    graphics->tile_data = data;
    graphics->tile_data_size = size;
}

void unes_set_scanline_interrupt(unes_scanline_interrupt irq) {
    graphics->scanline_irq = irq;
}

void unes_set_scanline_interrupt_counter(uint8_t counter) {
    graphics->scanline_irq_counter = counter;
}

uint8_t* unes_get_tile_data(size_t index) {
    if (!_unes_valid_tile(index)) return NULL;
    return &graphics->tile_data[index * SIZEOF_TILE];
}

Tile* unes_get_bg_tile(uint8_t x, uint8_t y) {
    if (x >= TOTAL_BACKGROUND_WIDTH || y >= TOTAL_BACKGROUND_HEIGHT) return NULL;
    return &graphics->nametables[x][y];
}

Sprite* unes_get_sprite(uint16_t index) {
    if (!_unes_valid_sprite(index)) return NULL;
    return &graphics->oam[index];
}

bool unes_render() {
    SDL_RenderClear(graphics->renderer);
    memset(graphics->raw_screen, 0, sizeof(graphics->raw_screen));

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x+=8) {
            
        }

        if (graphics->scanline_irq_counter != 0) {
            if (graphics->scanline_irq_counter-- == 0) {
                (*graphics->scanline_irq)(y);
            }
        }
    }

    void* pixels = NULL;
    int pitch = 0;
    SDL_LockTexture(graphics->tex, NULL, &pixels, &pitch);
    
    if (pixels == NULL) {
        printf("%s\n", SDL_GetError());
        exit(1);
    }

    memcpy(pixels, graphics->raw_screen, sizeof(graphics->raw_screen));
    SDL_UnlockTexture(graphics->tex);

    SDL_RenderCopy(graphics->renderer, graphics->tex, NULL, NULL);
    SDL_RenderPresent(graphics->renderer);

    graphics->fps_end = SDL_GetPerformanceCounter();

    double elapsed = (graphics->fps_end - graphics->fps_start) / (double) SDL_GetPerformanceFrequency() * 1000;
    SDL_Delay(floor(16.66666 - elapsed));

    graphics->fps_start = SDL_GetPerformanceCounter();

    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0) {
        switch (ev.type) {
            case SDL_QUIT:
                return false;
        }
    }

    return true;
}

Color DEFAULT_PALETTE[64] = {{124,124,124},{0,0,252},{0,0,188},{68,40,188},{148,0,132},{168,0,32},{168,16,0},{136,20,0},{80,48,0},{0,120,0},{0,104,0},{0,88,0},{0,64,88},{0,0,0},{0,0,0},{0,0,0},{188,188,188},{0,120,248},{0,88,248},{104,68,252},{216,0,204},{228,0,88},{248,56,0},{228,92,16},{172,124,0},{0,184,0},{0,168,0},{0,168,68},{0,136,136},{0,0,0},{0,0,0},{0,0,0},{248,248,248},{60,188,252},{104,136,252},{152,120,248},{248,120,248},{248,88,152},{248,120,88},{252,160,68},{248,184,0},{184,248,24},{88,216,84},{88,248,152},{0,232,216},{120,120,120},{0,0,0},{0,0,0},{252,252,252},{164,228,252},{184,184,248},{216,184,248},{248,184,248},{248,164,192},{240,208,176},{252,224,168},{248,216,120},{216,248,120},{184,248,184},{184,248,216},{0,252,252},{248,216,248},{0,0,0},{0,0,0}};
