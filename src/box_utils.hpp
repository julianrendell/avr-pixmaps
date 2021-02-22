/* 
 * draw basic boxes
 */
#include <ncurses.h>

typedef struct _box_border_struct {
  chtype ls, rs, ts, bs, tl, tr, bl, br;
} BOX_BORDER;

typedef struct _BOX_struct {
  int startx, starty;
  int height, width;
  BOX_BORDER border;
} BOX;

void init_box_params(BOX* p_win);
void print_box_params(BOX* p_win);
void create_box(BOX* box, bool flag);

