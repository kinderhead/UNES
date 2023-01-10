#include "unes.h"

#ifdef UNESPLUS
bool IS_UNESPLUS = true;
#else
bool IS_UNESPLUS = false;
#endif

void unes_init()
{
    _UNES_PPU_init();
    _UNES_APU_init();
}

void unes_finish()
{
    _UNES_PPU_free();
    _UNES_APU_free();
}
