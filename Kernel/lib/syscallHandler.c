// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <font.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <memoryManagement.h>
#include <pipe.h>
#include <processes.h>
#include <scheduler.h>
#include <semaphores.h>
#include <sound.h>
#include <stdint.h>
#include <time.h>
#include <videoDriver.h>

#include <syscallHandler.h>

// from interruptions we get the register array to read it
extern const uint64_t show_registers_dump[17];
extern const uint64_t has_regs;

extern uint16_t getHours();
extern uint16_t getMinutes();
extern uint16_t getSeconds();

// syscalls params:	RDI	RSI	RDX	R10	R8	R9
// syscallHandler:	RDI RSI RDX R10 R8  RAX
// params in C are:	RDI RSI RDX RCX R8  R9

void exit_process() {
  kill(getpid());
  yield();
}

uint64_t read(uint64_t fileDescriptor, uint64_t buffer, uint64_t length) {
  int pid = getpid();
  int whereFrom = get_process_input(pid);
  if (fileDescriptor != STDIN) {
    return 0;  // we dont support reading directly from a pipe in read !
  }
  if (whereFrom != STDIN) {
    return read_from_pipe(whereFrom, (char *) buffer, length);
  } else {
    // we probs wont use this but its wrong at the moment
    char *bufferPosition = (char *) buffer;
    int i = 0;
    char readCharacter;
    cleanRead();
    while (i < length && (readCharacter = getFromBuffer()) != '\0') {
      if (readCharacter == EOF_CHAR) {
        bufferPosition[i] = '\0';
        return i;
      }
    }
    bufferPosition[i] = '\0';
    return i;
  }
}

uint64_t write(uint64_t fileDescriptor, uint64_t buffer, uint64_t length) {
  int pid = getpid();
  int whereTo = get_process_output(pid);
  // Check if writing to pipe or to screen here !!
  if (whereTo != STDOUT) {
    return write_to_pipe(whereTo, (char *) buffer, length);
  } else {
    print(fileDescriptor, (char *) buffer);
  }
  return 0;
}

uint64_t get_current_time() {
  uint16_t hours = getHours();
  if (hours >= 3)
    hours -= 3;
  else if (hours == 2)
    hours = 23;
  else if (hours == 1)
    hours = 22;
  else if (hours == 0)
    hours = 21;
  return (uint64_t) ((hours * 10000) + (getMinutes() * 100) + (getSeconds()));
}

uint64_t elapsed_millis() { return millisElapsed(); }

uint64_t get_height_ch() { return getHeightChars(); }

uint64_t get_width_ch() { return getWidthChars(); }

void clear_line(uint64_t line) {
  int startY = lineToHeight(line);
  for (int i = 0; i < getWidthPixels(); i++) {
    int heightCounter = 0;
    while (heightCounter < CHAR_HEIGHT * getScale()) {
      putPixel(0x000000, i, heightCounter + startY);
      heightCounter++;
    }
  }
}

void clear_screen() { clearScreen(); }

uint64_t put_pixel(uint64_t color, uint64_t x, uint64_t y) {
  return putPixel((uint32_t) color, (uint64_t) x, (uint64_t) y);
}

uint64_t draw_rect(uint64_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height) {
  return drawRectangle((uint64_t) hexColor, (uint64_t) x, (uint64_t) y, (int) width, (int) height);
}

uint64_t scale_up() { return scaleUp(); }

uint64_t scale_down() { return scaleDown(); }

uint64_t make_sound(uint64_t freq, uint64_t duration, uint64_t wait) {
  playNoteSound((uint64_t) freq, (uint64_t) duration, (uint64_t) wait);
  return 1;
}

uint64_t get_height_pix() { return (uint64_t) getHeightPixels(); }

uint64_t get_width_pix() { return (uint64_t) getWidthPixels(); }

uint32_t get_pix(uint64_t x, uint64_t y) { return getPixelColor((uint64_t) x, (uint64_t) y); }

uint64_t get_max_lines() { return getHeightChars(); }

uint64_t set_cursor_to_line(uint64_t line) {
  if (line >= get_max_lines())
    return 1;
  setCursorLine(line);
  return 0;
}

uint64_t get_registers(uint64_t buffer) {
  if (has_regs) {
    for (int i = 0; i < 17; i++) {
      ((uint64_t *) buffer)[i] = show_registers_dump[i];
    }
    return 0;
  }
  return 1;
}

uint64_t wait(uint64_t millis) {
  int delta = millis * 18 / 1000;
  timer_wait(delta);
  return 0;
}

uint64_t get_char(uint64_t fileDescriptor) {
  if (fileDescriptor != STDIN)
    return 0;
  uint64_t c = getLastChar();
  return c;
}

void set_cursor(uint64_t posx, uint64_t line) { setCursor(posx, lineToHeight(line)); }

uint64_t syscallHandler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
  switch (rax) {
  case 0:
    return read(rdi, rsi, rdx);
  case 1:
    return write(rdi, rsi, rdx);
  case 2:
    return get_current_time();
  case 3:
    return elapsed_millis();
  case 4:
    return get_height_ch();
  case 5:
    return get_width_ch();
  case 6:
    clear_line(rdi);
    break;
  case 7:
    clear_screen();
    break;
  case 8:
    return put_pixel(rdi, rdi, rdx);
  case 9:
    return draw_rect(rdi, rsi, rdx, r10, r8);
  case 10:
    return scale_up();
  case 11:
    return scale_down();
  case 12:
    return make_sound(rdi, rsi, rdx);
  case 13:
    return get_height_pix();
  case 14:
    return get_width_pix();
  case 15:
    return get_pix(rdi, rsi);
  case 16:
    return get_max_lines();
  case 17:
    set_cursor_to_line(rdi);
    break;
  case 18:
    return get_registers(rdi);
  case 19:
    return get_char(rdi);
  case 20:
    return (uint64_t) malloc(rdi);
  case 21:
    free((void *) rdi);
    break;
  case 22:
    return (uint64_t) createProcess((void *) rdi);
  case 23:
    return (uint64_t) getpid();
  case 24:
    return (uint64_t) kill(rdi);
  case 25:
    return (uint64_t) block(rdi);
  case 26:
    return (uint64_t) unblock(rdi);
  case 27:
    yield();
    break;
  case 28:
    return setPriority(rdi, rsi);
  case 29:
    return listProcessesInfo((void *) rdi, rsi);
  case 30:
    exit_process();
    break;
  case 31:
    waitForChildren();
    break;
  case 32:
    return getProcessInfo(rdi, (void *) rsi);
  case 33:
    return isForeground(rdi);
  case 34:
    return nice(rdi, rsi);
  case 35:
    wait(rdi);
    break;
  case 36:
    set_cursor(rdi, rsi);
    break;
  case 37:
    return sem_open((void *) rdi, rsi);
  case 38:
    return sem_close(rdi);
  case 39:
    return sem_post(rdi);
  case 40:
    return sem_wait(rdi);
  case 41:
    memory_manager_state();
    break;
  case 42:
    return open_pipe(rdi);
  case 43:
    return close_pipe(rdi);
  case 44:
    return read_from_pipe(rdi, (char *) rsi, rdx);
  case 45:
    return write_to_pipe(rdi, (char *) rsi, rdx);
  case 46:
    return get_pipe_info(rdi, (void *) rsi);
  case 47:
    waitForPID(rdi);
    break;
  default:
    return 1;
  }
  return 0;
}