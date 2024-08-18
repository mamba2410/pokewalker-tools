#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SCREEN_SCALE 4

typedef struct pw_img_s {
    uint16_t width;
    uint16_t height;
    size_t size;
    uint8_t *data;
} pw_img_t;

typedef enum {
    DECODE_OK = 0,
    DECODE_OUTPUT_TOO_SMALL,
    DECODE_NO_INPUT,
    DECODE_INDEX_OOB,
    N_DECODE_RESULT,
} decode_tag_t;

typedef struct decode_result_s {
    decode_tag_t tag;
    size_t data1;
    size_t data2;
} decode_result_t;

const char* DECODE_RESULT_STRINGS[N_DECODE_RESULT] = {
    [DECODE_OK]               = "Decode ok",
    [DECODE_OUTPUT_TOO_SMALL] = "Output too small",
    [DECODE_NO_INPUT]         = "No input",
    [DECODE_INDEX_OOB]        = "Index OOB",
};

const uint16_t colour_map[4] = {
    0xe75b, // white
    0xbe16, // light grey
    0x7c0e, // dark grey
    0x5289, // black
};

decode_result_t decode_img(pw_img_t *img, size_t out_len, uint8_t out_buf[out_len]);

void usage() {
    fprintf(stderr, """ \
usage: decode-rotate <width> <height> <input_file> <output_file> \
    input_file is expected to be raw bytes in Pokewalker image format \
    output_file will be 16-bit RGB565 data in binary format \
""");
}

int main(int argc, char** argv) {

    if(argc != 5) {
        fprintf(stderr, "Error: Expected 5 arguments, got %d\n", argc);
        usage();
        return EXIT_FAILURE;
    }

    /*
     * Positional arguments
     */
    size_t width   = strtol(argv[1], NULL, 0);
    size_t height  = strtol(argv[2], NULL, 0);
    char *ifname   = argv[3];
    char *ofname   = argv[4];

    printf("""Running with arguments:\n\
    width: %lu \n\
    height: %lu \n\
    infile: %s \n\
    outfile: %s \n""",
        width, height, ifname, ofname
    );

    FILE* fin = fopen(ifname, "rb");
    if(!fin) {
        fprintf(stderr, "Error: can't open file '%s'\n", ifname);
        return EXIT_FAILURE;
    }

    struct stat s;
    if( stat(ifname, &s) == -1 ) {
        fprintf(stderr, "Error: can't stat file '%s'\n", ifname);
        return EXIT_FAILURE;
    }

    size_t expected_size = width*height*2/8;
    if( s.st_size != expected_size ) {
        fprintf(stderr, "Error: expected file size %lu for image size %lu x %lu, got size %lu instead\n",
                expected_size, width, height, s.st_size);
        return EXIT_FAILURE;
    }

    uint8_t *in_buf = malloc(s.st_size);

    // 2 bytes per pixel
    size_t out_len = width * height * 2 * SCREEN_SCALE*SCREEN_SCALE;
    uint8_t *out_buf = malloc(out_len);
    memset(out_buf, 0, out_len);

    pw_img_t img = (pw_img_t){
        .width = width,
        .height = height,
        .size = expected_size,
        .data = in_buf,
    };

    decode_result_t res = decode_img(&img, out_len, out_buf);

    if(res.tag != DECODE_OK) {
        printf("Decode failure (%s), not writing to file\n", DECODE_RESULT_STRINGS[res.tag]);
        switch(res.tag) {
            case DECODE_OUTPUT_TOO_SMALL: {
                printf("\tExpected %lu bytes, buffer is %lu\n", res.data1, res.data2);
                break;
            }
            case DECODE_NO_INPUT: break;
            case DECODE_INDEX_OOB: {
                printf("Trying to access array at index %lu (size %lu)\n", res.data1, res.data2);
                break;
            }
            default: {
                printf("Invalid tag\n");
                break;
            }
        }
        return EXIT_SUCCESS;
    }

    FILE* fout = fopen(ofname, "wb");
    if(!fout) {
        fprintf(stderr, "Error: can't open outfile '%s'\n", ofname);
        return EXIT_FAILURE;
    }

    fwrite(out_buf, 1, out_len, fout);

    printf("Decode success, output written to '%s'\n", ofname);

    return EXIT_SUCCESS;
}


decode_result_t decode_img(pw_img_t *pw_img, size_t out_len, uint8_t out_buf[out_len]) {

    if(!pw_img) return (decode_result_t){DECODE_NO_INPUT, 0};

    uint8_t pixel_value, bpu, bpl;
    size_t row, col, stride = pw_img->height;

    pw_img->size = pw_img->width * pw_img->height * 2/8;

    // quit if the output buffer can't hold all the data
    size_t expected_size = pw_img->size * 2*SCREEN_SCALE*SCREEN_SCALE;
    if(out_len < expected_size) return (decode_result_t){DECODE_OUTPUT_TOO_SMALL, expected_size, out_len};

    // i = number of bytes into image
    for(size_t i = 0; i < pw_img->size; i+= 2) {
        bpu = pw_img->data[i+0];
        bpl = pw_img->data[i+1];

        // j = index of pixel in chunk
        for(size_t j = 0; j < 8; j++) {
            pixel_value  = ((bpu>>j) & 1) << 1;
            pixel_value |= ((bpl>>j) & 1);

            // transform coords
            size_t x_normal = (i/2)%pw_img->width;
            size_t y_normal = 8*(i/(2*pw_img->width)) + j;
            col = pw_img->height - y_normal;
            row = x_normal;

            // now we have pixel coordinate, write to all pixels
            // that need the colour
            for(size_t py = 0; py < SCREEN_SCALE; py++) {
                for(size_t px = 0; px < SCREEN_SCALE; px++) {
                    size_t base_index = SCREEN_SCALE*((SCREEN_SCALE*row+py)*stride) + SCREEN_SCALE*col+px;

                    out_buf[2*base_index+0] = colour_map[pixel_value]>>8;
                    out_buf[2*base_index+1] = colour_map[pixel_value]&0xff;
                    if(2*base_index >= out_len) return (decode_result_t){DECODE_INDEX_OOB, 2*base_index, out_len};
                }
            }
        }
    }


    return (decode_result_t){DECODE_OK, 0};
}

