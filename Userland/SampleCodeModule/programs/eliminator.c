// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "eliminator.h"
#include "commands.h"
#include "libSysCalls.h"
#include "music.h"
#include "utils.h"
#define UP         0
#define LEFT       1
#define DOWN       2
#define RIGHT      3
#define SPEED      1000
#define MAX_POINTS 999999

char p1Keys[4] = {'w', 'a', 's', 'd'};
char p2Keys[4] = {'i', 'j', 'k', 'l'};

char p1Dir = DOWN;
char p2Dir = UP;

uint64_t width;
uint64_t height;

uint32_t p1Color = 0xFF0000;
uint32_t p2Color = 0x00FF00;

int64_t p1Coord[2] = {0, 0};  // pxCoord is a coordinate to center
int64_t p2Coord[2] = {0, 0};

void change_dir(char c) {
  switch (c) {
  case 'w':
    p1Dir = UP;
    break;
  case 'a':
    p1Dir = LEFT;
    break;
  case 's':
    p1Dir = DOWN;
    break;
  case 'd':
    p1Dir = RIGHT;
    break;
  case 'i':
    p2Dir = UP;
    break;
  case 'j':
    p2Dir = LEFT;
    break;
  case 'k':
    p2Dir = DOWN;
    break;
  case 'l':
    p2Dir = RIGHT;
    break;
  default:
    break;
  }
}

void move() {
  switch (p1Dir) {
  case UP:
    p1Coord[1]--;
    break;
  case LEFT:
    p1Coord[0]--;
    break;
  case DOWN:
    p1Coord[1]++;
    break;
  case RIGHT:
    p1Coord[0]++;
    break;
  default:
    break;
  }

  switch (p2Dir) {
  case UP:
    p2Coord[1]--;
    break;
  case LEFT:
    p2Coord[0]--;
    break;
  case DOWN:
    p2Coord[1]++;
    break;
  case RIGHT:
    p2Coord[0]++;
    break;
  default:
    break;
  }
}

int did_lose(int players) {
  int toReturn = 0;
  if (!(p1Coord[0] >= 0 && p1Coord[0] < width && p1Coord[1] >= 16 && p1Coord[1] < height &&
        sys_get_pixel_color(p1Coord[0], p1Coord[1]) == 0x000000)) {
    toReturn = 1;
  }

  if (!(p2Coord[0] >= 0 && p2Coord[0] < width && p2Coord[1] >= 16 && p2Coord[1] < height &&
        sys_get_pixel_color(p2Coord[0], p2Coord[1]) == 0x000000) &&
      players == 2) {
    toReturn = (toReturn == 1) ? 3 : 2;
  }

  return toReturn;
}

void lose(int whoLost, int pts) {
  play_wah_wah_wah();
  sys_clear_screen();
  sys_set_cursor_to_line(15);
  switch (whoLost) {
  case 1:
    fprintf(
        STDOUT,
        "\n                                      Player 1 LOST! Do you want to play again? YES / NO?\n               "
        "                                   (change decision with a or d)\n\n");
    break;
  case 2:
    fprintf(
        STDOUT,
        "\n                                      Player 2 LOST! Do you want to play again? YES / NO?\n               "
        "                                   (change decision with a or d)\n\n");
    break;
  case 3:
    fprintf(
        STDOUT,
        "\n                                           Player 1 and 2 TIED! Want a rematch? YES / NO?\n               "
        "                                   (change decision with a or d)\n\n");
    break;
  default:
    break;
  }
  fprintf(STDOUT, "                                                       Y               N");
  fprintf(STDOUT, "\n\n                                                           PTS: ");
  char aux[7] = {0};
  fprintf(STDOUT, itoa(pts, aux, 10));

  char c = 0;
  char currentDecision = 1;
  while (c != '\n') {
    c = sys_get_char();
    switch (c) {
    case 'a':
      sys_draw_rectangle(0xFFFFFF, 439, 320, 8, 1);
      sys_draw_rectangle(0x000000, 568, 320, 8, 1);
      currentDecision = 1;
      break;
    case 'd':
      sys_draw_rectangle(0x000000, 439, 320, 8, 1);
      sys_draw_rectangle(0xFFFFFF, 568, 320, 8, 1);
      currentDecision = 2;
      break;
    default:
      break;
    }
  }
  if (currentDecision == 1) {
    eliminator();
  } else {
    sys_clear_screen();
  }
}

void print_pts(int pts) {
  sys_set_cursor(4, 0);
  fprintf(STDOUT, "PTS: ");
  char s[50] = {0};
  fprintf(STDOUT, itoa(pts, s, 10));
}

void play1() {
  int counter = 0;
  int lost = 0;
  int pts = 0;
  sys_clear_screen();

  width = sys_get_max_width();
  height = sys_get_max_height();

  sys_draw_rectangle(0x328fa8, 0, 16, width, 1);

  p1Coord[0] = width / 2;
  p1Coord[1] = 16;

  p1Dir = DOWN;
  p2Dir = UP;

  char c;
  while (lost == 0) {
    c = sys_get_char();
    change_dir(c);

    if (counter == 0) {
      move();
      lost = did_lose(1);
      sys_draw_rectangle(p1Color, p1Coord[0], p1Coord[1], 1, 1);
    }
    counter++;
    if (counter >= SPEED) {
      counter = 0;
      print_pts(pts);
      if (pts < MAX_POINTS) {
        pts++;
      }
    }
  }
  lose(lost, pts);
}

void play2() {
  int counter = 0;
  int lost = 0;
  int pts = 0;
  sys_clear_screen();

  width = sys_get_max_width();
  height = sys_get_max_height();

  sys_draw_rectangle(0x328fa8, 0, 16, width, 1);

  p1Dir = DOWN;
  p2Dir = UP;

  p1Coord[0] = width / 2;
  p1Coord[1] = 16;
  p2Coord[0] = width / 2;
  p2Coord[1] = height - 1;

  char c;
  while (lost == 0) {
    c = sys_get_char();
    change_dir(c);

    if (counter == 0) {
      move();
      lost = did_lose(2);
      sys_draw_rectangle(p1Color, p1Coord[0], p1Coord[1], 1, 1);
      sys_draw_rectangle(p2Color, p2Coord[0], p2Coord[1], 1, 1);
    }
    counter++;
    if (counter >= SPEED) {
      counter = 0;
      print_pts(pts);
      if (pts < MAX_POINTS) {
        pts++;
      }
    }
  }
  lose(lost, pts);
}

void eliminator() {
  for (int i = 0; i < 4; i++) {
    sys_scale_down();
  }
  sys_clear_screen();
  sys_set_cursor_to_line(15);
  fprintf(
      STDOUT,
      "\n                                      Please select the amount of players and press ENTER\n                 "
      "                               (change decision with a or d)\n\n");
  fprintf(STDOUT, "                                                       1               2");
  play_dragon_melody();
  char c = 0;
  char currentDecision = 1;
  while (c != '\n') {
    c = sys_get_char();
    switch (c) {
    case 'a':
      sys_draw_rectangle(0xFFFFFF, 439, 320, 8, 1);
      sys_draw_rectangle(0x000000, 568, 320, 8, 1);
      currentDecision = 1;
      break;
    case 'd':
      sys_draw_rectangle(0x000000, 439, 320, 8, 1);
      sys_draw_rectangle(0xFFFFFF, 568, 320, 8, 1);
      currentDecision = 2;
      break;
    default:
      break;
    }
  }
  currentDecision == 1 ? play1() : play2();
}