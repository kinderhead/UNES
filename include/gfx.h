#ifndef __GFX_H__
#define __GFX_H__

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef UNESPLUS
#define SPRITE_COUNT 256
#define BACKGROUND_COUNT 3
#define PALETTE_COUNT 16
#define NAMETABLE_COUNT 16
#else
#define SPRITE_COUNT 64
#define BACKGROUND_COUNT 1
#define PALETTE_COUNT 4
#define NAMETABLE_COUNT 4
#endif // UNESPLUS

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

#define NAMETABLE_WIDTH 32
#define NAMETABLE_HEIGHT 30

#define TOTAL_BACKGROUND_WIDTH NAMETABLE_COUNT*NAMETABLE_WIDTH
#define TOTAL_BACKGROUND_HEIGHT NAMETABLE_COUNT*NAMETABLE_HEIGHT

/**
 * @brief Simple color struct
 */
typedef struct {
    uint8_t r;
    uint8_t b;
    uint8_t g;
} Color;

/**
 * @brief Struct representing a background tile
 */
typedef struct {
    /**
     * @brief Tile index
     */
    uint16_t tile;

    /**
     * @brief Palette index
     */
    uint8_t palette;
} Tile;

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
 * @brief Internal graphics struct
 */
typedef struct {
    SDL_Texture* tex;
    SDL_Renderer* renderer;
    SDL_Window* window;
    uint8_t raw_screen[SCREEN_WIDTH*2][SCREEN_HEIGHT*2];
    
    uint16_t scrollx;
    uint16_t scrolly;

    Tile nametables[TOTAL_BACKGROUND_WIDTH][TOTAL_BACKGROUND_HEIGHT];
    Sprite oam[SPRITE_COUNT];
} _UNES_GFX;

/**
 * @brief Default nes palette
 */
extern Color DEFAULT_PALETTE[64];

/**
 * @brief Internal init function
 */
void _UNES_GFX_init();

/**
 * @brief Internal cleanup function
 */
void _UNES_GFX_free();

/**
 * @brief Set the screen scroll
 * 
 * @param scrollx X position
 * @param scrolly Y position
 */
void unes_set_scroll(uint16_t scrollx, uint16_t scrolly);

/**
 * @brief Gets the address of a sprite
 * @details The pointer can be reused. It should not be modified during rendering.
 * 
 * @param index Sprite index
 * @return Sprite* Sprite pointer
 */
Sprite* unes_get_sprite(uint16_t index);

#endif // __GFX_H__