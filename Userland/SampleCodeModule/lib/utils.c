// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "utils.h"

char *itoa(int num, char *str, int base) {
  int i = 0;
  char isNegative = 0;

  if (num == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return str;
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

  return str;
}

void sprintf(char *dest, const char *format, int number) {
  int dest_pos = 0;
  int fmt_pos = 0;

  while (format[fmt_pos] != '\0') {
    if (format[fmt_pos] == '%' && format[fmt_pos + 1] == 'd') {
      char num_str[20];
      itoa(number, num_str, 10);

      int i = 0;
      while (num_str[i] != '\0') {
        dest[dest_pos++] = num_str[i++];
      }

      fmt_pos += 2;
    } else {
      dest[dest_pos++] = format[fmt_pos++];
    }
  }

  dest[dest_pos] = '\0';
}