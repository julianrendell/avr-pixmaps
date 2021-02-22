#include <ncurses.h>

#include "pixmaps.hpp"

const u_int8_t WIDTH = 64;
const u_int8_t HEIGHT = 32;
const u_int8_t FIELDS =4;

u_int8_t pix_fields[FIELDS][WIDTH/8][HEIGHT];
u_int8_t col_mask[WIDTH/8][HEIGHT];

void fill_display(u_int8_t color) {
    for (int f=0; f<FIELDS; f++) {
        u_int8_t f_mask = (color & (0x01 << f)) != 0;

        for (int x=0; x< WIDTH; x++) {
            u_int8_t byte = x >> 3;
            u_int8_t bit = x & 0x07;
            u_int8_t clear_mask = ~(0x01 << bit);
            u_int8_t bit_mask = f_mask << bit;
            for (int y=0; y<HEIGHT; y++) {
                pix_fields[f][byte][y] &= clear_mask; //clear the bit
                pix_fields[f][byte][y] |= bit_mask; // set it as per the color
            }       
        }
    }
}

void plot(u_int8_t x, u_int8_t y, u_int8_t color) {
    u_int8_t byte = x >> 3;
    u_int8_t bit = x & 0x07;
    u_int8_t clear_mask = ~(0x01 << bit);
    for (int f=0; f<FIELDS; f++) {
        u_int8_t f_mask = (color & (0x01 << f)) != 0;
        u_int8_t bit_mask = f_mask << bit;
        pix_fields[f][byte][y] &= clear_mask; //clear the bit
        pix_fields[f][byte][y] |= bit_mask; // set it as per the color
    }
}

u_int8_t get(u_int8_t x, u_int8_t y) {
    u_int8_t byte = x >> 3;
    u_int8_t bit = x & 0x07;
    u_int8_t bit_mask = 0x01 << bit;
    u_int8_t pixel = 0;
    for (int f=0; f<FIELDS; f++) {
        u_int8_t field_val = (pix_fields[f][byte][y] >> bit)  & 0x01;
        pixel |= field_val << f; // set it as per the color
    }
    return pixel;
}

void draw_display() {
    for (int y=0; y<HEIGHT; y++) {
        for (int x=0; x< WIDTH ; x++) {

            u_int8_t pixel = get(x,y);
            
            char pixel_char;
            switch (pixel)
            {
            case 0:
                pixel_char = ' ';
                break;

            case 1:
                pixel_char = '.';
                break;
            
            case 2:
                pixel_char = '+';
                break;

            case 3:
                pixel_char = '0';
                break;

            default:
                pixel_char = '0' + pixel;
                break;
            }
            mvaddch(y, x, pixel_char);

            if (pixel > 0) {
                pixel -= 1;
            }
            
            plot(x,y, pixel);
        }
    }
}
