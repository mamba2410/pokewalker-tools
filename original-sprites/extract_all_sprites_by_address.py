# -*- coding: utf-8 -*-
"""
Created on 2025-12-6 00:39

@author: David Bahena

Credit: DmitryGR for the decoding algorithm
Reference: https://github.com/UnrealPowerz/pokewalker-eeprom-editor
"""

# Python Standard Library
import os
from struct import unpack

# Third Party Library
from PIL import Image


# Grayscale colors
COLORS = [
    (172, 174, 163),  # Lightest
    (128, 130, 116),
    (92, 92, 82),
    (30, 25, 22)      # Darkest
]

# Sprite directory - all POI sprites with their metadata
SPRITES = [
    # Digits
    (0x0280, 8, 16, 1, "digit_0"),
    (0x02A0, 8, 16, 1, "digit_1"),
    (0x02C0, 8, 16, 1, "digit_2"),
    (0x02E0, 8, 16, 1, "digit_3"),
    (0x0300, 8, 16, 1, "digit_4"),
    (0x0320, 8, 16, 1, "digit_5"),
    (0x0340, 8, 16, 1, "digit_6"),
    (0x0360, 8, 16, 1, "digit_7"),
    (0x0380, 8, 16, 1, "digit_8"),
    (0x03A0, 8, 16, 1, "digit_9"),
    (0x03C0, 8, 16, 1, "digit_colon"),
    (0x03E0, 8, 16, 1, "digit_dash"),
    (0x0400, 8, 16, 1, "digit_slash"),

    # Symbols
    (0x0420, 16, 16, 1, "watt_symbol"),
    (0x0460, 8, 8, 1, "pokeball"),
    (0x0470, 8, 8, 1, "pokeball_grey"),
    (0x0488, 8, 8, 1, "item"),
    (0x0498, 8, 8, 1, "item_grey"),
    (0x04A8, 8, 8, 1, "map_icon"),
    (0x04B8, 8, 8, 1, "card_heart"),
    (0x04C8, 8, 8, 1, "card_spade"),
    (0x04D8, 8, 8, 1, "card_diamond"),
    (0x04E8, 8, 8, 1, "card_club"),

    # Arrows
    (0x04F8, 8, 8, 1, "arrow_up_normal"),
    (0x0508, 8, 8, 1, "arrow_up_offset"),
    (0x0518, 8, 8, 1, "arrow_up_invert"),
    (0x0528, 8, 8, 1, "arrow_down_normal"),
    (0x0538, 8, 8, 1, "arrow_down_offset"),
    (0x0548, 8, 8, 1, "arrow_down_invert"),
    (0x0558, 8, 8, 1, "arrow_left_normal"),
    (0x0568, 8, 8, 1, "arrow_left_offset"),
    (0x0578, 8, 8, 1, "arrow_left_invert"),
    (0x0588, 8, 8, 1, "arrow_right_normal"),
    (0x0598, 8, 8, 1, "arrow_right_offset"),
    (0x05A8, 8, 8, 1, "arrow_right_invert"),
    (0x05B8, 8, 16, 1, "menu_arrow_left"),
    (0x05D8, 8, 16, 1, "menu_arrow_right"),
    (0x05F8, 8, 16, 1, "menu_arrow_return"),

    # Status & Communication
    (0x0660, 8, 8, 1, "low_battery"),
    (0x0670, 24, 16, 1, "talk_face_exclaim"),
    (0x06D0, 24, 16, 1, "talk_face_heart"),
    (0x0730, 24, 16, 1, "talk_face_music"),
    (0x0790, 24, 16, 1, "talk_face_smile"),
    (0x07F0, 24, 16, 1, "talk_face_neutral"),
    (0x0850, 24, 16, 1, "talk_face_ellipsis"),
    (0x08B0, 24, 16, 1, "talk_exclamation"),

    # Menu Titles
    (0x0910, 80, 16, 1, "menu_title_pokeradar"),
    (0x0A50, 80, 16, 1, "menu_title_dowsing"),
    (0x0B90, 80, 16, 1, "menu_title_connect"),
    (0x0CD0, 80, 16, 1, "menu_title_trainer_card"),
    (0x0E10, 80, 16, 1, "menu_title_inventory"),
    (0x0F50, 80, 16, 1, "menu_title_settings"),

    # Menu Icons
    (0x1090, 16, 16, 1, "menu_icon_pokeradar"),
    (0x10D0, 16, 16, 1, "menu_icon_dowsing"),
    (0x1110, 16, 16, 1, "menu_icon_connect"),
    (0x1150, 16, 16, 1, "menu_icon_trainer_card"),
    (0x1190, 16, 16, 1, "menu_icon_inventory"),
    (0x11D0, 16, 16, 1, "menu_icon_settings"),

    # Trainer Card & Settings
    (0x1210, 16, 16, 1, "person_icon"),
    (0x1250, 80, 16, 1, "trainer_name"),
    (0x1390, 16, 16, 1, "route_small"),
    (0x13D0, 40, 16, 1, "steps_frame"),
    (0x1470, 32, 16, 1, "time_frame"),
    (0x14F0, 40, 16, 1, "days_frame"),
    (0x1590, 64, 16, 1, "total_days_frame"),
    (0x1690, 40, 16, 1, "sound_frame"),
    (0x1730, 40, 16, 1, "shade_frame"),
    (0x17D0, 24, 16, 1, "speaker_off"),
    (0x1830, 24, 16, 1, "speaker_low"),
    (0x1890, 24, 16, 1, "speaker_high"),
    (0x18F0, 8, 16, 1, "contrast_demo"),

    # Items
    (0x1910, 32, 24, 1, "treasure_large"),
    (0x19D0, 32, 24, 1, "map_large"),
    (0x1A90, 32, 24, 1, "present_large"),

    # Dowsing & Radar
    (0x1B50, 16, 16, 1, "dowsing_bush_dark"),
    (0x1B90, 16, 16, 1, "dowsing_bush_light"),
    (0x1CB0, 32, 24, 1, "radar_bush"),
    (0x1D70, 16, 16, 1, "radar_bubble_one"),
    (0x1DB0, 16, 16, 1, "radar_bubble_two"),
    (0x1DF0, 16, 16, 1, "radar_bubble_three"),
    (0x1E30, 16, 16, 1, "radar_click"),
    (0x1E70, 16, 32, 1, "radar_attack_hit"),
    (0x1EF0, 16, 32, 1, "radar_critical_hit"),
    (0x1F70, 32, 24, 1, "radar_appear_cloud"),
    (0x2030, 8, 8, 1, "radar_hp_blip"),
    (0x2040, 8, 8, 1, "radar_catch_effect"),

    # Misc Icons
    (0x2350, 32, 32, 1, "pokewalker_big"),
    (0x2450, 8, 16, 1, "ir_arcs"),
    (0x2470, 8, 8, 1, "music_note"),
    (0x2490, 40, 16, 1, "hours_frame"),

    # Routes
    (0x8FBE, 32, 24, 1, "route_large"),

    # Pokemon sprites (animated - 2 frames)
    (0x91BE, 32, 24, 2, "pokemon_small_anim"),
    (0x933E, 64, 48, 2, "pokemon_large_anim"),

    # Route pokemon (animated - 2 frames each)
    (0x9A7E, 32, 24, 2, "route_pokemon_1"),
    (0x9A7E + 192, 32, 24, 2, "route_pokemon_2"),
    (0x9A7E + 384, 32, 24, 2, "route_pokemon_3"),
    (0x9EFE, 64, 48, 2, "route_pokemon_3_large"),

    # Event & Special pokemon
    (0xBA80, 32, 24, 2, "event_pokemon_small"),
    (0xBF7C, 32, 24, 2, "special_pokemon"),
    (0xC83C, 32, 24, 1, "special_route_large"),

    # Peer play
    (0xF400, 32, 24, 2, "peer_pokemon"),
]

def decode_sprite(data:bytes, width:int, height:int) -> Image.Image:
    """
    Decode sprite from binary data to PIL Image.

    Args:
        data: Binary sprite data
        width: Sprite width in pixels
        height: Sprite height in pixels

    Returns:
        PIL Image
    """
    # Convert to uint16 array (little-endian)
    uint16_data = []
    for i in range(0, len(data), 2):
        if i + 1 < len(data):
            value = unpack('<H', data[i:i+2])[0]
            uint16_data.append(value)
        else:
            # Handle odd byte count
            uint16_data.append(data[i])

    # Initialize result bitmap
    bitmap = [0] * (width * height)

    i = 0
    # Process in 8-row blocks
    for y in range(0, height, 8):
        for x in range(width):
            if i >= len(uint16_data):
                break

            current = uint16_data[i]
            i += 1

            # Extract 8 vertical pixels
            for z in range(8):
                if y + z >= height:
                    break

                # Extract 2 bits for this pixel
                # High bit from bit (8+z), low bit from bit z
                o = current >> z
                col = ((o & 0x100) >> 7) | (o & 1)

                bitmap[(y + z) * width + x] = col

    # Convert 2-bit bitmap to PIL Image
    img = Image.new('RGB', (width, height))
    pixels = img.load()

    for y in range(height):
        for x in range(width):
            idx = y * width + x
            if idx < len(bitmap):
                color_idx = bitmap[idx]
                pixels[x, y] = COLORS[color_idx]

    return img
    
if __name__ == '__main__':
    eeprom_path = 'images/resource/eeprom/eeprom_beautiful_beach.bin'
    if not os.path.exists(eeprom_path):
        print(f"Error: {eeprom_path} not found!")
        exit()

    with open(eeprom_path, 'rb') as f:
        eeprom_data = f.read()

    output_dir = 'images/output/sprites_by_address'
    os.makedirs(output_dir, exist_ok=True)

    total_sprites = 0
    for offset, width, height, frames, name in SPRITES:
        try:
            if frames == 1: # Single frame sprite
                filename = f"0x{offset:04X}_{name}.png"
                filepath = os.path.join(output_dir, filename)

                sprite_size = (width * height) // 4
                sprite_data = eeprom_data[offset:offset + sprite_size]
                img = decode_sprite(sprite_data, width, height)
                img.save(filepath)
                total_sprites += 1
                print(f"Extracted: {filename} ({width}x{height})")
            else: # Multi-frame animation
                frame_size = (width * height) // 4
                for frame_idx in range(frames):
                    frame_offset = offset + (frame_idx * frame_size)
                    filename = f"0x{frame_offset:04X}_{name}_frame{frame_idx + 1}.png"
                    filepath = os.path.join(output_dir, filename)

                    sprite_data = eeprom_data[frame_offset:frame_offset + frame_size]
                    img = decode_sprite(sprite_data, width, height)
                    img.save(filepath)
                    total_sprites += 1
                print(f"Extracted: {name} ({width}x{height}, {frames} frames)")

        except Exception as e:
            print(f"Error extracting sprite at 0x{offset:04X} ({name}): {e}")

    print(f"\n✓ Extracted {total_sprites} sprites to {output_dir}/")