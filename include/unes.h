#ifndef __UNES_H__
#define __UNES_H__

#include <SDL.h>

#include "gfx.h"

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

#endif // __UNES_H__