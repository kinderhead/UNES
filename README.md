# UltraNES

**Note: still WIP**

What if your friendly neighborhood Nintendo Entertainment System was built today?

Documentation: https://kinderhead.github.io/UNES/

## Features

* Pure ANSI C11 library
  * At least that's what is defined in CMake, but some C90+ features (inline) don't work properly
  * Bring your own program
* Simple but less limited graphics api
  * No sprite limit per scanline
  * Easier to use scrolling and nametable system
  * Combined background and OAM tile data
* Interrupts as events
* Sound (hopefully)
* SDL2 backend
* Keyboard input
* Save data

## Getting Started

First, download or compile the library. The current version can be found in Github Actions.

Then create a starting program:
``` c
#include <unes.h>

int main(int argc, char** argv) {
    unes_init();

    // Initialize tile data and palettes
    // unes_set_tile_data(tile_data, sizeof(tile_data));
    // unes_set_palette(0, (Palette){0x00, 0x01, 0x02, 0x03});

    while (unes_render()) {
        // Game code here
    }

    unes_finish();
}
```
Tile data can be imported from popular programs like YY-CHR or from a rom.

## UltraNES+

If `UNESPLUS` is defined during library compilation, the normal specs are increased. 

Some changes are:
* Sprites: 64 -> 256
* Backgrounds: 1 -> 3
* Palette count (per layer): 4 -> 16
* 32 bit universal background color

If the library is compiled with `UNESPLUS`, then `UNESPLUS` is highly recommended to be defined in the final program by the user. Until a reasonable way to check this during runtime is implemented, you may get undefined behaviors.

## Examples

To compile the evolving example (under /test), you need the CHR ROM from Super Mario Bros. One way is to legally dump your cart and put the dump into the repo folder. Rename it to `smb.nes` and run `smb_extractor.py`. Actual examples will be added once more features are complete.

## Contributing

Some things that need to (or should be) done are:
- [ ] Graphics
  - [ ] Optimize rendering algorithm
- [ ] Audio
- [ ] Controller input