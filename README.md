# UltraNES

**Note: still WIP and might not finish without your help**

What if your friendly neighborhood Nintendo Entertainment System was built today?

## Features

* Pure ANSI C11 library
  * Bring your own program
* Simple, but less limited functional abstraction for the graphics
  * No sprite limit per scanline
  * Each tile can have individual palette information
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

If the library is compiled with `UNESPLUS`, then `UNESPLUS` must be defined in the final program.
