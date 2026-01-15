# -*- coding: utf-8 -*-
"""
Created on 2025-12-30 11:46

@author: David Bahena
"""

# Python Standard Library
import sys
from pathlib import Path
from typing import List, Dict

# Third Party Library
from PIL import Image

__HEADER__ = """#ifndef ICONS_MAP_H
#define ICONS_MAP_H

#ifndef __ASSEMBLER__
#include <stdint.h>

// Base structure (ALL sprites including UI icons)
typedef struct {
    uint16_t eeprom_address;    // Original EEPROM address
    uint32_t bin_offset;        // Offset in merged binary file
    uint32_t size;              // Size in bytes
    uint16_t width;             // Width in pixels
    uint16_t height;            // Height in pixels
} color_icons_t;

"""

__FOOTER__ = """extern const color_icons_t icons_map[ICONS_COUNT];
extern uint8_t color_icons[ICONS_BIN_SIZE];

/********************************************************************************
 * @brief                   Find sprite image data by original EEPROM address using binary search
                            Time complexity: O(log n)
 * @param eeprom_address    address needed for lookup table
 * @return                  Returns pointer to image data in color_icons bin, or NULL if not found
********************************************************************************/
const color_icons_t* find_icon_by_eeprom_address(uint16_t eeprom_address)
//uint8_t* find_icon_by_eeprom_address(uint16_t eeprom_address);

#endif // __ASSEMBLER__

#endif // ICONS_MAP_H

"""
__LOOKUP_TABLE__ = """
//uint8_t* find_icon_by_eeprom_address(uint16_t eeprom_address) 
const color_icons_t* find_icon_by_eeprom_address(uint16_t eeprom_address)
{
    int left = 0;
    int right = ICONS_COUNT - 1;

    while (left <= right) 
    {
        int mid = left + (right - left) / 2;
        uint16_t mid_addr = icons_map[mid].eeprom_address;

        if (mid_addr == eeprom_address) 
        {
            uint32_t offset = icons_map[mid].bin_offset;
            uint32_t size = icons_map[mid].size;

            if (offset + size > ICONS_BIN_SIZE) 
            {
                // printf("[COLOR_ICON_ERROR] Address 0x%04X: bounds check failed (offset=0x%06X + size=%u > BIN_SIZE=%u)", eeprom_address, offset, size, ICONS_BIN_SIZE);
                return NULL; // Out of bounds
            }
            // printf("[COLOR_ICON_FOUND] Address 0x%04X: offset=0x%06X, size=%u bytes, %ux%u pixels", eeprom_address, offset, size, icons_map[mid].width, icons_map[mid].height);
            // return color_icons + offset;
            return &icons_map[mid];
        } 
        else if (mid_addr < eeprom_address) left = mid + 1;
        else right = mid - 1;
    }

    // printf("[COLOR_ICON_MISS] Address 0x%04X: not found in lookup table", eeprom_address);
    return NULL;
}
"""

def write_map_header(address_map:List[Dict], output_file:Path, bin_size:int):
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
        file.write(f"#define ICONS_COUNT {address_map.__len__()}\n")
        file.write(f"#define ICONS_BIN_SIZE {bin_size}\n\n")
        file.write(__FOOTER__)

    # C Code File
    output_c_file = output_file.__str__().replace('.h','.c')
    with open(output_c_file, 'w') as file:
        file.write("#include <stddef.h>\n")
        file.write("#include <stdio.h>\n")
        file.write(f"""#include "{output_file.name}"\n\n""") # picowalker_rp2xxx_color_icons.h
        file.write("const color_icons_t icons_map[ICONS_COUNT] = {\n")

        for entry in address_map:
            file.write(f"    {{ 0x{entry.get('eeprom_address'):04X}, "
                   f"0x{entry.get('bin_offset'):06X}, "
                   f"{entry.get('size'):4}, "
                   f"{entry.get('width'):2}, "
                   f"{entry.get('height'):2} }},  // {entry.get('name')}\n")

        file.write("};\n\n")

        # Write binary search lookup function
        file.write(__LOOKUP_TABLE__)

__INFO__ = """Usage: merge_icons_to_bin.py <input_dir> <output.bin> <output_map.h>

Example:
    .venv/bin/python3.12 colour-sprites/merge_icons_to_bin.py images/color_icons images/output/color_icons.bin images/output/picowalker_rp2xxx_color_icons.h
"""

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
            _address = png_file.name.split('_')
            if _address.__len__() < 2:
                raise ValueError(f"Invalid filename format: {png_file.name}")
            
            _address:str = _address[0]
            if _address.startswith('0x') or _address.startswith('0X'):
                address = int(_address, 16)
            else:
                raise ValueError(f"Address must start with 0x: {_address}")
            
            sprites.append({
                'eeprom_address': address,
                'name': png_file.name,
                'filename': png_file.__str__(),
                'path': png_file
            })
        except ValueError as exception:
            print(f"Warning: Skipping {png_file.name}: {exception}")

    # Sort by EEPROM address (ascending)
    sprites.sort(key=lambda x: x.get('eeprom_address'))

    print(f"Found {sprites.__len__()} sprites")
    print(f"Address range: 0x{sprites[0]['eeprom_address']:04X} - 0x{sprites[-1]['eeprom_address']:04X}")

    # Build binary file with PNG data
    bin_data = b''
    address_map = []

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

        # Record mapping
        bin_offset = bin_data.__len__()
        address_map.append({
            'eeprom_address': sprite.get('eeprom_address'),
            'bin_offset': bin_offset,
            'size': raw_pixels.__len__(),
            'width': width,
            'height': height,
            'name': sprite.get('name')
        })

        # Append PNG data to binary file
        bin_data += raw_pixels

        print(f"  0x{sprite.get('eeprom_address'):04X} -> offset 0x{bin_offset:06X} ({png_data.__len__()} bytes) {width}x{height} {sprite.get('name')}")

    # Write binary file
    with open(output_bin, 'wb') as file:
        file.write(bin_data)
        print(f"\nWrote {bin_data.__len__()} bytes to {output_bin}")

    # Write C header map file
    write_map_header(address_map, output_map, bin_data.__len__())
    print(f"Wrote address map to {output_map}")