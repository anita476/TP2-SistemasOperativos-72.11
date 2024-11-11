// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <betterCallcaOS.h>
#include <idtLoader.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <processes.h>
#include <scheduler.h>
#include <sound.h>
#include <stdint.h>
#include <string.h>
#include <syscallHandler.h>
#include <time.h>
#include <videoDriver.h>

extern void _cli();
extern void _sti();
extern void _hlt();

extern uint64_t reset_main();
extern uint64_t show_registers[17];

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const startHeapAddres = (void *) 0xF00000;
static void *const endHeapAddres = (void *) 0x2000000;

typedef int (*EntryPoint)();

void clear_BSS(void *bssAddress, uint64_t bssSize) { memset(bssAddress, 0, bssSize); }

void *get_stack_base() {
  return (void *) ((uint64_t) &endOfKernel + PageSize * 8  // The size of the stack itself, 32KiB
                   - sizeof(uint64_t)                      // Begin at the top of the stack
  );
}

void *initialize_kernel_binary() {
  void *moduleAddresses[] = {sampleCodeModuleAddress};
  load_modules(&endOfKernelBinary, moduleAddresses);
  clear_BSS(&bss, &endOfKernel - &bss);
  return get_stack_base();
}

void init_shell() {
  createProcessInfo shellInfo = {.name = "shell",
                                 .fg_flag = 1,
                                 .priority = MAX_PRIORITY,
                                 .start = (ProcessStart) sampleCodeModuleAddress,
                                 .argc = 0,
                                 .argv = (const char *const *) NULL,
                                 .input = STDIN,
                                 .output = STDOUT};
  create_process(&shellInfo);
}

void welcome_sequence() {
  for (int i = 0; i < 4; i++) {
    scale_down();
  }
  scale_up();
  set_cursor_line(8);
  print(STDOUT, "                        Welcome to caOS!\n");
  put_saul(450, 300, 125, 125);
  play_better_call_saul();
  set_cursor_line(23);
  print(STDOUT, " Starting...");
  wait(1000);
  scale_down();
  clear_screen();
}

int main() {
  _cli();
  load_IDT();
  welcome_sequence();
  init_memory_manager(startHeapAddres, (size_t) (endHeapAddres - startHeapAddres));
  init_scheduler();
  init_shell();
  _sti();
  while (1) {
    yield();
    _hlt();
  }

  return 0;
}
