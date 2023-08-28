#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "types.h"
#include "ir.h"

typedef struct {
    uint8_t cmd;
    uint8_t extra;
    uint16_t checksum;
    uint32_t session_id;
} packet_header_t;

static uint32_t g_session_id;

uint32_t swap_u32(uint32_t d) {
    return ( ((d&0x000000ff)<<24) | ((d&0x0000ff00)<<8) | ((d&0x00ff0000)>>8) | ((d&0xff000000)>>24));
}

typedef struct {
    uint16_t start;
    size_t   len;
    bool     is_write;
    bool     in_run;
    size_t   n_packets;
} rw_run_t;

// this got a little more complex than I hoped...
// basically just counts packets which consecutively read/write
// the same area
// returns true if we "handled" the packet (ie don't bother printing the packet)
bool count_run(const size_t len, uint8_t packet[len], rw_run_t *current_run) {
    rw_run_t next_run = {0};

    // super large read/write run tracking
    if(
        packet[0] == CMD_EEPROM_WRITE_CMP_00 ||
        packet[0] == CMD_EEPROM_WRITE_CMP_80 ||
        packet[0] == CMD_EEPROM_WRITE_RAW_00 ||
        packet[0] == CMD_EEPROM_WRITE_RAW_80 ||
        packet[0] == CMD_EEPROM_WRITE_RND ||
        packet[0] == CMD_EEPROM_WRITE_ACK
    ) {
        current_run->n_packets++;
        if(packet[0] == CMD_EEPROM_WRITE_ACK) return true;
        uint16_t addr = (packet[1]<<8) | (packet[0] & 0x80);
        size_t this_len = 0x80;
        if(packet[0] == CMD_EEPROM_WRITE_RND) {
            this_len = len-8;
        }

        if(current_run->start == 0x0000) {
            // we aren't in a run
            current_run->start = addr;
            current_run->len = this_len;
            current_run->is_write = true;
            current_run->in_run = true;
        } else {
            // we are in a run

            if(addr == (current_run->start+current_run->len)) {
                // run continues
                current_run->len += 0x80;
            } else {
                // run breaks but we're writing to an area
                next_run.start = addr;
                next_run.len = 0x80;
                next_run.in_run = true;
                next_run.n_packets = 1;
                next_run.is_write = true;

                current_run->n_packets--; // excluse this packet from last run
                current_run->in_run = false;
            }
        }
    } else if (packet[0] == CMD_EEPROM_READ_REQ || packet[0] == CMD_EEPROM_READ_RSP) {
        current_run->n_packets++;
        if(packet[0] == CMD_EEPROM_READ_RSP) return true;
        uint16_t addr = (packet[8]<<8) | (packet[9]);

        if(current_run->start == 0x0000) {
            // we aren't in a run
            current_run->start = addr;
            current_run->len = packet[10];
            current_run->is_write = false;
            current_run->in_run = true;
        } else {
            // we are in a run

            if(addr == (current_run->start+current_run->len)) {
                // run continues
                current_run->len += packet[10];
            } else {
                // run breaks, but we are reading an area

                next_run = (rw_run_t) {
                    .start = addr,
                    .len = packet[10],
                    .is_write = false,
                    .n_packets = 1,
                    .in_run = true,
                };

                current_run->n_packets--; // excluse this packet from last run
                current_run->in_run = false;
            }
        }
    } else {
        current_run->in_run = false;
    }

    if(!current_run->in_run) {
        if( current_run->start != 0x0000) {
            if(current_run->is_write) {
                printf("[Write 0x%04x bytes to addr 0x%04x (%lu packets)]\n", current_run->len, current_run->start, current_run->n_packets);
                *current_run = (rw_run_t) {
                    0
                };
            } else {
                printf("[Read 0x%04x bytes from addr 0x%04x (%lu packets)]\n", current_run->len, current_run->start, current_run->n_packets);
                *current_run = (rw_run_t) {
                    0
                };
            }

            // there's a chance that current run breaks because master
            // starts writing to a different area.
            // in this case, we are currently processing the first
            // packet of this new run.
            // so we need to keep track of this packet or else we'll
            // have an off-by-one error
            if(next_run.in_run) {
                *current_run = next_run;
            }

            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

size_t decode_packet(const size_t len, uint8_t buf[len]) {
    if(len == 0) return 0; // nothing to decode
    if(len < 8) {
        printf("Error: Remaining buffer length less than sizeof(packet_header)\n");
        return 0;
    }

    packet_header_t *ph = (packet_header_t*)buf;

    if(ph->session_id != g_session_id) {
        printf("Error: Packet session id (%08x) does not match global session id (%08x)\n",
               ph->session_id, g_session_id);
        return 0;
    }

    // brute force. assume every byte is start of packet.
    // if session id does not match, slide up by 1 byte
    size_t packet_len;
    for(
        packet_len = 8;
        (((packet_header_t*)(buf+packet_len))->session_id != g_session_id) && packet_len < len;
        packet_len++
    );
    if(packet_len > 0x88) {
        printf("Error: packet too long, aborting\n");
        return 0;
    }
    if(packet_len == len) return len;

    return packet_len;
}

void print_walker_info(walker_info_t *wi) {
    printf("\tunk0: 0x%08x; unk1: 0x%08x\n", wi->le_unk0, wi->le_unk1);
    printf("\tunk2:     0x%04x; unk3:     0x%04x\n", wi->le_unk2, wi->le_unk3);
    printf("\ttid: %d\n\tsid: %d\n", wi->le_tid, wi->le_sid);
    printf("\tidentity_data: ");
    for(size_t i = 0; i < sizeof(unique_identity_data_t); i++) {
        if(i%16 == 0) printf("\n\t\t");
        else if(i%8 == 0) printf(" ");
        printf("%02x", wi->identity_data.data[i]);
    }
    printf("\n\tevent_bitmap: ");
    for(size_t i = 0; i < sizeof(event_bitmap_t); i++) {
        if(i%8 == 0) printf(" ");
        printf("%02x", wi->event_bitmap.data[i]);
    }
    printf("\n\tevent_index: %d\n", wi->event_index);
    printf("\ttrainer_name: ");
    for(size_t i = 0; i < 8; i++) {
        printf("0x%04x ", wi->le_trainer_name[i]);
    }
    printf("\n\tflags: 0x%02x", wi->flags);
    if(wi->flags != 0) {
        printf(" [");
        if(wi->flags & WALKER_INFO_FLAG_INIT) printf("\n\t\tWALKER_INFO_FLAG_INIT,");
        if(wi->flags & WALKER_INFO_FLAG_HAS_POKEMON) printf("\n\t\tWALKER_INFO_FLAG_HAS_POKEMON,");
        if(wi->flags & WALKER_INFO_FLAG_POKEMON_JOINED) printf("\n\t\tWALKER_INFO_FLAG_POKEMON_JOINED,");
        printf("\n\t]\n");
    } else printf("\n");
    printf("\tprotocol_ver: 0x%02x; subver: 0x%02x\n", wi->protocol_ver, wi->protocol_subver);

    printf("\tlast_sync: %d (le)\n", swap_u32(wi->be_last_sync));
    printf("\tstep_count: %d (le)\n", swap_u32(wi->be_step_count));

    printf("\tunk4: 0x%02x 0x%02x\n", wi->unk4[0], wi->unk4[1]);
    printf("\tunk5: 0x%02x\n", wi->unk5);
    printf("\tunk8: 0x%02x\n", wi->unk8);

}

size_t print_packet(const size_t len, uint8_t buf[len]) {
    pw_packet_t *packet = (pw_packet_t*)buf;

    printf(" %02x (extra: 0x%02x) ", packet->cmd, packet->extra);

    switch(packet->cmd) {
    case CMD_EEPROM_WRITE_CMP_00: {
        printf("(CMD_EEPROM_WRITE_CMP)\n");
        printf("\taddr: 0x%02x00; compressed_len: %lu; len: 128\n",
               packet->extra, len-8);

        break;
    }
    case CMD_EEPROM_WRITE_CMP_80: {
        printf("(CMD_EEPROM_WRITE_CMP)\n");
        printf("\taddr: 0x%02x80; compressed_len: %lu; len: 128\n",
               packet->extra, len-8);

        break;
    }
    case CMD_EEPROM_WRITE_RAW_00: {
        printf("(CMD_EEPROM_WRITE_RAW)\n");
        printf("\taddr: 0x%02x00; len: 128\n",
               packet->extra);

        break;
    }
    case CMD_EEPROM_WRITE_RAW_80: {
        printf("(CMD_EEPROM_WRITE_RAW)\n");
        printf("\taddr: 0x%02x80; len: 128\n",
               packet->extra);
        break;
    }
    case CMD_RAM_WRITE: {
        printf("(CMD_RAM_WRITE)\n");
        printf("\tpayload_len: %lu\n", len-8);
        break;
    }
    case CMD_EEPROM_WRITE_ACK: {
        printf("(CMD_EEPROM_WRITE_ACK)\n");
        break;
    }
    case CMD_EEPROM_READ_REQ: {
        printf("(CMD_EEPROM_READ_REQ)\n");
        printf("\taddr: 0x%02x%02x; len: %d\n",
               packet->payload[0], packet->payload[1], packet->payload[2]);
        break;
    }
    case CMD_EEPROM_READ_RSP: {
        printf("(CMD_EEPROM_READ_RSP)\n");
        printf("\tpayload_len: %lu\n", len-8);
        break;
    }
    case CMD_IDENTITY_REQ: {
        printf("(CMD_IDENTITY_REQ)\n");
        break;
    }
    case CMD_IDENTITY_RSP: {
        printf("(CMD_IDENTITY_RSP)\n");
        walker_info_t *wi = (walker_info_t*)packet->payload;
        print_walker_info(wi);
        break;
    }
    case CMD_IDENTITY_SEND: {
        printf("(CMD_IDENTITY_SEND)\n");
        walker_info_t *wi = (walker_info_t*)packet->payload;
        print_walker_info(wi);
        break;
    }
    case CMD_IDENTITY_ACK: {
        printf("(CMD_IDENTITY_ACK)\n");
        break;
    }
    case CMD_IDENTITY_SEND_ALIAS1: {
        printf("(CMD_IDENTITY_SEND_ALIAS1)\n");
        walker_info_t *wi = (walker_info_t*)packet->payload;
        print_walker_info(wi);
        break;
    }
    case CMD_IDENTITY_ACK_ALIAS1: {
        printf("(CMD_IDENTITY_ACK_ALIAS1)\n");
        break;
    }
    case CMD_IDENTITY_SEND_ALIAS2: {
        printf("(CMD_IDENTITY_SEND_ALIAS2)\n");
        walker_info_t *wi = (walker_info_t*)packet->payload;
        print_walker_info(wi);
        break;
    }
    case CMD_IDENTITY_ACK_ALIAS2: {
        printf("(CMD_IDENTITY_ACK_ALIAS2)\n");
        break;
    }
    case CMD_IDENTITY_SEND_ALIAS3: {
        printf("(CMD_IDENTITY_SEND_ALIAS3)\n");
        walker_info_t *wi = (walker_info_t*)packet->payload;
        print_walker_info(wi);
        break;
    }
    case CMD_IDENTITY_ACK_ALIAS3: {
        printf("(CMD_IDENTITY_ACK_ALIAS3)\n");
        break;
    }
    case CMD_PING: {
        printf("(CMD_PING)\n");
        break;
    }
    case CMD_PONG: {
        printf("(CMD_PONG)\n");
        break;
    }
    case CMD_WALK_START: {
        printf("(CMD_WALK_START)\n");
        break;
    }
    case CMD_WALK_START_INIT: {
        printf("(CMD_WALK_START_INIT)\n");
        break;
    }
    case CMD_DISCONNECT: {
        printf("(CMD_DISCONNECT)\n");
        break;
    }
    case CMD_WALK_END_REQ: {
        printf("(CMD_WALK_END_REQ)\n");
        break;
    }
    case CMD_WALK_END_ACK: {
        printf("(CMD_WALK_END_ACK)\n");
        break;
    }
    case CMD_CONNECT_COMPLETE: {
        printf("(CMD_CONNECT_COMPLETE)\n");
        break;
    }
    case CMD_CONNECT_COMPLETE_ACK: {
        printf("(CMD_CONNECT_COMPLETE_ACK)\n");
        break;
    }
    case CMD_WALKER_RESET_1: {
        printf("(CMD_WALKER_RESET_1)\n\tpayload:");
        for(size_t i = 0; i < len-8; i++) {
            if(i%16 == 0) printf("\n\t\t");
            else if(i%8 == 0) printf(" ");
            printf("%02x", packet->payload[i]);
        }
        printf("\n");
        break;
    }
    default:
        printf("\npayload_len: %lu\n", len-8);
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: decode-transaction file.bin");
        exit(EXIT_FAILURE);
    }

    char *fname = argv[1];

    FILE *fh = fopen(fname, "rb");
    if(!fh) {
        fprintf(stderr, "Error: cannot open file: %s\n", fname);
        exit(EXIT_FAILURE);
    }

    struct stat s;
    if( stat(fname, &s) == -1 ) exit(EXIT_FAILURE);

    size_t orig_len = s.st_size;
    uint8_t *bytes = malloc(orig_len);

    fread(bytes, 1, orig_len, fh);
    fclose(fh);

    if(bytes[0] != 0xfc) {
        fprintf(stderr, "Error: file does not start with advertising byte 0xfc\n");
        exit(EXIT_FAILURE);
    }

    if(bytes[1] != 0xfa) {
        fprintf(stderr, "Error: first packet after advertising byte is not 0xfa\n");
        exit(EXIT_FAILURE);
    }

    size_t cursor = 1 + 8 + 8; // skip first advertising byte and two handshake packets
    size_t len = orig_len - cursor;

    packet_header_t *ack1 = (packet_header_t*)(bytes+1);
    packet_header_t *ack2 = (packet_header_t*)(bytes+1+8);
    g_session_id = ack1->session_id ^ ack2->session_id;
    printf("Setting global seession id: %08x\n", g_session_id);

    size_t n_packets = 0;
    const size_t max_packets = 1024;
    uint8_t **packets = malloc(sizeof(packets[0]) * max_packets);
    size_t *packet_lens = malloc(sizeof(size_t) * max_packets);

    uint8_t *buf = bytes+cursor;
    size_t packet_len;
    while( (packet_len=decode_packet(len, buf)) != 0 ) {
        if(n_packets == max_packets) {
            fprintf(stderr, "Error: too many packets to put in buffer. Increase buffer size\n");
            exit(EXIT_FAILURE);
        }

        packet_lens[n_packets] = packet_len;
        packets[n_packets] = buf;
        n_packets++;

        buf += packet_len;
        len -= packet_len;
    }

    rw_run_t current_run = {0};
    size_t unprocessed_packets = 0;
    bool in_run = false;

    for(size_t i = 0; i < n_packets; i++) {

        if(!count_run(packet_lens[i], packets[i], &current_run)) {
            // assume always A-B messages
            if(i%2 == 1) {
                printf("slave: ");
            } else {
                printf("master:");
            }
            unprocessed_packets += print_packet(packet_lens[i], packets[i]);

        }
    }



    /*
    if(len != ) {
        fprintf(stderr, "Error: Bytes left at end of file, decode incomplete\n");
        exit(EXIT_FAILURE);
    }
    */

    printf("Finished with %lu packets\n", n_packets);
    printf("(%lu unprocessed)\n", unprocessed_packets);

    return 0;
}
