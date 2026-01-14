# -*- coding: utf-8 -*-
"""
Created on 2026-01-08 22:40

@author: David Bahena
"""

# Python Standard Library
import sys
import json
from pathlib import Path
from typing import List, Dict, Optional

# Third Party Library
from PIL import Image

__FORMS__ = {
    "A": 0,
    "B": 1,
    "C": 2,
    "D": 3,
    "E": 4,
    "F": 5,
    "G": 6,
    "H": 7,
    "I": 8,
    "J": 9,
    "K": 10,
    "L": 11,
    "M": 12,
    "N": 13,
    "O": 14,
    "P": 15,
    "Q": 16,
    "R": 17,
    "S": 18,
    "T": 19,
    "U": 20,
    "V": 21,
    "W": 22,
    "X": 23,
    "Y": 24,
    "Z": 25,
    "Exclamation Mark": 26, # !
    "Question Mark": 27, # ?
    "Normal": 0,
    "Fighting": 1,
    "Flying": 2,
    "Poison": 3,
    "Ground": 4,
    "Rock": 5,
    "Bug": 6,
    "Ghost": 7,
    "Steel": 8,
    "Fire": 9,
    "Water": 10,
    "Grass": 11,
    "Electric": 12,
    "Psychic": 13,
    "Ice": 14,
    "Dragon": 15,
    "Dark": 16,
    "Attack": 1,
    "Defense": 2,
    "Speed": 3,
    "Plant_Cloak": 0,
    "Sandy_Cloak": 1,
    "Trash_Cloak": 2,
    "Altered": 0,
    "Origin": 1,
    "Land": 0,
    "Sky": 1,
    "Heat": 1,
    "Wash": 2,
    "Frost": 3,
    "Fan": 4,
    "Mow": 5
}

__HEADER__ = """#ifndef POKEMON_SMALL_MAP_H
#define POKEMON_SMALL_MAP_H

#ifndef __ASSEMBLER__
#include <stdint.h>

// Pokemon sprite structure with variant support
typedef struct {
    //uint16_t species;          // Pokemon species ID (1-493)
    //uint8_t variant_index;     // Form variant (0-31)
    uint32_t composite_key;    // (species << 8) | (variant_index & 0x1F)
    uint32_t bin_offset;       // Offset in merged binary file
    uint32_t size;             // Size in bytes
    uint16_t width;            // Width in pixels
    uint16_t height;           // Height in pixels
} pokemon_small_entry_t;

"""

__FOOTER__ = """extern const pokemon_small_entry_t pokemon_small_map[POKEMON_SMALL_COUNT];
extern uint8_t color_pokemon_small[POKEMON_SMALL_BIN_SIZE];

/********************************************************************************
 * @brief                   Find Pokemon sprite by species, gender, and variant
 *                          Time complexity: O(log n) binary search
 * @param species           Pokemon species ID (1-493)
 * @param variant_index     Form variant (0-31)
 * @return                  Returns pointer to sprite data, or NULL if not found
 ********************************************************************************/
uint8_t* find_pokemon_small(uint16_t species, uint8_t variant_index);

#endif // __ASSEMBLER__

#endif // POKEMON_SMALL_MAP_H

"""

__LOOKUP_FUNCTIONS__ = """
// Composite key: (species << 5) | (variant_index & 0x1F)
static inline uint32_t make_pokemon_small_key(uint16_t species, uint8_t variant_index) {
    return ((uint32_t)species << 5) | (variant_index & 0x1F);
}

uint8_t* find_pokemon_small(uint16_t species, uint8_t variant_index)
{
    uint32_t search_key = make_pokemon_small_key(species, variant_index);
    int left = 0;
    int right = POKEMON_SMALL_COUNT - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        uint32_t mid_key = pokemon_small_map[mid].composite_key;

        if (mid_key == search_key)
        {
            uint32_t offset = pokemon_small_map[mid].bin_offset;
            uint32_t size = pokemon_small_map[mid].size;

            if (offset + size > POKEMON_SMALL_BIN_SIZE)
            {
                // printf("[COLOR_POKEMON_SMALL_ERROR] Key 0x%06X: bounds check failed (offset=0x%06X + size=%u > BIN_SIZE=%u)", search_key, offset, size, POKEMON_SMALL_BIN_SIZE);
                return NULL; // Out of bounds
            }
            // printf("[COLOR_POKEMON_SMALL_FOUND] Key 0x%06X: offset=0x%06X, size=%u bytes, %ux%u pixels", search_key, offset, size, pokemon_small_map[mid].width, pokemon_small_map[mid].height);
            return color_pokemon_small + offset;
        }
        else if (mid_key < search_key) left = mid + 1;
        else right = mid - 1;
    }

    // printf("[COLOR_POKEMON_SMALL_MISS] Key 0x%06X: not found in lookup table", search_key);
    return NULL;
}
"""

# =============================================================================
#
# =============================================================================
def write_map_header(address_map: List[Dict], output_file: Path, bin_size: int):
    """
    Write C header file with Pokemon sprite mapping.

    Args:
        address_map: List of Pokemon sprite metadata dictionaries
        output_file: Output header file path
        bin_size: Total size of the binary file in bytes
    """
    # C Header File
    with open(output_file, 'w') as file:
        file.write(__HEADER__)
        file.write(f"#define POKEMON_SMALL_COUNT {len(address_map)}\n")
        file.write(f"#define POKEMON_SMALL_BIN_SIZE {bin_size}\n\n")
        file.write(__FOOTER__)

    # C Code File
    output_c_file = output_file.with_suffix('.c')
    with open(output_c_file, 'w') as file:
        file.write("#include <stddef.h>\n")
        file.write("#include <stdio.h>\n")
        file.write(f"#include \"{output_file.name}\"\n\n")
        file.write("const pokemon_small_entry_t pokemon_small_map[POKEMON_SMALL_COUNT] = {\n")

        for entry in address_map:
            file.write(
                f"    {{ 0x{entry.get('composite_key'):06X}, "
                # f"    {{ {entry.get('species'):3d}, "
                # f"{entry.get('variant_index'):2d}, "
                f"0x{entry.get('bin_offset'):06X}, "
                f"{entry.get('size'):5d}, "
                f"{entry.get('width'):2d}, "
                f"{entry.get('height'):2d} }}, // {entry.get('name'):10} {entry.get('form')}\n")

        file.write("};\n\n")

        # Write lookup functions
        file.write(__LOOKUP_FUNCTIONS__)

# =============================================================================
#
# =============================================================================
def make_composite_key(species: int, variant_index: int) -> int:
    """Create composite lookup key: (species << 8) | variant"""
    return (species << 8) | (variant_index & 0x1F)


__INFO__ = """Usage: merge_pokemon_small_to_bin.py <input_dir> <json_file> <output.bin> <output_map.h>

Example:
  .venv/bin/python3.12 colour-sprites/merge_pokemon_small_to_bin.py images/color_pokemon_small images/color_pokemon_small/icon_map.json images/output/color_pokemon_small.bin images/output/picowalker_rp2xxx_color_pokemon_small.h

"""

# =============================================================================
#
# =============================================================================
if __name__ == '__main__':
    if len(sys.argv) != 5:
        print(__INFO__)
        sys.exit(1)

    input_dir = Path(sys.argv[1])
    json_file = Path(sys.argv[2])
    output_bin = Path(sys.argv[3])
    output_map = Path(sys.argv[4])

    # Load JSON metadata
    if not json_file.exists():
        print(f"Error: JSON file not found: {json_file}")
        sys.exit(1)

    with open(json_file, 'r') as f:
        json_data:Dict = json.load(f)

    # Find all PNG files
    png_files = list(input_dir.glob("*.png"))

    if not png_files:
        print(f"No PNG files found in {input_dir}")
        sys.exit(1)

    # Parse filenames and extract Pokemon metadata
    sprites: List[Dict] = []
    for png_file in png_files:
        metadata:Dict = json_data.get(png_file.stem, "")
        if metadata:
            sprites.append({
                'composite_key': make_composite_key(int(metadata.get('dex', 0)), 
                                                    int(__FORMS__.get(metadata.get('form',''), 0))),
                'species': int(metadata.get('dex', 0)),
                'variant_index': int(__FORMS__.get(metadata.get('form',''), 0)),
                'name': metadata.get('name',''),
                "form": metadata.get('form',''),
                'filename': png_file.__str__(),
                'path': png_file
            })
        else:
            print(f"Warning: Skipping {png_file.name} (invalid format)")

    # Sort by composite key for efficient binary search
    sprites.sort(key=lambda x: x.get('composite_key'))

    print(f"Found {sprites.__len__()} Pokemon sprites")
    print(f"Species range: {sprites[0].get('composite_key')} - {sprites[-1].get('composite_key')}")

    # Build binary file with sprite data
    bin_data = b''
    address_map = []

    for sprite in sprites:
        with open(sprite.get('path'), 'rb') as file:
            png_data = file.read()

        # Get image dimensions
        img = Image.open(sprite.get('path'))
        if img.mode != 'RGBA': img = img.convert('RGBA')
        width, height = img.size

        # Convert to RGB565 raw pixel data (matching your color icons format)
        raw_pixels = bytearray()
        for y in range(height):
            for x in range(width):
                r, g, b, a = img.getpixel((x, y))
                # Convert RGB888 to RGB565
                r5 = (r >> 3) & 0x1F
                g6 = (g >> 2) & 0x3F
                b5 = (b >> 3) & 0x1F
                rgb565 = (r5 << 11) | (g6 << 5) | b5
                # Store as little-endian uint16
                raw_pixels.append(rgb565 & 0xFF)
                raw_pixels.append((rgb565 >> 8) & 0xFF)

        # Record mapping
        bin_offset = bin_data.__len__()
        address_map.append({
            'composite_key': sprite.get('composite_key'),
            'species': int(sprite.get('species')),
            'variant_index': int(sprite.get('variant_index')),
            'bin_offset': bin_offset,
            'size': raw_pixels.__len__(),
            'width': width,
            'height': height,
            'name': sprite.get('name'),
            'form': sprite.get('form')
        })

        # Append sprite data to binary file
        bin_data += raw_pixels

        composite_key = sprite.get('composite_key') # make_composite_key(sprite.get('species'), sprite.get('variant_index'))
        print(f"  Species {sprite.get('species'):03d}_{sprite.get('variant_index')} -> "
              f"offset 0x{bin_offset:06X} ({raw_pixels.__len__()} bytes) "
              f"{width}x{height} {sprite.get('name')} [key=0x{composite_key:06X}]")

    # Write binary file
    with open(output_bin, 'wb') as file:
        file.write(bin_data)
        print(f"\nWrote {len(bin_data)} bytes to {output_bin}")

    # Write C header map file
    write_map_header(address_map, output_map, len(bin_data))
    print(f"Wrote address map to {output_map}")
    print(f"Wrote C implementation to {output_map.with_suffix('.c')}")
