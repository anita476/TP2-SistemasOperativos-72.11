// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "music.h"
#include "libSysCalls.h"

void error_sound() {
  sys_make_sound(261, 5, 0);
  sys_make_sound(329, 5, 0);
  sys_make_sound(195, 5, 0);
  sys_make_sound(130, 5, 0);
}

void play_dragon_melody() {
  sys_make_sound(659, 5, 1);
  sys_make_sound(659, 5, 1);
  sys_make_sound(659, 5, 1);
  sys_make_sound(523, 5, 1);
  sys_make_sound(659, 5, 1);
  sys_make_sound(783, 5, 2);
}

void play_wah_wah_wah() {
  sys_make_sound(195, 10, 2);
  sys_make_sound(184, 10, 2);
  sys_make_sound(174, 10, 2);
  sys_make_sound(146, 6, 0);
  sys_make_sound(138, 6, 0);
  sys_make_sound(146, 6, 0);
  sys_make_sound(138, 6, 0);
  sys_make_sound(146, 6, 0);
}