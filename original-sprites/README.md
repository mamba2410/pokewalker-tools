# Fork of Pokewalker-Scripts by S-len

See original repo [here](https://github.com/S-len/Pokewalker-Scripts) and [README_Original.md](./README_Original.md).

## Usage

### dump_sprites.py

Rip greyscale pokemon sprites from the game files.

- Obtain a completely legal copy of the HG/SS rom.
- Use [EveryFileExplorer](https://github.com/PabloMK7/EveryFileExplorer) to extract `a/2/5/6` to get all the pokewalker large sprites, and `a/2/4/8` for the small sprites.
- Run `dump_sprites.py a256_pwlarge.narc` to extract all the raw binary info (`out/*.bin`) and convert the sprites to png (`out/*.png`).
- Results should be in the `out/` directory.

### str_to_img.py

Create text images for use on the walker.

- Grab the diamond/pearl/platinum font
- Run `str_to_img.py "my string" outfile.bmp`
- This should create an image containing `my_string` and save it as a bitmap image

You can then convert this to pokewalker format with `img_to_bin.py`

### img_to_bin.py

Convert an image to pokewalker format binary data.

- Run `img_to_bin.py infile.bmp outfile.bin`

### License

MIT
