#include <ncurses.h>

#include "pixmaps.hpp"

const uint8_t WIDTH = 64;
const uint8_t HEIGHT = 32;
const uint8_t FIELDS =4;

uint8_t pix_fields[FIELDS][WIDTH/8][HEIGHT];
uint8_t col_mask[WIDTH/8][HEIGHT];

void fill_display(uint8_t color) {
    for (int f=0; f<FIELDS; f++) {
        uint8_t f_mask = (color & (0x01 << f)) != 0;

        for (int x=0; x< WIDTH; x++) {
            uint8_t byte = x >> 3;
            uint8_t bit = x & 0x07;
            uint8_t clear_mask = ~(0x01 << bit);
            uint8_t bit_mask = f_mask << bit;
            for (int y=0; y<HEIGHT; y++) {
                pix_fields[f][byte][y] &= clear_mask; //clear the bit
                pix_fields[f][byte][y] |= bit_mask; // set it as per the color
            }       
        }
    }
}

void plot(uint8_t x, uint8_t y, uint8_t color) {
    uint8_t byte = x >> 3;
    uint8_t bit = x & 0x07;
    uint8_t clear_mask = ~(0x01 << bit);
    for (int f=0; f<FIELDS; f++) {
        uint8_t f_mask = (color & (0x01 << f)) != 0;
        uint8_t bit_mask = f_mask << bit;
        pix_fields[f][byte][y] &= clear_mask; //clear the bit
        pix_fields[f][byte][y] |= bit_mask; // set it as per the color
    }
}

uint8_t get(uint8_t x, uint8_t y) {
    uint8_t byte = x >> 3;
    uint8_t bit = x & 0x07;
    uint8_t pixel = 0;
    for (int f=0; f<FIELDS; f++) {
        uint8_t field_val = (pix_fields[f][byte][y] >> bit)  & 0x01;
        pixel |= field_val << f; // set it as per the color
    }
    return pixel;
}

/**
 * @brief: commit the display buffer to the physical screen
 *
 * @param fading if each pixel should have 1 subtracted after being displayed -- currently only useful for index shifting/intensity only displays
 */
void draw_display(bool fading) {
    for (int y=0; y<HEIGHT; y++) {
        for (int x=0; x< WIDTH ; x++) {

            uint8_t pixel = get(x,y);
            
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

            if (fading) { 
              if (pixel > 0) {
                  pixel -= 1;
              }
            }
            plot(x,y, pixel);
        }
    }
}
