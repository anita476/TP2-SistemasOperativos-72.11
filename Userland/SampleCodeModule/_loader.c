// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* _loader.c */
#include <_loader.h>

extern char bss;
extern char endOfBinary;

int main();

void *memset(void *destiny, int32_t c, uint64_t length);

int _start() {
  memset(&bss, 0, &endOfBinary - &bss);
  return main();
}

void *memset(void *destiation, int32_t c, uint64_t length) {
  uint8_t chr = (uint8_t) c;
  char *dst = (char *) destiation;
  while (length--) {
    dst[length] = chr;
  }
  return destiation;
}

size_t strlen(const char *str) {
  size_t l;
  for (l = 0; *str != 0; str++, l++)
    ;
  return l;
}

char *strcpy(char *dst, const char *src) {
  char *w;
  for (w = dst; *src != '\0'; *(w++) = *(src++))
    ;
  *w = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *) s1 - *(const unsigned char *) s2;
}

char *strcat(char *dst, const char *src) {
  char *rdest = dst;
  while (*dst) {
    dst++;
  }
  while ((*dst++ = *src++))
    ;
  return rdest;
}

int strncmp(const char *s1, const char *s2, int n) {
  for (int i = 0; i < n && s1[i] != 0 && s2[i] != 0; i++) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
  }
  return 0;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
  uint64_t i;
  if ((uint64_t) destination % sizeof(uint32_t) == 0 && (uint64_t) source % sizeof(uint32_t) == 0 &&
      length % sizeof(uint32_t) == 0) {
    uint32_t *d = (uint32_t *) destination;
    const uint32_t *s = (const uint32_t *) source;
    for (i = 0; i < length / sizeof(uint32_t); i++) {
      d[i] = s[i];
    }
  } else {
    uint8_t *d = (uint8_t *) destination;
    const uint8_t *s = (const uint8_t *) source;
    for (i = 0; i < length; i++) {
      d[i] = s[i];
    }
  }
  return destination;
}