#ifndef __GFX_H__
#define __GFX_H__

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef UNESPLUS
#define SPRITE_COUNT 256
#define BACKGROUND_COUNT 3
#define PALETTE_COUNT 16
#else
#define SPRITE_COUNT 64
#define BACKGROUND_COUNT 1
#define PALETTE_COUNT 4
#endif // UNESPLUS

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

/**
 * @brief Internal graphics struct
 */
typedef struct {
    SDL_Texture* tex;
    SDL_Renderer* renderer;
    SDL_Window* window;
    uint8_t raw_screen[SCREEN_WIDTH][SCREEN_HEIGHT];
} _UNES_GFX;

/**
 * @brief Sprite struct
 * @remark This object should not be created manually
 */
typedef struct {
    /**
     * @brief X position
     */
    uint8_t x;

    /**
     * @brief Y Position
     */
    uint8_t y;

    /**
     * @brief Palette index
     */
    uint8_t palette;

    /**
     * @brief Controls whether the sprite is infront of or behind the background
     * @details If true, the sprite is behind the background. If false, the sprite is infront of the background
     */
    bool priority;

    /**
     * @brief Controls if the sprite is flipped horizontally
     */
    bool h_flip;

    /**
     * @brief Controls if the sprite is flipped vertically
     */
    bool v_flip;
} Sprite;

/**
 * @brief Internal init function
 */
void _UNES_GFX_init();

/**
 * @brief Internal cleanup function
 */
void _UNES_GFX_free();

#endif // __GFX_H__