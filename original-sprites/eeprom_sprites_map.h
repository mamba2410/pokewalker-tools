/**
 * Pokéwalker EEPROM Memory Map
 *
 * Based on DmitryGR's reverse engineering work
 * Reference: https://dmitry.gr/?r=05.Projects&proj=28.+pokewalker
 *
 * EEPROM Size: 64KB (0x10000 bytes)
 * Sprite Format: 2-bit grayscale (4 colors)
 * Storage: uint16 little-endian, organized in 8-row blocks
 */

#ifndef POKEWALKER_EEPROM_MAP_H
#define POKEWALKER_EEPROM_MAP_H

#include <stdint.h>

/* Sprite calculation macro */
#define SPRITE_SIZE(width, height) ((width) * (height) / 4)
#define ANIMATED_SPRITE_SIZE(width, height, frames) ((width) * (height) * (frames) / 4)

/* ============================================================================
 * NUMERIC CHARACTERS AND SYMBOLS
 * ============================================================================ */

/* Numeric character images: "0123456789:-/" */
#define EEPROM_DIGITS_START         0x0280
#define EEPROM_DIGITS_END           0x041F
#define DIGIT_WIDTH                 8
#define DIGIT_HEIGHT                16
#define DIGIT_SIZE                  SPRITE_SIZE(8, 16)  /* 32 bytes */
#define NUM_DIGITS                  13

/* Individual digit offsets */
#define EEPROM_DIGIT_0              0x0280
#define EEPROM_DIGIT_1              0x02A0
#define EEPROM_DIGIT_2              0x02C0
#define EEPROM_DIGIT_3              0x02E0
#define EEPROM_DIGIT_4              0x0300
#define EEPROM_DIGIT_5              0x0320
#define EEPROM_DIGIT_6              0x0340
#define EEPROM_DIGIT_7              0x0360
#define EEPROM_DIGIT_8              0x0380
#define EEPROM_DIGIT_9              0x03A0
#define EEPROM_DIGIT_COLON          0x03C0  /* ':' */
#define EEPROM_DIGIT_DASH           0x03E0  /* '-' */
#define EEPROM_DIGIT_SLASH          0x0400  /* '/' */

/* WATT symbol */
#define EEPROM_WATT_SYMBOL          0x0420
#define WATT_WIDTH                  16
#define WATT_HEIGHT                 16
#define WATT_SIZE                   SPRITE_SIZE(16, 16)  /* 64 bytes */

/* ============================================================================
 * ICONS AND SYMBOLS (8x8)
 * ============================================================================ */

#define ICON_WIDTH                  8
#define ICON_HEIGHT                 8
#define ICON_SIZE                   SPRITE_SIZE(8, 8)  /* 16 bytes */

#define EEPROM_POKEBALL             0x0460
#define EEPROM_POKEBALL_GREY        0x0470
#define EEPROM_ITEM                 0x0488
#define EEPROM_ITEM_GREY            0x0498
#define EEPROM_MAP_ICON             0x04A8

/* Card suit symbols (8x8 each) */
#define EEPROM_CARD_SUITS_START     0x04B8
#define EEPROM_CARD_HEART           0x04B8
#define EEPROM_CARD_SPADE           0x04C8
#define EEPROM_CARD_DIAMOND         0x04D8
#define EEPROM_CARD_CLUB            0x04E8

/* ============================================================================
 * ARROW SYMBOLS
 * ============================================================================ */

/* Directional arrows (8x8 each, 3 variants: normal, offset, inverted) */
#define EEPROM_ARROWS_START         0x04F8
#define ARROW_VARIANT_SIZE          SPRITE_SIZE(8, 8)  /* 16 bytes */
#define ARROW_VARIANTS              12

#define EEPROM_ARROW_UP_NORMAL      0x04F8
#define EEPROM_ARROW_UP_OFFSET      0x0508
#define EEPROM_ARROW_UP_INVERT      0x0518
#define EEPROM_ARROW_DOWN_NORMAL    0x0528
#define EEPROM_ARROW_DOWN_OFFSET    0x0538
#define EEPROM_ARROW_DOWN_INVERT    0x0548
#define EEPROM_ARROW_LEFT_NORMAL    0x0558
#define EEPROM_ARROW_LEFT_OFFSET    0x0568
#define EEPROM_ARROW_LEFT_INVERT    0x0578
#define EEPROM_ARROW_RIGHT_NORMAL   0x0588
#define EEPROM_ARROW_RIGHT_OFFSET   0x0598
#define EEPROM_ARROW_RIGHT_INVERT   0x05A8

/* Menu navigation arrows (8x16 each) */
#define MENU_ARROW_WIDTH            8
#define MENU_ARROW_HEIGHT           16
#define MENU_ARROW_SIZE             SPRITE_SIZE(8, 16)  /* 32 bytes */

#define EEPROM_MENU_ARROW_LEFT      0x05B8
#define EEPROM_MENU_ARROW_RIGHT     0x05D8
#define EEPROM_MENU_ARROW_RETURN    0x05F8

/* ============================================================================
 * STATUS AND COMMUNICATION ICONS
 * ============================================================================ */

/* Low battery icon (8x8) */
#define EEPROM_LOW_BATTERY          0x0660
#define LOW_BATTERY_SIZE            SPRITE_SIZE(8, 8)  /* 16 bytes */

/* Talk bubbles with emotion icons (24x16 each, 6 variants) */
#define EEPROM_TALK_FACES_START     0x0670
#define TALK_FACE_WIDTH             24
#define TALK_FACE_HEIGHT            16
#define TALK_FACE_SIZE              SPRITE_SIZE(24, 16)  /* 96 bytes */
#define TALK_FACE_COUNT             6

#define EEPROM_TALK_FACE_EXCLAIM    0x0670
#define EEPROM_TALK_FACE_HEART      0x06D0
#define EEPROM_TALK_FACE_MUSIC      0x0730
#define EEPROM_TALK_FACE_SMILE      0x0790
#define EEPROM_TALK_FACE_NEUTRAL    0x07F0
#define EEPROM_TALK_FACE_ELLIPSIS   0x0850

/* Large talk bubble with exclamation (24x16) */
#define EEPROM_TALK_EXCLAMATION     0x08B0
#define TALK_EXCLAMATION_SIZE       SPRITE_SIZE(24, 16)  /* 96 bytes */

/* ============================================================================
 * MENU TITLES (80x16 EACH)
 * ============================================================================ */

#define MENU_TITLE_WIDTH            80
#define MENU_TITLE_HEIGHT           16
#define MENU_TITLE_SIZE             SPRITE_SIZE(80, 16)  /* 320 bytes */

#define EEPROM_MENU_TITLE_POKERADAR 0x0910
#define EEPROM_MENU_TITLE_DOWSING   0x0A50
#define EEPROM_MENU_TITLE_CONNECT   0x0B90
#define EEPROM_MENU_TITLE_TRAINER_CARD 0x0CD0
#define EEPROM_MENU_TITLE_INVENTORY 0x0E10
#define EEPROM_MENU_TITLE_SETTINGS  0x0F50

/* ============================================================================
 * MENU ICONS (16x16 EACH)
 * ============================================================================ */

#define MENU_ICON_WIDTH             16
#define MENU_ICON_HEIGHT            16
#define MENU_ICON_SIZE              SPRITE_SIZE(16, 16)  /* 64 bytes */

#define EEPROM_MENU_ICON_POKERADAR  0x1090
#define EEPROM_MENU_ICON_DOWSING    0x10D0
#define EEPROM_MENU_ICON_CONNECT    0x1110
#define EEPROM_MENU_ICON_TRAINER_CARD 0x1150
#define EEPROM_MENU_ICON_INVENTORY  0x1190
#define EEPROM_MENU_ICON_SETTINGS   0x11D0

/* ============================================================================
 * TRAINER CARD AND SETTINGS UI
 * ============================================================================ */

/* Person icon (16x16) */
#define EEPROM_PERSON_ICON          0x1210
#define PERSON_ICON_SIZE            SPRITE_SIZE(16, 16)  /* 64 bytes */

/* Trainer name (80x16) */
#define EEPROM_TRAINER_NAME         0x1250
#define TRAINER_NAME_SIZE           SPRITE_SIZE(80, 16)  /* 320 bytes */

/* Small route image (16x16) */
#define EEPROM_ROUTE_SMALL          0x1390
#define ROUTE_SMALL_SIZE            SPRITE_SIZE(16, 16)  /* 64 bytes */

/* Stats frames for trainer card */
#define EEPROM_STEPS_FRAME          0x13D0
#define STEPS_FRAME_SIZE            SPRITE_SIZE(40, 16)  /* 160 bytes */

#define EEPROM_TIME_FRAME           0x1470
#define TIME_FRAME_SIZE             SPRITE_SIZE(32, 16)  /* 128 bytes */

#define EEPROM_DAYS_FRAME           0x14F0
#define DAYS_FRAME_SIZE             SPRITE_SIZE(40, 16)  /* 160 bytes */

#define EEPROM_TOTAL_DAYS_FRAME     0x1590
#define TOTAL_DAYS_FRAME_SIZE       SPRITE_SIZE(64, 16)  /* 256 bytes */

/* Settings screen frames */
#define EEPROM_SOUND_FRAME          0x1690
#define SOUND_FRAME_SIZE            SPRITE_SIZE(40, 16)  /* 160 bytes */

#define EEPROM_SHADE_FRAME          0x1730
#define SHADE_FRAME_SIZE            SPRITE_SIZE(40, 16)  /* 160 bytes */

/* Speaker icons (24x16 each) */
#define SPEAKER_WIDTH               24
#define SPEAKER_HEIGHT              16
#define SPEAKER_SIZE                SPRITE_SIZE(24, 16)  /* 96 bytes */

#define EEPROM_SPEAKER_OFF          0x17D0
#define EEPROM_SPEAKER_LOW          0x1830
#define EEPROM_SPEAKER_HIGH         0x1890

/* Contrast demonstrator (8x16) */
#define EEPROM_CONTRAST_DEMO        0x18F0
#define CONTRAST_DEMO_SIZE          SPRITE_SIZE(8, 16)  /* 32 bytes */

/* ============================================================================
 * ITEM SPRITES
 * ============================================================================ */

/* Large item icons (32x24 each) */
#define ITEM_LARGE_WIDTH            32
#define ITEM_LARGE_HEIGHT           24
#define ITEM_LARGE_SIZE             SPRITE_SIZE(32, 24)  /* 192 bytes */

#define EEPROM_TREASURE_LARGE       0x1910
#define EEPROM_MAP_LARGE            0x19D0
#define EEPROM_PRESENT_LARGE        0x1A90

/* ============================================================================
 * DOWSING AND RADAR GAME ELEMENTS
 * ============================================================================ */

/* Dowsing bushes (16x16 each) */
#define BUSH_SMALL_WIDTH            16
#define BUSH_SMALL_HEIGHT           16
#define BUSH_SMALL_SIZE             SPRITE_SIZE(16, 16)  /* 64 bytes */

#define EEPROM_DOWSING_BUSH_DARK    0x1B50
#define EEPROM_DOWSING_BUSH_LIGHT   0x1B90

/* Radar bush (32x24) */
#define EEPROM_RADAR_BUSH           0x1CB0
#define RADAR_BUSH_SIZE             SPRITE_SIZE(32, 24)  /* 192 bytes */

/* Radar exclamation bubbles (16x16 each) */
#define RADAR_BUBBLE_WIDTH          16
#define RADAR_BUBBLE_HEIGHT         16
#define RADAR_BUBBLE_SIZE           SPRITE_SIZE(16, 16)  /* 64 bytes */

#define EEPROM_RADAR_BUBBLE_ONE     0x1D70
#define EEPROM_RADAR_BUBBLE_TWO     0x1DB0
#define EEPROM_RADAR_BUBBLE_THREE   0x1DF0

/* Radar click effect (16x16) */
#define EEPROM_RADAR_CLICK          0x1E30
#define RADAR_CLICK_SIZE            SPRITE_SIZE(16, 16)  /* 64 bytes */

/* Attack effects (16x32 each) */
#define ATTACK_EFFECT_WIDTH         16
#define ATTACK_EFFECT_HEIGHT        32
#define ATTACK_EFFECT_SIZE          SPRITE_SIZE(16, 32)  /* 128 bytes */

#define EEPROM_RADAR_ATTACK_HIT     0x1E70
#define EEPROM_RADAR_CRITICAL_HIT   0x1EF0

/* Pokemon appear cloud (32x24) */
#define EEPROM_RADAR_APPEAR_CLOUD   0x1F70
#define APPEAR_CLOUD_SIZE           SPRITE_SIZE(32, 24)  /* 192 bytes */

/* HP bar segment (8x8) */
#define EEPROM_RADAR_HP_BLIP        0x2030
#define HP_BLIP_SIZE                SPRITE_SIZE(8, 8)  /* 16 bytes */

/* Catch effect star (8x8) */
#define EEPROM_RADAR_CATCH_EFFECT   0x2040
#define CATCH_EFFECT_SIZE           SPRITE_SIZE(8, 8)  /* 16 bytes */

/* ============================================================================
 * MISCELLANEOUS ICONS
 * ============================================================================ */

/* Pokewalker device image (32x32) */
#define EEPROM_POKEWALKER_BIG       0x2350
#define POKEWALKER_BIG_SIZE         SPRITE_SIZE(32, 32)  /* 256 bytes */

/* IR transmission arcs (8x16) */
#define EEPROM_IR_ARCS              0x2450
#define IR_ARCS_SIZE                SPRITE_SIZE(8, 16)  /* 32 bytes */

/* Music note (8x8) */
#define EEPROM_MUSIC_NOTE           0x2470
#define MUSIC_NOTE_SIZE             SPRITE_SIZE(8, 8)  /* 16 bytes */

/* Hours frame (40x16) - appears unused */
#define EEPROM_HOURS_FRAME          0x2490
#define HOURS_FRAME_SIZE            SPRITE_SIZE(40, 16)  /* 160 bytes */

/* ============================================================================
 * ROUTE SPRITES
 * ============================================================================ */

/* Current route large image (32x24) */
#define EEPROM_ROUTE_LARGE          0x8FBE
#define ROUTE_LARGE_SIZE            SPRITE_SIZE(32, 24)  /* 192 bytes */

/* ============================================================================
 * ANIMATED POKEMON SPRITES
 * ============================================================================ */

/* Small animated sprite (32x24, 2 frames) */
#define EEPROM_POKE_SMALL_ANIM      0x91BE
#define POKE_SMALL_WIDTH            32
#define POKE_SMALL_HEIGHT           24
#define POKE_SMALL_FRAMES           2
#define POKE_SMALL_SIZE             ANIMATED_SPRITE_SIZE(32, 24, 2)  /* 384 bytes */

/* Large animated sprite for main screen (64x48, 2 frames) */
#define EEPROM_POKE_LARGE_ANIM      0x933E
#define POKE_LARGE_WIDTH            64
#define POKE_LARGE_HEIGHT           48
#define POKE_LARGE_FRAMES           2
#define POKE_LARGE_SIZE             ANIMATED_SPRITE_SIZE(64, 48, 2)  /* 1536 bytes */

/* What about Variants that can be found in the route info of your pokemon? 0x8f00 */

// Variant Address 0x8f0d
// Pikachu Female (0x20) 32 
// Pikachu Male   (0x00) 0

/* ============================================================================
 * ROUTE POKEMON SPRITES
 * ============================================================================ */

/* Route available pokemon (3 pokemon, 32x24, 2 frames each) */
#define EEPROM_ROUTE_POKE_START     0x9A7E
#define ROUTE_POKE_WIDTH            32
#define ROUTE_POKE_HEIGHT           24
#define ROUTE_POKE_FRAMES           2
#define ROUTE_POKE_COUNT            3
#define ROUTE_POKE_SINGLE_SIZE      ANIMATED_SPRITE_SIZE(32, 24, 2)  /* 384 bytes */
#define ROUTE_POKE_TOTAL_SIZE       (ROUTE_POKE_SINGLE_SIZE * 3)     /* 1152 bytes */

/* Individual route pokemon offsets */
#define EEPROM_ROUTE_POKE_1         0x9A7E
#define EEPROM_ROUTE_POKE_2         (0x9A7E + ROUTE_POKE_SINGLE_SIZE)
#define EEPROM_ROUTE_POKE_3         (0x9A7E + ROUTE_POKE_SINGLE_SIZE * 2)

/* Third route pokemon large sprite (64x48, 2 frames) */
#define EEPROM_ROUTE_POKE_3_LARGE   0x9EFE
#define ROUTE_POKE_3_LARGE_SIZE     ANIMATED_SPRITE_SIZE(64, 48, 2)  /* 1536 bytes */

/* ============================================================================
 * EVENT POKEMON SPRITES
 * ============================================================================ */

/* Event pokemon small sprite (32x24, 2 frames) */
#define EEPROM_EVENT_POKE_SMALL     0xBA80
#define EVENT_POKE_WIDTH            32
#define EVENT_POKE_HEIGHT           24
#define EVENT_POKE_FRAMES           2
#define EVENT_POKE_SIZE             ANIMATED_SPRITE_SIZE(32, 24, 2)  /* 384 bytes */

/* Special route pokemon sprite (32x24, 2 frames) - Note: unusual size of 368 bytes instead of 384 */
#define EEPROM_SPECIAL_POKE         0xBF7C
#define SPECIAL_POKE_WIDTH          32
#define SPECIAL_POKE_HEIGHT         24
#define SPECIAL_POKE_FRAMES         2
#define SPECIAL_POKE_SIZE           368  /* Unusually 368 bytes instead of expected 384 */
#define SPECIAL_POKE_FRAME_SIZE     172  /* 172 bytes per frame instead of 192 */

/* Special route large image (32x24) */
#define EEPROM_SPECIAL_ROUTE_LARGE  0xC83C
#define SPECIAL_ROUTE_LARGE_SIZE    SPRITE_SIZE(32, 24)  /* 192 bytes */

/* ============================================================================
 * PEER PLAY SPRITES
 * ============================================================================ */

/* Peer play pokemon sprite (32x24, 2 frames) */
#define EEPROM_PEER_POKE            0xF400
#define PEER_POKE_WIDTH             32
#define PEER_POKE_HEIGHT            24
#define PEER_POKE_FRAMES            2
#define PEER_POKE_SIZE              ANIMATED_SPRITE_SIZE(32, 24, 2)  /* 384 bytes */

/* ============================================================================
 * SPRITE STRUCTURE DEFINITIONS
 * ============================================================================ */

typedef struct {
    uint16_t offset;        /* EEPROM offset */
    uint8_t width;          /* Sprite width in pixels */
    uint8_t height;         /* Sprite height in pixels */
    uint8_t frames;         /* Number of animation frames */
    uint16_t size;          /* Total size in bytes */
    const char* name;       /* Sprite description */
} sprite_info_t;

/* ============================================================================
 * SPRITE DIRECTORY
 * ============================================================================ */

static const sprite_info_t SPRITE_DIRECTORY[] = {
    /* Digits */
    {EEPROM_DIGIT_0,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 0"},
    {EEPROM_DIGIT_1,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 1"},
    {EEPROM_DIGIT_2,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 2"},
    {EEPROM_DIGIT_3,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 3"},
    {EEPROM_DIGIT_4,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 4"},
    {EEPROM_DIGIT_5,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 5"},
    {EEPROM_DIGIT_6,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 6"},
    {EEPROM_DIGIT_7,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 7"},
    {EEPROM_DIGIT_8,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 8"},
    {EEPROM_DIGIT_9,            DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit 9"},
    {EEPROM_DIGIT_COLON,        DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit :"},
    {EEPROM_DIGIT_DASH,         DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit -"},
    {EEPROM_DIGIT_SLASH,        DIGIT_WIDTH, DIGIT_HEIGHT, 1, DIGIT_SIZE, "Digit /"},

    /* Symbols */
    {EEPROM_WATT_SYMBOL,        WATT_WIDTH, WATT_HEIGHT, 1, WATT_SIZE, "WATT Symbol"},
    {EEPROM_POKEBALL,           ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Pokeball"},
    {EEPROM_POKEBALL_GREY,      ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Pokeball Grey"},
    {EEPROM_ITEM,               ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Item"},
    {EEPROM_ITEM_GREY,          ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Item Grey"},
    {EEPROM_MAP_ICON,           ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Map Icon"},
    {EEPROM_CARD_HEART,         ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Card Heart"},
    {EEPROM_CARD_SPADE,         ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Card Spade"},
    {EEPROM_CARD_DIAMOND,       ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Card Diamond"},
    {EEPROM_CARD_CLUB,          ICON_WIDTH, ICON_HEIGHT, 1, ICON_SIZE, "Card Club"},

    /* Arrows */
    {EEPROM_ARROW_UP_NORMAL,    8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Up Normal"},
    {EEPROM_ARROW_UP_OFFSET,    8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Up Offset"},
    {EEPROM_ARROW_UP_INVERT,    8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Up Invert"},
    {EEPROM_ARROW_DOWN_NORMAL,  8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Down Normal"},
    {EEPROM_ARROW_DOWN_OFFSET,  8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Down Offset"},
    {EEPROM_ARROW_DOWN_INVERT,  8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Down Invert"},
    {EEPROM_ARROW_LEFT_NORMAL,  8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Left Normal"},
    {EEPROM_ARROW_LEFT_OFFSET,  8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Left Offset"},
    {EEPROM_ARROW_LEFT_INVERT,  8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Left Invert"},
    {EEPROM_ARROW_RIGHT_NORMAL, 8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Right Normal"},
    {EEPROM_ARROW_RIGHT_OFFSET, 8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Right Offset"},
    {EEPROM_ARROW_RIGHT_INVERT, 8, 8, 1, ARROW_VARIANT_SIZE, "Arrow Right Invert"},
    {EEPROM_MENU_ARROW_LEFT,    MENU_ARROW_WIDTH, MENU_ARROW_HEIGHT, 1, MENU_ARROW_SIZE, "Menu Arrow Left"},
    {EEPROM_MENU_ARROW_RIGHT,   MENU_ARROW_WIDTH, MENU_ARROW_HEIGHT, 1, MENU_ARROW_SIZE, "Menu Arrow Right"},
    {EEPROM_MENU_ARROW_RETURN,  MENU_ARROW_WIDTH, MENU_ARROW_HEIGHT, 1, MENU_ARROW_SIZE, "Menu Arrow Return"},

    /* Status & Communication */
    {EEPROM_LOW_BATTERY,        8, 8, 1, LOW_BATTERY_SIZE, "Low Battery"},
    {EEPROM_TALK_FACE_EXCLAIM,  TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_FACE_SIZE, "Talk Face Exclaim"},
    {EEPROM_TALK_FACE_HEART,    TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_FACE_SIZE, "Talk Face Heart"},
    {EEPROM_TALK_FACE_MUSIC,    TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_FACE_SIZE, "Talk Face Music"},
    {EEPROM_TALK_FACE_SMILE,    TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_FACE_SIZE, "Talk Face Smile"},
    {EEPROM_TALK_FACE_NEUTRAL,  TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_FACE_SIZE, "Talk Face Neutral"},
    {EEPROM_TALK_FACE_ELLIPSIS, TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_FACE_SIZE, "Talk Face Ellipsis"},
    {EEPROM_TALK_EXCLAMATION,   TALK_FACE_WIDTH, TALK_FACE_HEIGHT, 1, TALK_EXCLAMATION_SIZE, "Talk Exclamation"},

    /* Menu Titles */
    {EEPROM_MENU_TITLE_POKERADAR, MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT, 1, MENU_TITLE_SIZE, "Menu Title PokeRadar"},
    {EEPROM_MENU_TITLE_DOWSING,   MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT, 1, MENU_TITLE_SIZE, "Menu Title Dowsing"},
    {EEPROM_MENU_TITLE_CONNECT,   MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT, 1, MENU_TITLE_SIZE, "Menu Title Connect"},
    {EEPROM_MENU_TITLE_TRAINER_CARD, MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT, 1, MENU_TITLE_SIZE, "Menu Title Trainer Card"},
    {EEPROM_MENU_TITLE_INVENTORY, MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT, 1, MENU_TITLE_SIZE, "Menu Title Inventory"},
    {EEPROM_MENU_TITLE_SETTINGS,  MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT, 1, MENU_TITLE_SIZE, "Menu Title Settings"},

    /* Menu Icons */
    {EEPROM_MENU_ICON_POKERADAR,  MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 1, MENU_ICON_SIZE, "Menu Icon PokeRadar"},
    {EEPROM_MENU_ICON_DOWSING,    MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 1, MENU_ICON_SIZE, "Menu Icon Dowsing"},
    {EEPROM_MENU_ICON_CONNECT,    MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 1, MENU_ICON_SIZE, "Menu Icon Connect"},
    {EEPROM_MENU_ICON_TRAINER_CARD, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 1, MENU_ICON_SIZE, "Menu Icon Trainer Card"},
    {EEPROM_MENU_ICON_INVENTORY,  MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 1, MENU_ICON_SIZE, "Menu Icon Inventory"},
    {EEPROM_MENU_ICON_SETTINGS,   MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 1, MENU_ICON_SIZE, "Menu Icon Settings"},

    /* Trainer Card & Settings */
    {EEPROM_PERSON_ICON,        16, 16, 1, PERSON_ICON_SIZE, "Person Icon"},
    {EEPROM_TRAINER_NAME,       80, 16, 1, TRAINER_NAME_SIZE, "Trainer Name"},
    {EEPROM_ROUTE_SMALL,        16, 16, 1, ROUTE_SMALL_SIZE, "Route Small"},
    {EEPROM_STEPS_FRAME,        40, 16, 1, STEPS_FRAME_SIZE, "Steps Frame"},
    {EEPROM_TIME_FRAME,         32, 16, 1, TIME_FRAME_SIZE, "Time Frame"},
    {EEPROM_DAYS_FRAME,         40, 16, 1, DAYS_FRAME_SIZE, "Days Frame"},
    {EEPROM_TOTAL_DAYS_FRAME,   64, 16, 1, TOTAL_DAYS_FRAME_SIZE, "Total Days Frame"},
    {EEPROM_SOUND_FRAME,        40, 16, 1, SOUND_FRAME_SIZE, "Sound Frame"},
    {EEPROM_SHADE_FRAME,        40, 16, 1, SHADE_FRAME_SIZE, "Shade Frame"},
    {EEPROM_SPEAKER_OFF,        SPEAKER_WIDTH, SPEAKER_HEIGHT, 1, SPEAKER_SIZE, "Speaker Off"},
    {EEPROM_SPEAKER_LOW,        SPEAKER_WIDTH, SPEAKER_HEIGHT, 1, SPEAKER_SIZE, "Speaker Low"},
    {EEPROM_SPEAKER_HIGH,       SPEAKER_WIDTH, SPEAKER_HEIGHT, 1, SPEAKER_SIZE, "Speaker High"},
    {EEPROM_CONTRAST_DEMO,      8, 16, 1, CONTRAST_DEMO_SIZE, "Contrast Demo"},

    /* Items */
    {EEPROM_TREASURE_LARGE,     ITEM_LARGE_WIDTH, ITEM_LARGE_HEIGHT, 1, ITEM_LARGE_SIZE, "Treasure Large"},
    {EEPROM_MAP_LARGE,          ITEM_LARGE_WIDTH, ITEM_LARGE_HEIGHT, 1, ITEM_LARGE_SIZE, "Map Large"},
    {EEPROM_PRESENT_LARGE,      ITEM_LARGE_WIDTH, ITEM_LARGE_HEIGHT, 1, ITEM_LARGE_SIZE, "Present Large"},

    /* Dowsing & Radar */
    {EEPROM_DOWSING_BUSH_DARK,  BUSH_SMALL_WIDTH, BUSH_SMALL_HEIGHT, 1, BUSH_SMALL_SIZE, "Dowsing Bush Dark"},
    {EEPROM_DOWSING_BUSH_LIGHT, BUSH_SMALL_WIDTH, BUSH_SMALL_HEIGHT, 1, BUSH_SMALL_SIZE, "Dowsing Bush Light"},
    {EEPROM_RADAR_BUSH,         32, 24, 1, RADAR_BUSH_SIZE, "Radar Bush"},
    {EEPROM_RADAR_BUBBLE_ONE,   RADAR_BUBBLE_WIDTH, RADAR_BUBBLE_HEIGHT, 1, RADAR_BUBBLE_SIZE, "Radar Bubble One"},
    {EEPROM_RADAR_BUBBLE_TWO,   RADAR_BUBBLE_WIDTH, RADAR_BUBBLE_HEIGHT, 1, RADAR_BUBBLE_SIZE, "Radar Bubble Two"},
    {EEPROM_RADAR_BUBBLE_THREE, RADAR_BUBBLE_WIDTH, RADAR_BUBBLE_HEIGHT, 1, RADAR_BUBBLE_SIZE, "Radar Bubble Three"},
    {EEPROM_RADAR_CLICK,        RADAR_BUBBLE_WIDTH, RADAR_BUBBLE_HEIGHT, 1, RADAR_CLICK_SIZE, "Radar Click"},
    {EEPROM_RADAR_ATTACK_HIT,   ATTACK_EFFECT_WIDTH, ATTACK_EFFECT_HEIGHT, 1, ATTACK_EFFECT_SIZE, "Radar Attack Hit"},
    {EEPROM_RADAR_CRITICAL_HIT, ATTACK_EFFECT_WIDTH, ATTACK_EFFECT_HEIGHT, 1, ATTACK_EFFECT_SIZE, "Radar Critical Hit"},
    {EEPROM_RADAR_APPEAR_CLOUD, 32, 24, 1, APPEAR_CLOUD_SIZE, "Radar Appear Cloud"},
    {EEPROM_RADAR_HP_BLIP,      8, 8, 1, HP_BLIP_SIZE, "Radar HP Blip"},
    {EEPROM_RADAR_CATCH_EFFECT, 8, 8, 1, CATCH_EFFECT_SIZE, "Radar Catch Effect"},

    /* Misc Icons */
    {EEPROM_POKEWALKER_BIG,     32, 32, 1, POKEWALKER_BIG_SIZE, "Pokewalker Big"},
    {EEPROM_IR_ARCS,            8, 16, 1, IR_ARCS_SIZE, "IR Arcs"},
    {EEPROM_MUSIC_NOTE,         8, 8, 1, MUSIC_NOTE_SIZE, "Music Note"},
    {EEPROM_HOURS_FRAME,        40, 16, 1, HOURS_FRAME_SIZE, "Hours Frame"},

    /* Routes */
    {EEPROM_ROUTE_LARGE,        32, 24, 1, ROUTE_LARGE_SIZE, "Route Large"},

    /* Pokemon sprites */
    {EEPROM_POKE_SMALL_ANIM,    POKE_SMALL_WIDTH, POKE_SMALL_HEIGHT, POKE_SMALL_FRAMES, POKE_SMALL_SIZE, "Pokemon Small Animated"},
    {EEPROM_POKE_LARGE_ANIM,    POKE_LARGE_WIDTH, POKE_LARGE_HEIGHT, POKE_LARGE_FRAMES, POKE_LARGE_SIZE, "Pokemon Large Animated"},

    /* Route pokemon */
    {EEPROM_ROUTE_POKE_1,       ROUTE_POKE_WIDTH, ROUTE_POKE_HEIGHT, ROUTE_POKE_FRAMES, ROUTE_POKE_SINGLE_SIZE, "Route Pokemon 1"},
    {EEPROM_ROUTE_POKE_2,       ROUTE_POKE_WIDTH, ROUTE_POKE_HEIGHT, ROUTE_POKE_FRAMES, ROUTE_POKE_SINGLE_SIZE, "Route Pokemon 2"},
    {EEPROM_ROUTE_POKE_3,       ROUTE_POKE_WIDTH, ROUTE_POKE_HEIGHT, ROUTE_POKE_FRAMES, ROUTE_POKE_SINGLE_SIZE, "Route Pokemon 3"},
    {EEPROM_ROUTE_POKE_3_LARGE, POKE_LARGE_WIDTH, POKE_LARGE_HEIGHT, POKE_LARGE_FRAMES, ROUTE_POKE_3_LARGE_SIZE, "Route Pokemon 3 Large"},

    /* Event & Special pokemon */
    {EEPROM_EVENT_POKE_SMALL,   EVENT_POKE_WIDTH, EVENT_POKE_HEIGHT, EVENT_POKE_FRAMES, EVENT_POKE_SIZE, "Event Pokemon Small"},
    {EEPROM_SPECIAL_POKE,       SPECIAL_POKE_WIDTH, SPECIAL_POKE_HEIGHT, SPECIAL_POKE_FRAMES, SPECIAL_POKE_SIZE, "Special Route Pokemon"},
    {EEPROM_SPECIAL_ROUTE_LARGE, 32, 24, 1, SPECIAL_ROUTE_LARGE_SIZE, "Special Route Large"},

    /* Peer play */
    {EEPROM_PEER_POKE,          PEER_POKE_WIDTH, PEER_POKE_HEIGHT, PEER_POKE_FRAMES, PEER_POKE_SIZE, "Peer Play Pokemon"},

    /* Terminator */
    {0, 0, 0, 0, 0, ""}
};

#endif /* POKEWALKER_EEPROM_MAP_H */