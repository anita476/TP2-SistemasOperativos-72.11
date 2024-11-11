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
#include <syscallHandler.h>
#include <time.h>
#include <videoDriver.h>

extern const uint64_t show_registers_dump[17];
extern const uint64_t has_regs;

extern uint16_t get_hours();
extern uint16_t get_minutes();
extern uint16_t get_seconds();

// syscalls params:	RDI	RSI	RDX	R10	R8	R9
// syscallHandler:	RDI RSI RDX R10 R8  RAX
// params in C are:	RDI RSI RDX RCX R8  R9

void exit_process() {
  kill(get_pid());
  yield();
}

uint64_t read(uint64_t fileDescriptor, uint64_t buffer, uint64_t length) {
  int pid = get_pid();
  int whereFrom = get_process_input(pid);
  if (fileDescriptor != STDIN) {
    return 0;  // We don't support reading directly from a pipe in read!
  }
  if (whereFrom != STDIN) {
    open_pipe(whereFrom);
    int res = read_from_pipe(whereFrom, (char *) buffer, length);
    close_pipe(whereFrom);
    return res;
  } else {
    clean_read();
    while (!is_key_available()) {
      add_to_blocking_queue_read(pid);
    }
    return get_buffer((char *) buffer, length);
  }
}

uint64_t write(uint64_t fileDescriptor, uint64_t buffer, uint64_t length) {
  int pid = get_pid();
  int whereTo = get_process_output(pid);
  if (whereTo != STDOUT) {
    open_pipe(whereTo);
    int res = write_to_pipe(whereTo, (char *) buffer, length);
    close_pipe(whereTo);
    return res;
  } else {
    print(fileDescriptor, (char *) buffer);
  }
  return 0;
}

uint64_t get_current_time() {
  uint16_t hours = get_hours();
  if (hours >= 3) {
    hours -= 3;
  } else if (hours == 2) {
    hours = 23;
  } else if (hours == 1) {
    hours = 22;
  } else {
    hours = 21;
  }
  return ((uint64_t) hours * 10000) + ((uint64_t) get_minutes() * 100) + (uint64_t) get_seconds();
}

uint64_t elapsed_millis() { return millis_elapsed(); }

uint64_t get_height_ch() { return get_height_chars(); }

uint64_t get_width_ch() { return get_width_chars(); }

void clear_line(uint64_t line) {
  int startY = line_to_height(line);
  for (int i = 0; i < get_width_pixels(); i++) {
    int heightCounter = 0;
    while (heightCounter < CHAR_HEIGHT * get_scale()) {
      put_pixel(0x000000, i, heightCounter + startY);
      heightCounter++;
    }
  }
}

uint64_t draw_rect(uint64_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height) {
  return draw_rectangle((uint64_t) hexColor, (uint64_t) x, (uint64_t) y, (int) width, (int) height);
}

uint64_t make_sound(uint64_t freq, uint64_t duration, uint64_t wait) {
  play_note_sound((uint64_t) freq, (uint64_t) duration, (uint64_t) wait);
  return 1;
}

uint64_t get_height_pix() { return (uint64_t) get_height_pixels(); }

uint64_t get_width_pix() { return (uint64_t) get_width_pixels(); }

uint32_t get_pix(uint64_t x, uint64_t y) { return get_pixel_color((uint64_t) x, (uint64_t) y); }

uint64_t set_cursor_to_line(uint64_t line) {
  if (line >= get_height_chars()) {
    return 1;
  }
  set_cursor_line(line);
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
  if (fileDescriptor != STDIN) {
    return 0;
  }
  uint64_t c = get_last_char();
  return c;
}
uint64_t put_pixel_handler(uint64_t color, uint64_t x, uint64_t y) { return (uint64_t) put_pixel(color, x, y); }

typedef uint64_t (*SyscallFunction)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

static SyscallFunction sysCallTable[] = {
  (SyscallFunction) read, 
  (SyscallFunction) write,
  (SyscallFunction) get_current_time,
  (SyscallFunction) elapsed_millis,
  (SyscallFunction) get_height_ch,
  (SyscallFunction) get_width_ch,
  (SyscallFunction) clear_line,
  (SyscallFunction) clear_screen,
  (SyscallFunction) put_pixel_handler,
  (SyscallFunction) draw_rect,
  (SyscallFunction) scale_up,
  (SyscallFunction) scale_down,
  (SyscallFunction) make_sound,
  (SyscallFunction) get_height_pix,
  (SyscallFunction) get_width_pix,
  (SyscallFunction) get_pix,
  (SyscallFunction) get_height_chars,
  (SyscallFunction) set_cursor_to_line,
  (SyscallFunction) get_registers,
  (SyscallFunction) get_char,
  (SyscallFunction) malloc,
  (SyscallFunction) free,
  (SyscallFunction) create_process,
  (SyscallFunction) get_pid,
  (SyscallFunction) kill,
  (SyscallFunction) block,
  (SyscallFunction) unblock,
  (SyscallFunction) yield,
  (SyscallFunction) set_priority,
  (SyscallFunction) list_processes_info,
  (SyscallFunction) exit_process,
  (SyscallFunction) wait_for_children,
  (SyscallFunction) get_process_info,
  (SyscallFunction) is_foreground,
  (SyscallFunction) nice,
  (SyscallFunction) wait,
  (SyscallFunction) set_cursor,
  (SyscallFunction) sem_open,
  (SyscallFunction) sem_close,
  (SyscallFunction) sem_post,
  (SyscallFunction) sem_wait,
  (SyscallFunction) get_memory_info,
  (SyscallFunction) open_pipe,
  (SyscallFunction) close_pipe,
  (SyscallFunction) read_from_pipe,
  (SyscallFunction) write_to_pipe,
  (SyscallFunction) get_pipe_info,
  (SyscallFunction) wait_for_pid,
};

uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
  return sysCallTable[rax](rdi, rsi, rdx, r10, r8);
}