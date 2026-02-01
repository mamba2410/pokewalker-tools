# -*- coding: utf-8 -*-
"""
Created on Tue Jan 13 22:33:46 2026

@author: David Bahena
"""
 
# Python Standard Library
import sys
from pathlib import Path
from typing import List, Dict

# =============================================================================
#
# =============================================================================
def rle_compress_rgb565(raw_pixels: bytearray) -> bytearray:
    """
    RLE compress RGB565 pixel data (2 bytes per pixel).
    
    Format: [count][pixel_low][pixel_high] where count = 1-255 repeats
    For non-repeating pixels: count=0 means raw copy next pixel
    
    Args:
        raw_pixels: RGB565 data (2 bytes per pixel, little-endian)
    
    Returns:
        Compressed bytearray
    """
    if len(raw_pixels) == 0:
        return bytearray()
    
    compressed = bytearray()
    i = 0
    
    while i < len(raw_pixels):
        # Get current pixel (2 bytes)
        pixel_low = raw_pixels[i]
        pixel_high = raw_pixels[i + 1]
        
        # Count consecutive identical pixels
        run_length = 1
        while (i + run_length * 2 < len(raw_pixels) and
               raw_pixels[i + run_length * 2] == pixel_low and
               raw_pixels[i + run_length * 2 + 1] == pixel_high and
               run_length < 255):
            run_length += 1
        
        if run_length >= 3:  # Worth compressing (3+ repeats)
            compressed.append(run_length)
            compressed.append(pixel_low)
            compressed.append(pixel_high)
            i += run_length * 2
        else:
            # Not worth compressing, use literal run
            # Find how many non-repeating pixels
            literal_count = 0
            j = i
            while j < len(raw_pixels) and literal_count < 255:
                # Check if next pixel repeats 3+ times
                next_run = 1
                if j + 2 < len(raw_pixels):
                    while (j + next_run * 2 < len(raw_pixels) and
                           raw_pixels[j + next_run * 2] == raw_pixels[j] and
                           raw_pixels[j + next_run * 2 + 1] == raw_pixels[j + 1] and
                           next_run < 3):
                        next_run += 1

                if next_run >= 3:  # Found a run, stop literal sequence
                    break

                literal_count += 1
                j += 2

            # Encode literal run (always at least 1 pixel if run_length < 3)
            if literal_count == 0:
                literal_count = 1  # Must encode at least current pixel

            compressed.append(0)  # 0 = literal run marker
            compressed.append(literal_count)
            compressed.extend(raw_pixels[i:i + literal_count * 2])
            i += literal_count * 2
    
    return compressed

# =============================================================================
#
# =============================================================================
def rle_decompress_rgb565(compressed: bytearray, expected_size: int) -> bytearray:
    """
    Decompress RLE RGB565 data.
    
    Args:
        compressed: RLE compressed data
        expected_size: Expected output size in bytes
    
    Returns:
        Decompressed RGB565 bytearray
    """
    decompressed = bytearray()
    i = 0
    
    while i < len(compressed) and len(decompressed) < expected_size:
        count = compressed[i]
        i += 1
        
        if count == 0:  # Literal run
            literal_count = compressed[i]
            i += 1
            decompressed.extend(compressed[i:i + literal_count * 2])
            i += literal_count * 2
        else:  # Repeated pixel
            pixel_low = compressed[i]
            pixel_high = compressed[i + 1]
            i += 2
            for _ in range(count):
                decompressed.append(pixel_low)
                decompressed.append(pixel_high)
    
    return decompressed

__HEADER__ ="""#ifndef RLE_H
#define RLE_H

#include <stdint.h>
#include <stdlib.h>

"""

__FOOTER__ = """

/********************************************************************************
 * @brief               Decompress RLE RGB565 sprite data
 * @param compressed    Pointer to compressed RLE data
 * @param output_size   Expected output size in bytes
 * @return              Pointer to decompressed image
********************************************************************************/
uint8_t* rle_decompress_rgb565(const uint8_t* compressed, uint32_t output_size);

#endif // RLE_H
"""

__FUNCTIONS__ = """
uint8_t* rle_decompress_rgb565(const uint8_t* compressed, uint32_t output_size)
{
    static uint8_t output[64 * 96 * 2]; // Max size: 64 * 96 * 2 = 12,288 bytes = 12KB

    uint32_t out_pos = 0;
    uint32_t in_pos = 0;

    while (out_pos < output_size)
    {
        uint8_t count = compressed[in_pos++];

        if (count == 0)  // Literal run
        {
            uint8_t literal_count = compressed[in_pos++];

            // Copy literal pixels (2 bytes each)
            for (uint8_t i = 0; i < literal_count; i++)
            {
                if (out_pos >= output_size) break;
                output[out_pos++] = compressed[in_pos++];  // Low byte
                output[out_pos++] = compressed[in_pos++];  // High byte
            }
        }
        else  // Repeated pixel
        {
            uint8_t pixel_low = compressed[in_pos++];
            uint8_t pixel_high = compressed[in_pos++];

            // Repeat pixel 'count' times
            for (uint8_t i = 0; i < count; i++)
            {
                if (out_pos >= output_size) break;
                output[out_pos++] = pixel_low;
                output[out_pos++] = pixel_high;
            }
        }
    }

    return output;
}
"""

def write_map_header(output_file:Path):
    """
    Write C header file with address mapping.

    Args:
        output_file: Output header file path
    """

    # C Header File
    with open(output_file, 'w') as file:
        file.write(__HEADER__)
        file.write(__FOOTER__)

    # C Code File
    output_c_file = output_file.__str__().replace('.h','.c')
    with open(output_c_file, 'w') as file:
        file.write("#include <stddef.h>\n")
        file.write("#include <stdio.h>\n")
        file.write(f"""#include "{output_file.name}"\n\n""")

        # Write binary search lookup function
        file.write(__FUNCTIONS__)

__INFO__ = """Usage: merge_icons_to_bin.py <input_dir> <output.bin> <output_map.h>

Example:
    .venv/bin/python3.12 colour-sprites/rle_raw_pixels.py images/output/picowalker_rp2xxx_color_rle.h
"""

# =============================================================================
# Run main
# =============================================================================
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(__INFO__)
        sys.exit(1)

    output = Path(sys.argv[1])

    # Write C header map file
    write_map_header(output)
    print(f"Wrote to {output}")