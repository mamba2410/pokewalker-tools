# decode-rotate

This is meant to debug an issue that's happening with the DWO screen in the
picowalker.
The screen is portrait but the Pokewalker is in landscape so just scaling up
leads to lot of wasted screen space and the screen form factor just isnt right.

The aim of the `decode_img` function is to decode the 2-bpp format that the 
Pokewalker uses, converts it to RGB565 (basically just a 16-bit RGB value),
rotates it by 90 degrees and scales it up by a factor of 4.

The issue is that some (all?) images don't decode completely correctly.
Some images decode horribly, some others have a black pixel on the top-left 
corner, others have white bars on the bottom.

This script/program is meant to make it easy to debug the decode function so 
that I don't have to keep re-flashing the picowalker to test changes.

## Usage

Edit the `decode_img` function in `decode-rotate.c` then compile with

```bash
gcc decode-rotate.c -o decode-rotate
```

Then to test the output run the python script

```bash
python test-decode.py <width> <height> <infile> <outfile>
```

The python script will run `decode-rotate` on the input file, given the width
and height of the image that `infile` is supposed to contain.
It will then display the image on the screen for inspection.

## Dependencies

- gcc
- python 3
- python pillow package
- an eeprom dump or some image binaries extracted from one

## TODO

- `decode-rotate` communicates decode status with exit codes as well
- Point at an eeprom file, take in an offset and generate the outfile based on
the eeprom file name and input offset.
Makes it easier to change images.

