#include <stdio.h>
#include <stdint.h>

struct image {
    uint16_t *palette;
    uint8_t  *data;
};

int decode_img(uint8_t *data, size_t data_len, uint16_t *buf, size_t buf_len) {
    struct image img = {
        .palette=(uint16_t*)data,
        .data=&(data[32])
    };

    for(size_t i = 0; i < data_len; i++) {
        if( (2*i+1) >= buf_len ) return -1;
        buf[2*i]   = img.palette[data[i]>>4];
        buf[2*i+1] = img.palette[data[i]&0xf];
    }

    return 0;
}

int main(int argc, char** argv) {

    char* fname = argv[1];

    FILE *fp = fopen(fname, "rb");
    if(!fp) {
        fprintf(stderr, "Error: cannot open file %s\n", fname);
        return EXIT_FAILURE;
    }

    struct stat s;
    if( stat(fname, &s) == -1) {
        fprintf(stderr, "Error: cannot get size of file %s\n", fname);
        return EXIT_FAILURE;
    };

    size_t image_size = s.st_size;
    if(image_size <= 32) {
        fprintf(stderr, "Error: image size is too small\n");
        return EXIT_FAILURE;
    };

    size_t colour_size = (image_size-32)*2;
    uint8_t  *image_buf = malloc(image_size);
    uint16_t *colour_buf = malloc(colour_size);

    fread(image_buf, s.st_size, 1, fp);

    fclose(fp);

    decode_img(image_buf, s.st_size, colour_buf, colour_size);


    return 0;
}


