#ifndef __GFX_H__
#define __GFX_H__

#include <SDL.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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

// Obscure bug with modulo for some reason. Must be signed?
#define TOTAL_BACKGROUND_WIDTH (int)(NAMETABLES_WIDTH*NAMETABLE_WIDTH)
#define TOTAL_BACKGROUND_HEIGHT (int)(NAMETABLES_HEIGHT*NAMETABLE_HEIGHT)

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
     * @brief Tile index. It's uint16_t for UNESPLUS support
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
     * @brief This is not on the original console. It is just an optimization and ease of use component
     * 
     */
    bool enabled;

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
     * @brief Tile to use
     * @note If 8x16 mode is selected, the next tile is the bottom half
     */
    uint16_t tile;

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
 * @brief Sprite size
 */
typedef enum {
    SIZE_8x8 = 0,
    SIZE_8x16
} SPRITE_SIZE;

/**
 * @brief Function outline for a scanline interrupt function
 */
typedef void (*unes_scanline_interrupt)(uint8_t scanline);

/**
 * @brief Function outline for a sprite 0 hit funtion
 */
typedef void (*unes_sprite0_hit)(uint8_t scanline);

/**
 * @brief Internal graphics struct
 */
typedef struct {
    SDL_Texture* tex;
    SDL_Renderer* renderer;
    SDL_Window* window;

    bool ppu_enabled;

    uint32_t raw_screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    uint32_t temp_raw_screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    Color universal_bg_color;
    
    uint16_t scrollx;
    uint16_t scrolly;

    Tile nametables[TOTAL_BACKGROUND_WIDTH][TOTAL_BACKGROUND_HEIGHT];
    Sprite oam[SPRITE_COUNT];
    SPRITE_SIZE sprite_size;

    uint8_t* tile_data;
    size_t tile_data_size;

    Palette palettes[PALETTE_COUNT*2];

    unes_sprite0_hit sprite0_hit;
    unes_scanline_interrupt scanline_irq;
    uint8_t scanline_irq_counter;

    uint64_t fps_start;
    uint64_t fps_end;
} _UNES_PPU;

/**
 * @brief Internal struct for attribute tables
 */
typedef struct {
    uint8_t top_left;
    uint8_t top_right;
    uint8_t bottom_left;
    uint8_t bottom_right;
} _UNES_ATTR;

/**
 * @brief Default nes palette
 */
extern Color DEFAULT_PALETTE[64];

/**
 * @brief Internal init function
 */
void _UNES_PPU_init();

/**
 * @brief Internal cleanup function
 */
void _UNES_PPU_free();

/**
 * @brief Enable rendering
 */
void unes_ppu_enable();

/**
 * @brief Disable rendering
 */
void unes_ppu_disable();

/**
 * @brief Sets the sprite size
 * 
 * @param size 8x8 or 8x16
 */
void unes_set_sprite_size(SPRITE_SIZE size);

/**
 * @brief Sets the screen scroll.
 * @details Accepts signed integers which rap to screen size and are cast to uint16_t.
 * 
 * @param scrollx X position
 * @param scrolly Y position
 */
void unes_set_scroll(int scrollx, int scrolly);

/**
 * @brief Gets the screen scroll.
 * @details Use this if you increment scroll to prevent overflow because UNES keeps the roll within bounds
 * 
 * @param scrollx Out: X position
 * @param scrolly Out: Y position
 */
void unes_get_scroll(uint16_t* scrollx, uint16_t* scrolly);

/**
 * @brief Sets the location UNES uses to look for tiledata
 * @remark Be sure to free this memory before exiting. UNES will not free it for you
 * 
 * @param data Data pointer
 * @param size Size
 */
void unes_set_tile_data(uint8_t* data, size_t size);

/**
 * @brief Sets multiple palettes starting at index. Does nothing if it can't copy the palettes
 * @note Palettes 0-3 are for the background and 4-7 are for sprites
 * 
 * @param start Start index
 * @param palettes Pointer to palettes
 * @param num Number of palettes to copy
 */
void unes_set_palettes(uint8_t start, uint8_t* palettes, size_t num);

/**
 * @brief Sets a palette at index
 * @note Palettes 0-3 are for the background and 4-7 are for sprites
 * 
 * @param index Index
 * @param palette Palette
 */
static inline void unes_set_palette(uint8_t index, Palette palette) {
    unes_set_palettes(index, &palette[0], 1);
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
 * @brief Calls the function when the first non transparent pixel of sprite 0 is rendered
 * 
 * @param hit Callback function
 */
void unes_set_sprite0_handler(unes_sprite0_hit hit);

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
 * @brief Sets a tile at a location
 * 
 * @param tile Tile
 * @param x X coordinate
 * @param y Y coordinate
 */
void unes_set_bg_tile(Tile tile, uint8_t x, uint8_t y);

/**
 * @brief Sets a tile at a location
 * 
 * @param index Index
 * @param palette Palette
 * @param x X
 * @param y Y
 */
static inline void unes_set_bg_tile_alt(uint16_t index, uint8_t palette, uint8_t x, uint8_t y) {
    unes_set_bg_tile((Tile){index, palette}, x, y);
}

/**
 * @brief Sets the tile index at a location
 * 
 * @param index Tile index
 * @param x X coordinate
 * @param y Y coordinate
 */
void unes_set_bg_tile_index(uint16_t index, uint8_t x, uint8_t y);

/**
 * @brief Sets the palette index at a location
 * 
 * @param index Palette index
 * @param x X coordinate
 * @param y Y coordinate
 */
void unes_set_bg_tile_palette(uint8_t index, uint8_t x, uint8_t y);

/**
 * @brief Fill the whole screen with a tile
 * 
 * @param tile Tile
 */
void unes_fill_bg(Tile tile);

/**
 * @brief Fill the whole screen with a tile
 * 
 * @param index Index
 * @param palette Palette
 */
static inline void unes_fill_bg_alt(uint16_t index, uint8_t palette) {
    unes_fill_bg((Tile){index, palette});
}

/**
 * @brief Sets background map the old fashioned way
 * 
 * This function is for compatability with existing roms and tools.
 * Since there is no mirroring and proper nametables, an x and y
 * offset can be specified to solve some problems. The offsets don't
 * have to be 32 or 30. If anything goes wrong, a message will be
 * printed.
 * 
 * @param data Data
 * @param size Size
 * @param x_offset X offset in bytes
 * @param y_offset Y offset in bytes
 * @param row_size Size of a row
 * 
 * @see unes_legacy_set_nametable()
 */
void unes_legacy_set_map(uint8_t* data, size_t size, uint8_t x_offset, uint8_t y_offset, uint8_t row_size);

/**
 * @brief Sets a full nametable with attributes the old fashioned way
 * 
 * This function is for compatability with existing roms and tools.
 * It accepts a pointer with size of 1024 which includes the attribute
 * table. Offsets can be used for more control. The offsets don't have
 * to be 32 or 30. If anything goes wrong, a message will be printed.
 * The data's row size is assumed to be 32.
 * 
 * @param data Data with size 1024
 * @param x_offset X offset in bytes
 * @param y_offset Y offset in bytes
 * 
 * @see unes_legacy_set_map()
 */
void unes_legacy_set_nametable(uint8_t* data, uint8_t x_offset, uint8_t y_offset);

/**
 * @brief Gets the address of a sprite
 * @details The pointer can be reused. It should not be modified during rendering
 * 
 * @param index Sprite index
 * @return Sprite* Sprite pointer, or NULL if the index is invalid
 */
Sprite* unes_get_sprite(uint16_t index);

/**
 * @brief Gets the distance to the edge of the tile on all sides. It's used by the renderer, but I thought it would be useful
 * @warning This function does not work correctly and it might be needed
 * @deprecated
 * @param tile Tile index
 * @param l Out: Distance from left edge to leftmost pixel
 * @param r Out: Distance from left edge to rightmost pixel
 * @param u Out: Distance from upper edge to highest pixel
 * @param d Out: Distance from upper edge to lowest pixel
 */
void unes_get_bounding_box(uint16_t tile, uint8_t* l, uint8_t* r, uint8_t* u, uint8_t* d);

/**
 * @brief Sets the universal background color, or #3F00/#3F10 on original hardware
 * 
 * @param index Global palette index
 */
void unes_set_background_color(uint8_t index);

/**
 * @brief Render a frame. Will call interrupts
 * @remark While drawing back priority sprites, the renderer does not check if the background pixel is palette index 0. Only if it is the same as the universal background color.
 * @return bool Returns true if the execution should continue
 */
bool unes_render();

/// UNESPLUS only functions ///

/**
 * @brief Sets the universal background color
 * @remark Can only be called if using UNESPLUS
 * 
 * @param color 
 */
void unesplus_set_background_color(Color color);

#define CHECK_NULL(var, stmt) do {\
                                  var = stmt;\
                                  if (var == NULL) {\
                                      printf("%s\n", SDL_GetError());\
                                      printf("\n");\
                                      exit(1);\
                                  }\
                              } while(0)

#endif // __GFX_H__