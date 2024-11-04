// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "music.h"
#include "libSysCalls.h"

void errorSound() {
  make_sound(261, 5, 0);
  make_sound(329, 5, 0);
  make_sound(195, 5, 0);
  make_sound(130, 5, 0);
}

void playDragonMelody() {
  make_sound(659, 5, 1);
  make_sound(659, 5, 1);
  make_sound(659, 5, 1);
  make_sound(523, 5, 1);
  make_sound(659, 5, 1);
  make_sound(783, 5, 2);
}

void playWahWahWah() {
  make_sound(195, 10, 2);
  make_sound(184, 10, 2);
  make_sound(174, 10, 2);
  make_sound(146, 6, 0);
  make_sound(138, 6, 0);
  make_sound(146, 6, 0);
  make_sound(138, 6, 0);
  make_sound(146, 6, 0);
}