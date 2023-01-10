#include "unes.h"

#ifdef UNESPLUS
bool IS_UNESPLUS = true;
#else
bool IS_UNESPLUS = false;
#endif

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
