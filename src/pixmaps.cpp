#include <ncurses.h>
#include <iostream>

#include "pixmaps.hpp"

const uint8_t WIDTH = 64;
const uint8_t HEIGHT = 32;
const uint8_t FIELDS = 3;
const uint8_t BUFFERS = 1;

uint8_t pix_fields[BUFFERS][FIELDS][WIDTH/8][HEIGHT];
uint8_t ACTIVE_BUFFER = 0 ; // current buffer being drawn in
uint8_t RENDER_BUFFER = 0 ; // current buffer being displayed
uint8_t col_mask[WIDTH/8][HEIGHT];

enum display_flags_t {rendering =0x01} ;

uint8_t DISPLAY_FLAGS=0; // for coordinating display rendering, buffer swaps, etc

void fill_display(uint8_t color) {

  // optimize: remove x/y loops and use memset()
    for (int f=0; f<FIELDS; f++) {
        uint8_t f_mask = (color & (0x01 << f)) != 0;

        for (int x=0; x< WIDTH; x++) {
            uint8_t byte = x >> 3;
            uint8_t bit = x & 0x07;
            uint8_t clear_mask = ~(0x01 << bit);
            uint8_t bit_mask = f_mask << bit;
            for (int y=0; y<HEIGHT; y++) {
                pix_fields[ACTIVE_BUFFER][f][byte][y] &= clear_mask; //clear the bit
                pix_fields[ACTIVE_BUFFER][f][byte][y] |= bit_mask; // set it as per the color
            }       
        }
    }
}

void plot(uint8_t buffer, uint8_t x, uint8_t y, uint8_t color) {
    uint8_t byte = x >> 3;
    uint8_t bit = x & 0x07;
    uint8_t clear_mask = ~(0x01 << bit);
    for (int f=0; f<FIELDS; f++) {
        uint8_t f_mask = (color & (0x01 << f)) != 0;
        uint8_t bit_mask = f_mask << bit;
        pix_fields[buffer][f][byte][y] &= clear_mask; //clear the bit
        pix_fields[buffer][f][byte][y] |= bit_mask; // set it as per the color
    }
}

void plot(uint8_t x, uint8_t y, uint8_t color){
  plot(ACTIVE_BUFFER, x, y, color);
}

uint8_t get(uint8_t buffer, uint8_t x, uint8_t y) {
    uint8_t byte = x >> 3;
    uint8_t bit = x & 0x07;
    uint8_t pixel = 0;
    for (int f=0; f<FIELDS; f++) {
        uint8_t field_val = (pix_fields[buffer][f][byte][y] >> bit)  & 0x01;
        pixel |= field_val << f; // set it as per the color
    }
    return pixel;
}

uint8_t get(uint8_t x, uint8_t y) {
  return get(ACTIVE_BUFFER, x, y);
}

/**
 * @brief: commit the display buffer to the physical screen
 *
 */
void draw_display() {
  // yes this is redundant for single threaded/no interrupts...
  DISPLAY_FLAGS |= display_flags_t::rendering;

  for (int y=0; y<HEIGHT; y++) {
      for (int x=0; x< WIDTH ; x++) {
        uint8_t pixel = get(RENDER_BUFFER, x, y);
        init_pair(1, pixel, COLOR_BLACK);
        std::cout<<"pixel "<< pixel*2 << std::endl;
        attron(COLOR_PAIR(1));
        mvaddch(y, x, 'X');
        attroff(COLOR_PAIR(1));

      }
  }
  DISPLAY_FLAGS &= ~(display_flags_t::rendering);
}

/**
 * @brief: swap the buffer being drawn too and request display of current frame
 *
 */
void request_display_swap() {
  // this makes anything thing more than double buffering pointless
  // for triple buffering or more there'd need to be some sort of
  // dirty frame tracking, possibly including dropping frames...
  while(DISPLAY_FLAGS & display_flags_t::rendering) {
   // can't update render frame if we're still rendering!     
  }
  RENDER_BUFFER = ACTIVE_BUFFER;
  // this will eventually be a function pointer
  draw_display();

  ACTIVE_BUFFER++;
  if (ACTIVE_BUFFER == BUFFERS) { 
    ACTIVE_BUFFER = 0;
  }
  fill_display(0);
}