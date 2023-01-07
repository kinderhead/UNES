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
* Interrupts as events
* Sound (hopefully)
* SDL2 backend
* Keyboard input
* Save data

## UNES Plus

If `UNESPLUS` is defined during library compilation, the normal specs for the graphics is increased. 

Some changes are:
* Sprites: 64 -> 256
* Backgrounds: 1 -> 3
* Palette count (per layer): 4 -> 16
* 32 bit universal background color

If the library is compiled with `UNESPLUS`, then `UNESPLUS` must be defined in the final program.

## Contributing

Some things that need to (or should be) done are:
- [ ] Graphics
  - [ ] Optimize rendering algorithm
- [ ] Audio
- [ ] Controller input
