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
#define NAMETABLES_WIDTH 4
#define NAMETABLES_HEIGHT 4
#else
#define SPRITE_COUNT 64
#define BACKGROUND_COUNT 1
#define PALETTE_COUNT 4
#define NAMETABLES_WIDTH 2
#define NAMETABLES_HEIGHT 2
#endif // UNESPLUS

#define NAMETABLE_COUNT NAMETABLES_WIDTH*NAMETABLES_HEIGHT

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

#define NAMETABLE_WIDTH 32
#define NAMETABLE_HEIGHT 30

#define TOTAL_BACKGROUND_WIDTH NAMETABLES_WIDTH*NAMETABLE_WIDTH
#define TOTAL_BACKGROUND_HEIGHT NAMETABLES_HEIGHT*NAMETABLE_HEIGHT

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
 * @brief Palette struct
 * @details Each member value is an index in the global palette. 
 */
typedef uint8_t Palette[4];

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
 * @brief Function outline for a scanline interrupt function
 */
typedef void (*unes_scanline_interrupt)(uint8_t scanline);

/**
 * @brief Internal graphics struct
 */
typedef struct {
    SDL_Texture* tex;
    SDL_Renderer* renderer;
    SDL_Window* window;
    uint32_t raw_screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    
    uint16_t scrollx;
    uint16_t scrolly;

    Tile nametables[TOTAL_BACKGROUND_WIDTH][TOTAL_BACKGROUND_HEIGHT];
    Sprite oam[SPRITE_COUNT];

    uint8_t* tile_data;
    size_t tile_data_size;

    Palette palettes[PALETTE_COUNT];

    unes_scanline_interrupt scanline_irq;
    uint8_t scanline_irq_counter;

    uint64_t fps_start;
    uint64_t fps_end;
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
 * @brief Sets the screen scroll
 * 
 * @param scrollx X position
 * @param scrolly Y position
 */
void unes_set_scroll(uint16_t scrollx, uint16_t scrolly);

/**
 * @brief Sets the location UNES uses to look for tiledata
 * @remark Be sure to free this memory before exiting. UNES will not free it for you
 * 
 * @param data Data pointer
 * @param size Size
 */
void unes_set_tile_data(uint8_t* data, size_t size);

/**
 * @brief Makes
 * 
 * @param index 
 * @param palette 
 */
inline void unes_set_palette(uint8_t index, Palette palette) {

}

/**
 * @brief Sets the scanline interrupt function
 * @details It is called when the irq counter reaches zero. It is passed the current scanline number
 * 
 * @param irq Interrupt pointer
 */
void unes_set_scanline_interrupt(unes_scanline_interrupt irq);

/**
 * @brief Sets the scanline interrupt counter
 * 
 * It counts down at the end of rendering one scanline. If it
 * hits zero, the interrupt is called and the next scanline is
 * affected. For example, if the counter is set to 15, then the
 * interrupt is called before rendering the 16th scanline. The
 * 16th scanline and beyond can be modified. The counter can be
 * set again to interrupt again later in the same frame.
 * 
 * @param counter 
 */
void unes_set_scanline_interrupt_counter(uint8_t counter);

/**
 * @brief Returns a pointer to the specified tile's data
 * 
 * The pointer is a reference to the tile in memory, so it can be
 * edited in real time. The size will always be SIZEOF_TILE.
 * Techincally you can modify or view any tile ahead, but it is
 * unrecommended to do so. You should already have the raw data
 * yourself.
 * 
 * @param index Tile index
 * @return uint8_t* Tile pointer, or NULL if the index is invalid
 */
uint8_t* unes_get_tile_data(size_t index);

/**
 * @brief Gets the tile at position. It can also be modified
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @return Tile* Tile pointer, or NULL if the coordinate is invalid
 */
Tile* unes_get_bg_tile(uint8_t x, uint8_t y);

/**
 * @brief Gets the address of a sprite
 * @details The pointer can be reused. It should not be modified during rendering.
 * 
 * @param index Sprite index
 * @return Sprite* Sprite pointer, or NULL if the index is invalid
 */
Sprite* unes_get_sprite(uint16_t index);

/**
 * @brief Render a frame. Will call interrupts
 * @return bool Returns true if the execution should continue
 */
bool unes_render();

#define CHECK_NULL(var, stmt) do {\
                                  var = stmt;\
                                  if (var == NULL) {\
                                      printf("%s\n", SDL_GetError());\
                                      printf("\n");\
                                      exit(1);\
                                  }\
                              } while(0)

#endif // __GFX_H__