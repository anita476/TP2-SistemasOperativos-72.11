// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <font.h>
#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>

#define OFFWHITE 0xdbdbdb
#define RED      0xFF0000

struct vbe_mode_info_structure {
  uint16_t attributes;   // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a
                         // linear frame buffer.
  uint8_t window_a;      // deprecated
  uint8_t window_b;      // deprecated
  uint16_t granularity;  // deprecated; used while calculating bank numbers
  uint16_t window_size;
  uint16_t segment_a;
  uint16_t segment_b;
  uint32_t win_func_ptr;  // deprecated; used to switch banks from protected mode without returning to real mode
  uint16_t pitch;         // number of bytes per horizontal line
  uint16_t width;         // width in pixels
  uint16_t height;        // height in pixels
  uint8_t w_char;         // unused...
  uint8_t y_char;         // ...
  uint8_t planes;
  uint8_t bpp;    // bits per pixel in this mode
  uint8_t banks;  // deprecated; total number of banks in this mode
  uint8_t memory_model;
  uint8_t bank_size;  // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
  uint8_t image_pages;
  uint8_t reserved0;

  uint8_t red_mask;
  uint8_t red_position;
  uint8_t green_mask;
  uint8_t green_position;
  uint8_t blue_mask;
  uint8_t blue_position;
  uint8_t reserved_mask;
  uint8_t reserved_position;
  uint8_t direct_color_attributes;

  uint32_t framebuffer;  // physical address of the linear frame buffer; write here to draw to the screen
  uint32_t off_screen_mem_off;
  uint16_t off_screen_mem_size;  // size of memory in the framebuffer but not being displayed on the screen
  uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

uint8_t scale = 1;

uint16_t cursorX = 0;
uint16_t cursorY = 0;

uint32_t textColor = OFFWHITE;

uint8_t get_scale() { return scale; }

uint16_t get_width_pixels() { return VBE_mode_info->width; }

uint16_t get_height_pixels() { return VBE_mode_info->height; }

uint16_t get_width_chars() { return get_width_pixels() / (CHAR_WIDTH * scale); }

uint16_t get_height_chars() { return get_height_pixels() / (CHAR_HEIGHT * scale); }

int set_scale(uint8_t newScale) {
  if (newScale > 4 || newScale < 1) {
    return 1;
  }
  scale = newScale;
  return 0;
}

int scale_up() { return set_scale(scale + 1); }

int scale_down() { return set_scale(scale - 1); }

int put_pixel(uint64_t hexColor, uint64_t x, uint64_t y) {
  if (x >= get_width_pixels() || y >= get_height_pixels()) {
    return 1;
  }
  uint8_t *framebuffer = (uint8_t *) (uintptr_t) VBE_mode_info->framebuffer;
  uint64_t offset = (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
  framebuffer[offset] = (hexColor) & 0xFF;
  framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
  framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
  return 0;
}

int draw_rectangle(uint32_t hexColor, uint64_t x, uint64_t y, int width, int height) {
  if (x + width > get_width_pixels() || y + height > get_height_pixels()) {
    return 1;
  }
  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      put_pixel(hexColor, i, j);
    }
  }
  return 0;
}

void clear_screen() {
  for (int i = 0; i < get_width_pixels(); i++) {
    for (int j = 0; j < get_height_pixels(); j++) {
      put_pixel(0x00000000, i, j);
    }
  }
  cursorX = 0;
  cursorY = 0;
}

uint32_t get_pixel_color(uint64_t x, uint64_t y) {
  uint8_t *framebuffer = (uint8_t *) (uintptr_t) VBE_mode_info->framebuffer;
  uint64_t offset = (x * (VBE_mode_info->bpp / 8)) + (y * VBE_mode_info->pitch);
  uint32_t hexColor = 0;
  hexColor |= framebuffer[offset];            // Blue
  hexColor |= framebuffer[offset + 1] << 8;   // Green
  hexColor |= framebuffer[offset + 2] << 16;  // Red
  return hexColor;
}

void set_cursor(uint16_t x, uint16_t y) {
  uint16_t maxX = get_width_pixels() - CHAR_WIDTH * scale;
  uint16_t maxY = get_height_pixels() - CHAR_HEIGHT * scale;
  if (x < maxX) {
    cursorX = x;
  } else {
    cursorX = maxX;
  }
  if (y < maxY) {
    cursorY = y;
  } else {
    cursorY = maxY;
  }
}

uint16_t line_to_height(unsigned int line) { return line * (CHAR_HEIGHT * scale); }

int height_to_line(uint16_t height) { return height / (CHAR_HEIGHT * scale); }

int set_cursor_line(unsigned int line) {
  if (line >= height_to_line(get_height_pixels())) {
    return 1;
  }
  set_cursor(cursorX, line_to_height(line));
  return 0;
}

int put_char(char c, uint64_t x, uint64_t y) {
  if (c < FIRST_CHAR || c > LAST_CHAR) {
    return 1;
  }
  const uint8_t *charGlyph = IBM_VGA_8x16_glyph_bitmap + 16 * (c - FIRST_CHAR);
  for (int i = 0; i < CHAR_HEIGHT; i++) {
    for (int j = 0; j < CHAR_WIDTH; j++) {
      uint32_t color = charGlyph[i] & (1 << (CHAR_WIDTH - 1 - j)) ? textColor : 0x000000;
      for (int scaleX = 0; scaleX < scale; scaleX++) {
        for (int scaleY = 0; scaleY < scale; scaleY++) {
          put_pixel(color, x + j * scale + scaleX, y + i * scale + scaleY);
        }
      }
    }
  }
  return 0;
}

int put_char_cursor(char c) {
  if (c == '\n') {
    new_line();
    return 0;
  }
  if (c == '\b') {
    if (cursorX >= CHAR_WIDTH * scale) {
      cursorX -= CHAR_WIDTH * scale;
      put_char_cursor(' ');
      cursorX -= CHAR_WIDTH * scale;
      return 0;
    }
    return 1;
  }
  int successFlag = put_char(c, cursorX, cursorY);
  if (successFlag == 0) {
    cursorX += CHAR_WIDTH * scale;
    if (cursorX >= get_width_pixels() - CHAR_WIDTH * scale) {
      new_line();
    }
  }
  return successFlag;
}

void print(fd fileDes, char *str) {
  if (fileDes == STDERR) {
    set_color(RED);
  } else {
    set_color(OFFWHITE);
  }
  if (is_foreground(get_pid()) || fileDes == STDERR) {  // if im printing an error always write to screen
    for (; *str != '\0'; str++) {
      put_char_cursor(*str);
    }
  }
}

void println(char *str) {
  if (is_foreground(get_pid())) {
    print(STDOUT, str);
    new_line();
  }
}

void set_color(uint32_t newColor) { textColor = newColor; }

void new_line() {
  cursorX = 0;
  if (cursorY + 2 * CHAR_HEIGHT * scale <= get_height_pixels()) {
    cursorY += CHAR_HEIGHT * scale;
  } else {
    void *dst = (void *) ((uint64_t) VBE_mode_info->framebuffer);
    void *src = (void *) (dst + 3 * (CHAR_HEIGHT * scale * (uint64_t) get_width_pixels()));
    uint64_t len = 3 * ((uint64_t) get_width_pixels() * (get_height_pixels() - CHAR_HEIGHT * scale));
    memcpy(dst, src, len);
    memset(dst + len, 0, 3 * (uint64_t) get_width_pixels() * CHAR_HEIGHT * scale);
  }
}