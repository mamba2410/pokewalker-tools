#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "trainer_info.h"
#include "route_info.h"


int main(int argc, char** argv) {

    if(argc < 5) {
        fprintf(stderr, "Error: need 4 args\n");
        return 1;
    }
    char* ifname = argv[1];
    char* ofname = argv[2];
    size_t addr = strtol(argv[3], NULL, 0);
    size_t len  = strtol(argv[4], NULL, 0);

    fprintf(stderr, "Copying %lu bytes from '%s':%04x to '%s'\n ", len, ifname, addr, ofname);

    struct stat s;

    FILE *fin = fopen(ifname, "rb");
    if(!fin) return 1;
    if( stat(ifname, &s) == -1 ) return 1;

    uint8_t *buf = malloc(len);
    fseek(fin, addr, SEEK_SET);
    fread(buf, 1, len, fin);
    fclose(fin);

    FILE *fout = fopen(ofname, "wb");
    if(!fout) return 1;
    //if( stat(ofname, &s) == -1 ) return 1;

    fwrite(buf, len, 1, fout);
    fclose(fout);

    free(buf);

    return 0;
}
