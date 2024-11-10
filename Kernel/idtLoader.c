// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <defs.h>
#include <idtLoader.h>
#include <interrupts.h>

#pragma pack(push) /* Push current alignment */
#pragma pack(1)    /* Align the following structures to 1 byte */

typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop) /* Restore alignment */

DESCR_INT *idt = (DESCR_INT *) 0;  // IDT of 255 entries

static void setup_IDT_entry(int index, uint64_t offset);

void load_IDT() {
  // Divide by zero exception
  setup_IDT_entry(0x00, (uint64_t) &_exception0_handler);
  // Invalid opcode exception
  setup_IDT_entry(0x06, (uint64_t) &_exception6_handler);
  // Timer interruption
  setup_IDT_entry(0x20, (uint64_t) &_irq00_handler);
  // Keyboard interruption
  setup_IDT_entry(0x21, (uint64_t) &_irq01_handler);
  // Syscalls interruption
  setup_IDT_entry(0x80, (uint64_t) &_int80_handler);
  // scheduler
  setup_IDT_entry(0x81, (uint64_t) &_schedule);
  // Keyboard and timer interruptions enabled
  pic_master_mask(0xFC);
  pic_slave_mask(0xFF);
}

static void setup_IDT_entry(int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}
