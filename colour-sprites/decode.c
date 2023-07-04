#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#define RLE_DLEN_OFFSET     0
#define RLE_PALETTE_OFFSET  (RLE_DLEN_OFFSET+2)
#define RLE_DATA_OFFSET     (RLE_PALETTE_OFFSET+32)

struct image {
    uint16_t *palette;
    uint8_t  *data;
};

struct rle_image {
    uint16_t dlen;
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


/*
 *  Run-length encoded data, packed
 */
int decode_rle(uint8_t *fdata, size_t fdata_len, uint16_t *buf, size_t buf_len) {
    struct rle_image img = {
        .dlen = *(uint16_t*)fdata,
        .palette = (uint16_t*)(&fdata[RLE_PALETTE_OFFSET]),
        .data = &fdata[RLE_DATA_OFFSET]
    };

    if(img.dlen > fdata_len - RLE_DATA_OFFSET) return -1;

    size_t out_cursor = 0;
    for(size_t i = 0; i < img.dlen; i++) {
        uint8_t colour_idx = img.data[i]>>4;
        uint8_t run_length = img.data[i]&0x0f;

        // copy pixel value `run_length` times to the output buffer
        for(uint8_t j = 0; j < run_length; j++, out_cursor++) {
            if(out_cursor >= buf_len) return -1;
            buf[out_cursor] = img.palette[colour_idx];
        }
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

    //size_t colour_size = (image_size-32)*2;
    size_t colour_size = (64*48*2)*2;    // 64x48 image, 2 frames, 2 bytes per pixel

    uint8_t  *image_buf = malloc(image_size);
    uint16_t *colour_buf = malloc(colour_size);

    fread(image_buf, s.st_size, 1, fp);

    fclose(fp);

    int ret = 0;
    //ret = decode_img(image_buf, s.st_size, colour_buf, colour_size);
    ret = decode_rle(image_buf, image_size, colour_buf, colour_size);

    if(ret != 0) {
        printf("Could not decode %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    printf("Input file size: %lu\nOutput file size: %lu\n", image_size, colour_size);

    free(image_buf);
    free(colour_buf);

    return 0;
}


