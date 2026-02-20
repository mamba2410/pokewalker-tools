# -*- coding: utf-8 -*-
"""
Created on 2026-01-04 20:40

@author: David Bahena
"""

# Python Standard Library
import sys
from pathlib import Path
from typing import List, Dict

# Third Party Library
from PIL import Image

# RLE
from rle_raw_pixels import rle_compress_rgb565

__HEADER__ = """#ifndef ROUTES_MAP_H
#define ROUTES_MAP_H

#ifndef __ASSEMBLER__
#include <stdint.h>

// Base structure
typedef struct {
    uint8_t route_index;        // Values (0-7) 0xBF06 Address for Route Index Lookup
    uint32_t bin_offset;        // Offset in merged binary file
    uint32_t size;              // Size in bytes
    uint32_t uncompressed_size; // Size in bytes
    uint16_t width;             // Width in pixels
    uint16_t height;            // Height in pixels
} color_routes_t;

"""

__FOOTER__ = """extern const color_routes_t routes_map[ROUTES_COUNT];
extern uint8_t color_routes[ROUTES_BIN_SIZE];

/********************************************************************************
 * @brief                   Find sprite image data by route index (0-7)
                            Time complexity: O(1) - direct array access
 * @param eeprom_address    address needed for lookup table
 * @return                  Returns pointer to image data in color_routes bin, or NULL if not found
********************************************************************************/
const color_routes_t* find_route_by_index(uint8_t route_index);
//uint8_t* find_route_by_index(uint8_t route_index);

#endif // __ASSEMBLER__

#endif // ROUTES_MAP_H

"""

__LOOKUP_TABLE__ = r"""
//uint8_t* find_route_by_index(uint8_t route_index) 
const color_routes_t* find_route_by_index(uint8_t route_index)
{
    if (route_index >= ROUTES_COUNT) 
    {
        // printf("[COLOR_ROUTE_MISS] Invalid Index %u\n", route_index);
        return NULL;
    }

    uint32_t offset = routes_map[route_index].bin_offset;
    uint32_t size = routes_map[route_index].size;

    if (offset + size > ROUTES_BIN_SIZE) 
    {
        // printf("[COLOR_ROUTE_ERROR] bounds check failed (offset=0x%06X + size=%u > BIN_SIZE=%u)\n", offset, size, ROUTES_BIN_SIZE);
        return NULL; // Out of bounds
    }

    // printf("[COLOR_ROUTE_FOUND] index=%u\n", route_index);
    // return color_routes + offset;
    return &routes_map[route_index];
}
"""

# =============================================================================
#
# =============================================================================
def write_map_header(index_map:List[Dict], output_file:Path, bin_size:int):
    """
    Write C header file with address mapping.

    Args:
        address_map: List of sprite metadata dictionaries
        output_file: Output header file path
        total_bin_size: Total size of the binary file in bytes
    """

    # C Header File
    with open(output_file, 'w') as file:
        file.write(__HEADER__)
        file.write(f"#define ROUTES_COUNT {index_map.__len__()}\n")
        file.write(f"#define ROUTES_BIN_SIZE {bin_size}\n\n")
        file.write(__FOOTER__)

    # C Code File
    output_c_file = output_file.__str__().replace('.h','.c')
    with open(output_c_file, 'w') as file:
        file.write("#include <stddef.h>\n")
        file.write("#include <stdio.h>\n")
        file.write(f"""#include "{output_file.name}"\n\n""")
        file.write("const color_routes_t routes_map[ROUTES_COUNT] = {\n")

        for entry in index_map:
            file.write(f"    {{ {entry.get('route_index')}, "
                   f"0x{entry.get('bin_offset'):06X}, "
                   f"{entry.get('size')}, "
                   f"{entry.get('uncompressed_size'):4}, "
                   f"{entry.get('width')}, "
                   f"{entry.get('height')} }},  // {entry.get('name')}\n")

        file.write("};\n\n")

        # Write lookup function
        file.write(__LOOKUP_TABLE__)

__INFO__ = """Usage: merge_routes_to_bin.py <input_dir> <output.bin> <output_map.h>

Example:
    .venv/bin/python3.12 colour-sprites/merge_routes_to_bin.py images/color_routes images/output/color_routes.bin images/output/picowalker_rp2xxx_color_routes.h
"""

# =============================================================================
#
# =============================================================================
if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(__INFO__)
        sys.exit(1)

    input_dir = Path(sys.argv[1])
    output_bin = Path(sys.argv[2])
    output_map = Path(sys.argv[3])

    # Find all PNG files
    png_files = list(Path(input_dir).glob("*.png"))

    if not png_files:
        print(f"No PNG files found in {input_dir}")
        sys.exit(1)

    # Parse filenames and sort by address
    sprites:List[Dict] = []
    for png_file in png_files:
        try:
            _index = png_file.name.split('_')
            if _index.__len__() < 2:
                raise ValueError(f"Invalid filename format: {png_file.name}")

            # Extract index from filename (expecting format like "0_route_name.png")
            index:int = int(_index[1])

            if index < 0 or index > 7:
                raise ValueError(f"Route index must be 0-7: {index}")

            sprites.append({
                'index': index,
                'name': png_file.name,
                'filename': png_file.__str__(),
                'path': png_file
            })
        except ValueError as exception:
            print(f"Warning: Skipping {png_file.name}: {exception}")

    # Sort by Index (ascending)
    sprites.sort(key=lambda x: x.get('index'))

    print(f"Found {sprites.__len__()} sprites")

    # Build binary file with PNG data
    bin_data = b''
    index_map = []

    for sprite in sprites:
        with open(sprite.get('path'), 'rb') as file:
            png_data = file.read()

        # Get image dimensions
        img = Image.open(sprite.get('path'))
        width, height = img.size

        # Convert to RGB565 raw pixel data
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

        compressed_pixels = rle_compress_rgb565(raw_pixels)

        # Record mapping
        index_map.append({
            'route_index': sprite.get('index'),
            'bin_offset': bin_data.__len__(),
            'size': compressed_pixels.__len__(),
            'uncompressed_size': raw_pixels.__len__(),
            'width': width,
            'height': height,
            'name': sprite.get('name')
        })

        print(f"  Index: {sprite.get('index')} -> offset 0x{bin_data.__len__():06X} ({compressed_pixels.__len__()} bytes) {width}x{height} {sprite.get('name')}")

        # Append PNG data to binary file
        bin_data += compressed_pixels #raw_pixels

    # Write binary file
    with open(output_bin, 'wb') as file:
        file.write(bin_data)
        print(f"\nWrote {bin_data.__len__()} bytes to {output_bin}")

    # Write C header map file
    write_map_header(index_map, output_map, bin_data.__len__())
    print(f"Wrote address map to {output_map}")