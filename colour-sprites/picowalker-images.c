#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>

#define E4_IDX_MSK  0x0f
#define E4_IDX_OFF  0
#define E4_LEN_MSK  0xf0
#define E4_LEN_OFF  4

#define E5_IDX_MSK  0x1f
#define E5_IDX_OFF  0
#define E5_LEN_MSK  0xe0
#define E5_LEN_OFF  5

#define E4_TRANSPARENT 0x0f
#define E5_TRANSPARENT 0x1f

#define E4_MAXLEN (1<<4)  // 4 len bits
#define E5_MAXLEN (1<<3)  // 3 len bits

#define E4_IDX(e) ((e&E4_IDX_MSK)>>E4_IDX_OFF)
#define E4_LEN(e) (((e&E4_LEN_MSK)>>E4_LEN_OFF)+1) // len always >0
#define E5_IDX(e) ((e&E5_IDX_MSK)>>E5_IDX_OFF)
#define E5_LEN(e) (((e&E5_LEN_MSK)>>E5_LEN_OFF)+1) // len always >0

#define E4_FROM(l, i) ((l>>E4_LEN_OFF) | (i&E4_IDX_MSK))
#define E5_FROM(l, i) ((l>>E5_LEN_OFF) | (i&E5_IDX_MSK))

typedef int16_t screen_pos_t;
//typedef uint8_t[3] pixel_colour_t;
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_colour_t;

/*
 * Note: length bit pattern of 0b0000 corresponds to run length of 1
 *                             0b1111 corresponds to run length of 16
 * Reason: why store an entry if there are no pixels
 */
typedef uint8_t rle_entry4_t;

/*
 * Note: length bit pattern of 0b000 is run length of 1
 *                             0b111 is run length of 8
 * Reason: same as rle_entry4_t
 */
typedef uint8_t rle_entry5_t;

typedef struct {
    pixel_colour_t palette[15];
    screen_pos_t width;
    screen_pos_t height;
    size_t size;
    rle_entry4_t *data;
} image15_t;

typedef struct {
    pixel_colour_t palette[31];
    screen_pos_t width;
    screen_pos_t height;
    size_t size;
    rle_entry5_t *data;
} image31_t;

typedef struct {
    screen_pos_t width;
    screen_pos_t height;
    pixel_colour_t *data;
} uncompressed_image_t;

uint8_t min(uint8_t a, uint8_t b) {
    return (a<b)?a:b;
}

uint8_t max(uint8_t a, uint8_t b) {
    return (a>b)?a:b;
}


// for now, assume both same size
image31_t merge15(image15_t lower, image15_t upper) {
    image31_t merged = {0};
    if(lower.width != upper.width || lower.height != upper.height) return merged;

    merged.width = lower.width;
    merged.height = lower.height;
    merged.size = merged.width*merged.height;
    merged.data = malloc(merged.size); // alloc for worst-case scenario

    for(size_t i = 0; i < 15; i++) {
        merged.palette[i] = lower.palette[i];
        merged.palette[i+16] = upper.palette[i];
    }

    size_t lower_index  = 0;
    size_t upper_index  = 0;
    size_t output_index = 0;

    size_t start = 0;

    uint8_t llen = E4_LEN(lower.data[lower_index]);
    uint8_t lidx = E4_IDX(lower.data[lower_index]);

    uint8_t ulen = E4_LEN(upper.data[upper_index]);
    uint8_t uidx = E4_IDX(upper.data[upper_index]);

    while(start < merged.size) {

        uint8_t e5_len = min(llen, ulen);
        llen -= e5_len;  // shorten lower run
        ulen -= e5_len;  // shorten upper run
                         // at least one of these will be zero
        start += e5_len; // move up next run start

        // set e5 colour index
        uint8_t e5_idx;
        if(uidx == E4_TRANSPARENT) {
            e5_idx = lidx;
        } else {
            e5_idx = uidx + 16;
        }


        while(e5_len > 0) {
            // -1 because len=0 is actually a length of 1
            uint8_t l = max(E5_MAXLEN, e5_len)-1;
            merged.data[output_index] = E5_FROM(l, e5_idx);
            output_index += 1;
            e5_len -= l;
        }


        // out of upper run
        if(ulen == 0) {
            // get new upper run
            upper_index+=1;
            ulen = E4_LEN(upper.data[upper_index]);
            uidx = E4_IDX(upper.data[upper_index]);
        }

        // out of lower run
        if(llen == 0) {
            // get new lower run
            lower_index+=1;
            llen = E4_LEN(lower.data[lower_index]);
            lidx = E4_IDX(lower.data[lower_index]);

        }

    }

    // set used size
    merged.size = output_index;
    return merged;

}

void unpack_e5(image31_t img, const size_t buf_len, pixel_colour_t buf[buf_len]) {
    if(buf == 0) {
        buf = malloc(img.width*img.height*sizeof(*buf));
    }

    if(buf_len < img.width * img.height) return;

    size_t output_cursor = 0;
    for(size_t i = 0; i < img.size; i++) {
        rle_entry5_t e = img.data[i];
        uint8_t len = E5_LEN(e);
        uint8_t idx = E5_IDX(e);

        for(uint8_t j = 0; j < len; j++, output_cursor++) {
            buf[output_cursor] = img.palette[idx];
        }
    }


}

void dummy_screen_display(size_t len, pixel_colour_t buf[len]) { }

void merge_and_draw(image15_t lower, image15_t upper) {
    pixel_colour_t *line_buffer = malloc(lower.width*sizeof(*line_buffer));


    size_t lower_index  = 0;
    size_t upper_index  = 0;
    size_t output_offset = 0;

    size_t start = 0;

    uint8_t llen = E4_LEN(lower.data[lower_index]);
    uint8_t lidx = E4_IDX(lower.data[lower_index]);

    uint8_t ulen = E4_LEN(upper.data[upper_index]);
    uint8_t uidx = E4_IDX(upper.data[upper_index]);

    for(size_t y = 0; y < lower.height; y++) {

        while(start < lower.width) {
            uint8_t len = min(min(llen, ulen), lower.width-start);
            llen -= len;
            ulen -= len;
        
            pixel_colour_t colour;
            if(uidx == E4_TRANSPARENT) {
                colour = upper.palette[uidx];
            } else {
                colour = lower.palette[lidx];
            }

            for(uint8_t i = 0; i < len; i++,start++ ) {
                line_buffer[start] = colour;
                }

            // out of upper run
            if(ulen == 0) {
                // get new upper run
                upper_index+=1;
                ulen = E4_LEN(upper.data[upper_index]);
                uidx = E4_IDX(upper.data[upper_index]);
            }

            // out of lower run
            if(llen == 0) {
                // get new lower run
                lower_index+=1;
                llen = E4_LEN(lower.data[lower_index]);
                lidx = E4_IDX(lower.data[lower_index]);

            }
        }

        dummy_screen_display(lower.width, line_buffer);
        start = 0;
    }

}

image15_t compress_image(size_t len, uint8_t buf[len]) {

}

int main(int argc, char** argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: %s image.bmp\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* fname = argv[1];

    FILE *fh = fopen(fname, "rb");
    if(fh == NULL) {
        fprintf(stderr, "Can't open file \"%s\"\n", fname);
        exit(EXIT_FAILURE);
    }

    struct stat s;
    if( stat(fname, &s) == -1 ) {
        fprintf(stderr, "Can't stat file \"%s\"\n", fname);
        exit(EXIT_FAILURE);
    }

    uint8_t *buf = malloc(s.st_size);
    fread(buf, s.st_size, 1, fh);
    fclose(fh);

    uint32_t data_offset        = *( (uint32_t*)(&buf[0x0a]));
    int32_t  image_width        = *( ( int32_t*)(&buf[0x12]));
    int32_t  image_height       = *( ( int32_t*)(&buf[0x16]));
    uint16_t bitmap_depth       = *( (uint16_t*)(&buf[0x1c]));
    uint32_t compression_format = *( (uint32_t*)(&buf[0x1e]));
    uint32_t data_size          = *( (uint32_t*)(&buf[0x22]));

    printf(
"bitmap file:\
    width:  %d\
    height: %d\
    depth:  %d\
    data_size: %d\
\n",
    image_width, image_height, bitmap_depth, data_size);

    if(image_width <= 0 || image_height <= 0) {
        fprintf(stderr, "Image has no-positive dimensions\n");
        exit(EXIT_FAILURE);
    }

    if(compression_format != 0) {
        fprintf(stderr, "Image is using unsupported compression format (%d)\n", compression_format);
        exit(EXIT_FAILURE);
    }

    if(bitmap_depth != 24) {
        fprintf(stderr, "Image is using unsupported bit depth (%d)\n", bitmap_depth);
        exit(EXIT_FAILURE);
    }

    uncompressed_image_t image = {
        .width  = (screen_pos_t)image_width,
        .height = (screen_pos_t)image_height,
        .data   = (pixel_colour_t*)(&buf[data_offset]),
        //.size   = (size_t)data_size;
    };

    return 0;
}
