// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <interrupts.h>
#include <lib.h>
#include <time.h>
#include <videoDriver.h>

extern uint64_t get_minutes();
extern uint64_t get_seconds();
extern uint64_t get_hours();

static unsigned long ticks = 0;

void timer_handler() { ticks++; }

int ticks_elapsed() { return ticks; }

int millis_elapsed() { return ticks * (1000 / 18); }

// Rounding up slightly
int seconds_elapsed() { return ticks / 18; }

void timer_wait(int delta) {
  int initialTicks = ticks;
  while (ticks - initialTicks < delta) {
    _hlt();
  }
}
