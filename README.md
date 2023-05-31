# pokewalker-tools

A collection of random tools I have made to help with developing and aroudn the
pokewalker.

These are dev tools. Only use if you know what you're doing :)

## what they do

- `colour-sprites` - converting spritesheets to 16-bit colour images and
  compressing them with a colour image index.
  Python notebook to split/encode. c file as an example on how to decode.
- `decomp.c` - testing the pw decompression algo, probably incorrect
- `extract.c` - extract portions of a binary file to their own file. Used for
  getting specific parts of the roms.
- `help.c` - fix broken walker eeproms so that game accepts them in walk end
- `shellcode/` - random shellcode for the walker, like adding watts or steps.
- probably more in the future when I find a need for it

