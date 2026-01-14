# -*- coding: utf-8 -*-
"""
Created on 2026-01-11 21:10

@author: David Bahena
"""

import sys
from pathlib import Path
from PIL import Image


def crop_pokemon_icon(input_path: Path, output_dir: Path):
    """
    Crop a 32x64 Pokemon icon into a 32x48 image with two 32x24 frames merged.

    Args:
        input_path: Path to input 32x64 PNG file
        output_dir: Directory to save cropped and merged image
    """

    img = Image.open(input_path)
    width, height = img.size

    if width != 32 or height != 64:
        print(f"Warning: {input_path.name} is {width}x{height}, expected 32x64. Skipping.")
        return

    base_name = input_path.stem

    frame1 = img.crop((0, 8, 32, 32))  # x:0-32, y:8-32 (24 pixels tall)
    frame2 = img.crop((0, 40, 32, 64))  # x:0-32, y:40-64 (24 pixels tall)

    merged = Image.new('RGBA', (32, 48), (0, 0, 0, 0))

    merged.paste(frame1, (0, 0))
    merged.paste(frame2, (0, 24))

    output_path = output_dir / f"{base_name}.png"
    merged.save(output_path)

    print(f"✓ {input_path.name} -> {output_path.name} (32x48)")

__INFO__ = """Usage: crop_pokemon_icons.py <input_dir> <output_dir>\n

Example:
    ./.venv/bin/python3.12 crop_pokemon_icons.py images/color_mon_icon images/output/color_pokemon_small
"""
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(__INFO__)
        sys.exit(1)

    input_dir = Path(sys.argv[1])
    output_dir = Path(sys.argv[2])

    # Validate input directory
    if not input_dir.exists():
        print(f"Error: Input directory does not exist: {input_dir}")
        sys.exit(1)

    # Create output directory if it doesn't exist
    output_dir.mkdir(parents=True, exist_ok=True)

    # Find all PNG files
    png_files = list(input_dir.glob("*.png"))

    if not png_files:
        print(f"Error: No PNG files found in {input_dir}")
        sys.exit(1)

    print(f"Found {len(png_files)} PNG files to process")
    print(f"Output directory: {output_dir}")
    print()

    # Process each PNG file
    success_count = 0
    for png_file in sorted(png_files):
        try:
            crop_pokemon_icon(png_file, output_dir)
            success_count += 1
        except Exception as e:
            print(f"✗ Error processing {png_file.name}: {e}")

    print()
    print(f"Complete! Processed {success_count}/{len(png_files)} files")
    print(f"Output: {success_count} merged 32x48 images saved to {output_dir}")