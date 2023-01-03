#include "gfx.h"
#include <stdlib.h>

static _UNES_GFX* graphics;

void _UNES_GFX_init() {
    graphics = malloc(sizeof(_UNES_GFX));
}

void _UNES_GFX_free()
{
    SDL_DestroyTexture(graphics->tex);
    SDL_DestroyRenderer(graphics->renderer);
    SDL_DestroyWindow(graphics->window);

    free(graphics);
}
