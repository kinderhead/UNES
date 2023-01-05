#include "gfx.h"
#include "SDL_render.h"
#include <stdlib.h>

static _UNES_GFX* graphics;

void _UNES_GFX_init() {
    graphics = malloc(sizeof(_UNES_GFX));
    CHECK_NULL(graphics->window, SDL_CreateWindow("UNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
    CHECK_NULL(graphics->renderer, SDL_CreateRenderer(graphics->window, -1, SDL_RENDERER_ACCELERATED));
    CHECK_NULL(graphics->tex, SDL_CreateTexture(graphics->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT));
}

void _UNES_GFX_free()
{
    SDL_DestroyTexture(graphics->tex);
    SDL_DestroyRenderer(graphics->renderer);
    SDL_DestroyWindow(graphics->window);

    free(graphics);
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

uint8_t* unes_get_tile(size_t index) {
    if (!_unes_valid_tile(index)) return NULL;
    return &graphics->tile_data[index * SIZEOF_TILE];
}

Sprite* unes_get_sprite(uint16_t index) {
    if (!_unes_valid_sprite(index)) return NULL;
    return &graphics->oam[index];
}

void unes_render() {
    SDL_RenderClear(graphics->renderer);

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        if (graphics->scanline_irq_counter != 0) {
            if (graphics->scanline_irq_counter-- == 0) {
                (*graphics->scanline_irq)(x);
            }
        }
    }

    SDL_RenderCopy(graphics->renderer, graphics->tex, NULL, NULL);
    SDL_RenderPresent(graphics->renderer);
}

Color DEFAULT_PALETTE[64] = {{124,124,124},{0,0,252},{0,0,188},{68,40,188},{148,0,132},{168,0,32},{168,16,0},{136,20,0},{80,48,0},{0,120,0},{0,104,0},{0,88,0},{0,64,88},{0,0,0},{0,0,0},{0,0,0},{188,188,188},{0,120,248},{0,88,248},{104,68,252},{216,0,204},{228,0,88},{248,56,0},{228,92,16},{172,124,0},{0,184,0},{0,168,0},{0,168,68},{0,136,136},{0,0,0},{0,0,0},{0,0,0},{248,248,248},{60,188,252},{104,136,252},{152,120,248},{248,120,248},{248,88,152},{248,120,88},{252,160,68},{248,184,0},{184,248,24},{88,216,84},{88,248,152},{0,232,216},{120,120,120},{0,0,0},{0,0,0},{252,252,252},{164,228,252},{184,184,248},{216,184,248},{248,184,248},{248,164,192},{240,208,176},{252,224,168},{248,216,120},{216,248,120},{184,248,184},{184,248,216},{0,252,252},{248,216,248},{0,0,0},{0,0,0}};
