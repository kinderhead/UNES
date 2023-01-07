#ifndef __UNES_H__
#define __UNES_H__

#include <SDL.h>

#include "gfx.h"

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

static inline void _unes_check_unesplus() {
    #ifndef UNESPLUS
    if (IS_UNESPLUS) {
        printf("UNESPLUS is not defined but is in the library\n");
        exit(1);
    }
    #else
    if (!IS_UNESPLUS) {
        printf("UNESPLUS is defined but is not in the library\n");
        exit(1);
    }
    #endif
}

#endif // __UNES_H__