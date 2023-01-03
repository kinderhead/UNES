#include "unes.h"

void unes_init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
}

void unes_finish()
{
    _UNES_GFX_free();
    SDL_Quit();
}
