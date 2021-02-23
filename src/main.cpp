#include <stdio.h>
#include <cstdint>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "box_utils.hpp"
#include "pixmaps.hpp"

#ifndef CTRL
#define CTRL(x) ((x)&0x1f)
#endif

int main(int argc, char* argv[]) {
  initscr();
  raw();
  start_color();
  cbreak();
  timeout(0);

  keypad(stdscr, true);
  noecho();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  printw("Press Ctrl-C or Q to exit");
  refresh();
  attroff(COLOR_PAIR(1));

  bool quit = false;

  while (!quit) {
    int ch = getch();
    switch (ch) {
      case 'q':
        quit = true;
        break;
      case CTRL('c'):
        quit = true;
        break;
    }

    //fill_display(color);
    //draw_display();
    for ( int i=0; i< 10; i++) {
      plot(rand() % 64, rand() % 32, 15); //rand() % 4);
    }
    draw_display(true);
    refresh();

    //++color &= 0x03;

    //sleep(1);
    usleep(100000);

  }
  endwin();
  return 0;
}
