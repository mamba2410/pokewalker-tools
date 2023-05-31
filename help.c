#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "eeprom_map.h"
#include "trainer_info.h"

int main(int argc, char** argv) {
    uint8_t *eeprom = malloc(64*1024);

    printf("%s\n", argv[1]);
    FILE *fh = fopen(argv[1], "rb");
    if(!fh) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return -1;
    }
    fread(eeprom, 1, 64*1024, fh);
    fclose(fh);

    walker_info_t *info = (walker_info_t*)(eeprom + 0x00ed);
    pokemon_summary_t *ps = (pokemon_summary_t*)(eeprom+PW_EEPROM_ADDR_ROUTE_INFO);

    info->le_unk1 = info->le_unk0;
    info->le_unk3 = info->le_unk2;
    info->flags |= 0x02;
    info->protocol_ver = 2;

    //ps->le_species = 0x04d; // ponyta
    //ps->le_species = 0x072; // tangela
    //ps->le_species = 0x054; // doduo
    //ps->le_species = 0x05f; // onix
    //ps->le_species = 0x03c; // poliwag
    //ps->le_species = 0x036; // psyduck
    //ps->le_species = 0x0a1; // sentret
    //ps->le_species = 0x07b; // scyther
    ps->le_species = 0x0af; // togepi
    //ps->le_species = 0x0b7; // marill
    //ps->le_species = 0x0aa; // chinchou




    uint8_t chk = 1;
    for(size_t i = 0; i < 104; i++)
        chk += ((uint8_t*)info)[i];

    eeprom[0x00ed+104] = chk;
    memcpy(eeprom+0x001ed, info, 105);

    fh = fopen(argv[1], "wb");
    if(!fh) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return -1;
    }
    fwrite(eeprom, 1, 64*1024, fh);
    fclose(fh);
    free(eeprom);

    return 0;
}

