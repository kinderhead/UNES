#ifndef __GFX_H__
#define __GFX_H__

#include <SDL.h>
#include <stddef.h>
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

#define SIZEOF_TILE 16

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

    uint8_t* tile_data;
    size_t tile_data_size;
} _UNES_GFX;

/**
 * @brief Palette struct
 * @details Each member value is an index in the global palette. 
 */
typedef struct {
    uint8_t index0;
    uint8_t index1;
    uint8_t index2;
    uint8_t index3;
} Palette;

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
 * @brief Set the location UNES uses to look for tiledata
 * 
 * @param data Data pointer
 * @param size Size
 */
void unes_set_tile_data(uint8_t* data, size_t size);

/**
 * @brief Returns a pointer to the specified tile
 * 
 * The pointer is a reference to the tile in memory, so it can be
 * edited in real time. The size will always be SIZEOF_TILE.
 * Techincally you can modify or view any tile ahead, but it is
 * unrecommended to do so. You should already have the raw data
 * yourself.
 * 
 * @param index 
 * @return uint8_t* 
 */
uint8_t* unes_get_tile(size_t index);

/**
 * @brief Gets the address of a sprite
 * @details The pointer can be reused. It should not be modified during rendering.
 * 
 * @param index Sprite index
 * @return Sprite* Sprite pointer
 */
Sprite* unes_get_sprite(uint16_t index);

/**
 * @brief Render a frame. Will call interrupts
 */
void unes_render();

#define CHECK_NULL(var, stmt) do {\
                                  var = stmt;\
                                  if (var == NULL) {\
                                      printf("%s", SDL_GetError());\
                                      printf("\n");\
                                      exit(1);\
                                  }\
                              } while(0)

#endif // __GFX_H__