// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>

void *memset(void *destination, int32_t c, uint64_t length) {
  uint8_t chr = (uint8_t) c;
  char *dst = (char *) destination;
  while (length--) {
    dst[length] = chr;
  }
  return destination;
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

void int_to_str(int num, char *str, int base) {
  int i = 0;
  char isNegative = 0;

  if (num == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  if (num < 0 && base == 10) {
    isNegative = 1;
    num = -num;
  }

  while (num != 0) {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  if (isNegative) {
    str[i++] = '-';
  }

  str[i] = '\0';

  int start = 0;
  int end = i - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
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

char *strcat(char *dest, const char *src) {
  char *rdest = dest;
  while (*dest) {
    dest++;
  }
  while ((*dest++ = *src++))
    ;
  return rdest;
}
