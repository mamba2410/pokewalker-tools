#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "trainer_info.h"
#include "route_info.h"

int pw_decompress_data(uint8_t *data, uint8_t *buf, size_t dlen);

int main(int argc, char** argv) {

    char* fname = argv[1];

    FILE *fin = fopen(fname, "rb");
    if(!fin) return 1;
    struct stat s;
    if( stat(fname, &s) == -1 ) return 1;

    uint8_t *fbuf = malloc(256);
    fread(fbuf, 1, s.st_size, fin);
    fclose(fin);

    size_t len = s.st_size;
    uint8_t *dbuf = malloc(256);

    uint8_t *packet = fbuf;
    pw_decompress_data(packet+8, dbuf, len-8);

    fname = argv[2];
    FILE *fout = fopen(fname, "wb");
    if(!fout) return 1;
    if( stat(fname, &s) == -1 ) return 1;

    fwrite(dbuf, 128, 1, fout);
    fclose(fout);

    route_info_t *ri = (route_info_t*)dbuf;

    printf("species: %04x\n", ri->pokemon_summary.le_species);
    for(int i = 0; i < 3; i++)
        printf("");

    free(fbuf);
    free(dbuf);

    return 0;
}

/*
 *  data is packet minus 8-byte header
 *  Assume buf can hold decompressed data
 */
int pw_decompress_data(uint8_t *data, uint8_t *buf, size_t dlen) {
    if(data == 0 || buf == 0) return -1;

    size_t c = 0;
    size_t oc = 0;

    uint8_t decomp_type = data[c++];
    if(decomp_type != 0x10)
        return -1;

    // LE size
    uint32_t decomp_size = data[c] | data[c+1] << 8 | data[c+2] << 16;
    if(decomp_size != 128)
        return -1;
    c += 3;

    while(c < dlen) {
        // loop through header
        uint8_t header = data[c++];
        for(uint8_t chunk_idx = (1<<7); chunk_idx > 0; chunk_idx >>= 1) {
            uint8_t cmd = header & chunk_idx;

            if(cmd != 0) {
                // 2-byte backreference
                uint8_t sz = (data[c]>>4) + 3;
                uint16_t backref = (( (data[c]&0x0f) << 8) | (data[c+1] )) + 1;
                c += 2;

                //if(backref > oc)
                //    return -1;

                size_t start = oc-backref;
                size_t end   = start + sz;
                for(size_t i = start; i < end; i++, oc++)
                    buf[oc] = buf[i];

            } else {
                // 1-byte raw data
                buf[oc++] = data[c++];
            }
        }
    }

    if(oc != decomp_size) {
        fprintf(stderr, "Error: ourput cursor was %d\n", oc);
        return -1;
    }

    return 0;
}
