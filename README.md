![](https://img.shields.io/github/actions/workflow/status/kinderhead/UNES/build.yml)

# UltraNES

**Note: still WIP**

What if your friendly neighborhood Nintendo Entertainment System was built today?

Documentation: https://kinderhead.github.io/UNES/

## Features

* Pure ANSI C11 library
  * At least that's what is defined in CMake, but some C90+ features (inline) don't work properly
  * Bring your own program
* Simple, but less limited graphics
  * No sprite limit per scanline
  * Easier to use scrolling and nametable system
  * Combined background and OAM tile data
  * More accurate sprite placement
* Interrupts as events
* Sound
  * More volume control per channel (8 bits instead of 4)
  * Note: in its current state it is possible to play multiple tones in one frame. This may change in the future so don't rely on it
* SDL2 backend
* Keyboard input
* Save data

## Getting Started

First, download or compile the library. The newest updates can be found in Github Actions.

Then create a starting program:
``` c
#include <unes.h>

int main(int argc, char** argv) {
    unes_init();
    unes_ppu_enable();

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

## Tutorial

A tutorial is being worked on. It can be viewed on the documentation soon.

## Examples

To compile the evolving example (under `/test`), you need the CHR ROM from Super Mario Bros. One way is to legally dump your cart and put the dump into the repo folder. Rename it to `smb.nes` and run `smb_extractor.py`. Actual examples will be added once more features are complete.

There are now examples under `/examples`.

## Contributing

Some things that need to (or should be) done are:
- [ ] Graphics
  - [x] Optimize rendering algorithm
  - [ ] Sprites
- [ ] Audio
  - [ ] Proper audio engine (if it's unsuitable)
- [ ] Controller input
- [ ] Depopulate per translation unit global variables (ppu.c, apu.c)
