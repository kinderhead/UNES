#include "unes.h"
#include "gfx.h"

void unes_init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    _UNES_GFX_init();
}

void unes_finish()
{
    _UNES_GFX_free();
    SDL_Quit();
}
