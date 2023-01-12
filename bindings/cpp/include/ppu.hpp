#include <unes.h>

#include <tuple>
#include <vector>

namespace unes {
    namespace ppu {
        struct Color {
            uint8_t r;
            uint8_t b;
            uint8_t g;

            Color(uint8_t r, uint8_t b, uint8_t g) {
                this->r = r;
                this->b = b;
                this->g = g;
            }
        };

        typedef uint8_t Palette[4];

        struct Tile {
            uint16_t index;
            uint8_t palette;

            Tile(uint16_t index, uint8_t palette) {
                this->index = index;
                this->palette = palette;
            }
        };

        struct Sprite {
            bool enabled;
            uint8_t x;
            uint8_t y;
            uint8_t palette;
            uint16_t tile;
            bool priority;
            bool h_flip;
            bool v_flip;

        private:
            Sprite(bool enabled, uint8_t x, uint8_t y, uint8_t palette, uint16_t tile, bool priority, bool h_flip, bool v_flip) {
                this->enabled = enabled;
                this->x = x;
                this->y = y;
                this->palette = palette;
                this->tile = tile;
                this->priority = priority;
                this->h_flip = h_flip;
                this->v_flip = v_flip;
            }
        };

        using SPRITE_SIZE = SPRITE_SIZE;

        typedef void (*scanline_interrupt)(uint8_t scanline);
        typedef void (*sprite0_hit)(uint8_t scanline);

        inline void enable() {
            unes_ppu_enable();
        }

        inline void disable() {
            unes_ppu_disable();
        }

        inline void set_sprite_size(SPRITE_SIZE size) {
            unes_set_sprite_size(size);
        }

        inline void set_scroll(int scrollx, int scrolly) {
            unes_set_scroll(scrollx, scrolly);
        }

        inline void get_scroll(uint16_t* scrollx, uint16_t* scrolly) {
            unes_get_scroll(scrollx, scrolly);
        }

        inline void get_scroll(uint16_t& scrollx, uint16_t& scrolly) {
            unes_get_scroll((uint16_t*)scrollx, (uint16_t*)scrolly);
        }

        inline std::tuple<uint16_t, uint16_t> get_scroll() {
            uint16_t x;
            uint16_t y;
            unes_get_scroll(&x, &y);
            return {x, y};
        }

        inline void set_tile_data(uint8_t* data, size_t size) {
            unes_set_tile_data(data, size);
        }

        inline void set_tile_data(std::vector<uint8_t>& data) {
            unes_set_tile_data(data.data(), data.size());
        }

        inline void set_palette(uint8_t start, uint8_t* palettes, size_t num) {
            unes_set_palettes(start, palettes, num);
        }

        inline void set_palette(uint8_t start, std::vector<uint8_t>& palettes) {
            unes_set_palettes(start, palettes.data(), palettes.size());
        }

        inline void set_palette(uint8_t index, Palette palette) {
            unes_set_palettes(index, &palette[0], 1);
        }

        inline void set_scanline_interrupt(scanline_interrupt irq) {
            unes_set_scanline_interrupt(irq);
        }

        inline void set_scanline_interrupt_counter(uint8_t counter) {
            unes_set_scanline_interrupt_counter(counter);
        }

        inline void set_sprite0_handler(sprite0_hit hit) {
            unes_set_sprite0_handler(hit);
        }

        inline uint8_t* get_tile_data(size_t index) {
            return unes_get_tile_data(index);
        }

        // inline Tile& get_bg_tile(uint8_t x, uint8_t y) {
        //     return *unes_get_bg_tile(x, y);
        // }
    }
}
