#ifndef __UNES_H__
#define __UNES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL.h>

#include "ppu.h"
#include "apu.h"
#include "input.h"

extern bool IS_UNESPLUS;

/**
 * @brief Initialize the window and graphics
 * @remark Must be called before any other calls
 */
void unes_init();

/**
 * @brief Exits and frees all memory
 * @remark Must be called before exit
 */
void unes_finish();

#ifdef __cplusplus
}
#endif

#endif // __UNES_H__