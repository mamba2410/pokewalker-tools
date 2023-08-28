#ifndef PW_IR_H
#define PW_IR_H

#include <stdint.h>
#include <stddef.h>

/// @file ir/ir.h

#define CMD_EEPROM_READ_REQ     0x0c
#define CMD_EEPROM_READ_RSP     0x0e
#define CMD_EEPROM_WRITE_CMP_00 0x00    // eeprom write compressed
#define CMD_EEPROM_WRITE_RAW_00 0x02    // eeprom write directly
#define CMD_EEPROM_WRITE_CMP_80 0x80    // eeprom write compressed
#define CMD_EEPROM_WRITE_RAW_80 0x82    // eeprom write directly
#define CMD_EEPROM_WRITE_RND    0x0a    // eeprom write random length
#define CMD_EEPROM_WRITE_ACK    0x04

#define CMD_PEER_PLAY_START     0x10
#define CMD_PEER_PLAY_RSP       0x12
#define CMD_PEER_PLAY_DX        0x14    // peer_play_data_t exchange
#define CMD_PEER_PLAY_END       0x16
#define CMD_PEER_PLAY_SEEN      0x1c    // sent if peer was seen too recently

#define CMD_IDENTITY_REQ        0x20    // game requesting identity data
#define CMD_IDENTITY_RSP        0x22
#define CMD_IDENTITY_SEND       0x32    // master's identity to slave (used in first walk)
#define CMD_IDENTITY_ACK        0x34
#define CMD_IDENTITY_SEND_ALIAS1 0x40    // alias for 0x32
#define CMD_IDENTITY_ACK_ALIAS1  0x42    // resp for 0x42
#define CMD_IDENTITY_SEND_ALIAS2 0x52    // alias for 0x32
#define CMD_IDENTITY_ACK_ALIAS2  0x54    // resp for 0x52
#define CMD_IDENTITY_SEND_ALIAS3 0x60    // alias for 0x32 (used in item dump)
#define CMD_IDENTITY_ACK_ALIAS3  0x62    // resp for 0x60

#define CMD_PING                0x24
#define CMD_PONG                0x26
#define CMD_NOCOMPLETE          0x36
#define CMD_NOCOMPLETE_ALIAS1   0x56
#define CMD_NOCOMPLETE_ALIAS2   0x64
#define CMD_NOCOMPLETE_ALIAS3   0x44 // only found from g2w
#define CMD_NORX                0x9c
#define CMD_NORX_ACK            0x9e
#define CMD_CONNECT_COMPLETE    0x66
#define CMD_CONNECT_COMPLETE_ACK 0x68
#define CMD_ADVERTISING         0xfc
#define CMD_DISCONNECT          0xf4
#define CMD_SLAVE_ACK           0xf8
#define CMD_ASSERT_MASTER       0xfa

#define CMD_WALKER_RESET_0      0x2c    // don't clear events or lifetime
#define CMD_WALKER_RESET_1      0x2a    // recv this from game, clear events only
#define CMD_WALKER_RESET_3      0xe0    // clear events + lifetime
#define CMD_WALK_END_REQ        0x4e
#define CMD_WALK_END_ACK        0x50
#define CMD_WALK_START          0x5a
#define CMD_WALK_START_INIT     0x38

#define CMD_EVENT_MAP           0xc0
#define CMD_EVENT_POKEMON       0xc2
#define CMD_EVENT_ITEM          0xc4
#define CMD_EVENT_ROUTE         0xc6

#define CMD_EVENT_MAP_STAMPS           0xd0
#define CMD_EVENT_POKEMON_STAMPS       0xd2
#define CMD_EVENT_ITEM_STAMPS          0xd4
#define CMD_EVENT_ROUTE_STAMPS         0xd6

#define CMD_RAM_WRITE   0x06

#define EXTRA_FROM_MASTER       0x01
#define EXTRA_FROM_SLAVE        0x02
#define EXTRA_BYTE_FROM_WALKER  0x01
#define EXTRA_BYTE_TO_WALKER    0x02

#define MAX_ADVERTISING_PACKETS 20

#define MAX_PACKET_SIZE (128+8)

#endif /* PW_IR_H */

