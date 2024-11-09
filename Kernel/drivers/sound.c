// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Soundcode source: https://wiki.osdev.org/PC_Speaker

#include <lib.h>
#include <sound.h>
#include <time.h>

extern void outb(uint32_t param1, uint32_t param2);
extern uint8_t inb(uint16_t param1);

// Play sound using built in speaker
void play_sound(uint32_t nFrequence) {
  uint32_t Div;
  uint8_t tmp;

  nFrequence *= 0.5;

  // Sets the sound frequence
  Div = 1193180 / nFrequence;
  outb(0x43, 0xb6);
  outb(0x42, (uint8_t) (Div));
  outb(0x42, (uint8_t) (Div >> 8));

  // Plays the sound in the speakers
  tmp = inb(0x61);
  if (tmp != (tmp | 3)) {
    outb(0x61, tmp | 3);
  }
}

// Makes it stop
void no_sound() {
  uint8_t tmp = inb(0x61) & 0xFC;
  outb(0x61, tmp);
}

// Makes a beep
void beep() {
  play_sound(1000);
  timer_wait(10);
  no_sound();
}

void play_note_sound(int freq, int duration, int wait) {
  play_sound(freq);
  timer_wait(duration);
  no_sound();
  timer_wait(wait);
}

void play_melody() {
  play_note_sound(DO, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(MI, 10, 0);
  play_note_sound(SOL, 10, 0);
  play_note_sound(SOL, 10, 0);

  play_note_sound(MI, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(DO, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(MI, 10, 0);
  timer_wait(10);

  play_note_sound(DO, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(MI, 10, 0);
  play_note_sound(SI, 10, 0);
  play_note_sound(DO2, 10, 1);

  play_note_sound(DO2, 10, 1);
  play_note_sound(SI, 10, 0);
  play_note_sound(LA, 10, 0);
  play_note_sound(SOL, 10, 0);
  play_note_sound(LA, 10, 0);
  play_note_sound(LA, 10, 0);
  play_note_sound(SOL, 10, 0);
  timer_wait(10);

  play_note_sound(DO, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(MI, 10, 0);
  play_note_sound(SOL, 10, 0);
  play_note_sound(SOL, 10, 0);

  play_note_sound(MI, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(DO, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(DO, 10, 0);
  timer_wait(10);

  play_note_sound(DO, 10, 0);
  play_note_sound(RE, 10, 0);
  play_note_sound(MI, 10, 0);
  play_note_sound(SOL, 10, 0);

  play_note_sound(SOL, 10, 0);
  play_note_sound(FA, 10, 0);
  play_note_sound(FA, 10, 0);
  play_note_sound(MI, 10, 0);

  play_note_sound(DO2, 10, 1);
  play_note_sound(MI, 10, 0);
  play_note_sound(MI, 10, 0);
  play_note_sound(RE, 10, 0);
}

void play_better_call_saul() {
  play_note_sound(MI, 22, 1);
  play_note_sound(RE, 5, 0);
  play_note_sound(MI, 5, 0);
  play_note_sound(DO2, 5, 0);
  play_note_sound(SOL, 4, 0);
  play_note_sound(MI, 4, 0);
  play_note_sound(DO2, 22, 1);
  play_note_sound(MI2, 5, 0);
  play_note_sound(RE2, 5, 0);
  play_note_sound(DO2, 5, 1);
  play_note_sound(LA, 20, 1);
}