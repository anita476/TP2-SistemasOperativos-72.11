// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "utils.h"

char *itoa(int num, char *str, int base) {
  int i = 0;
  char isNegative = 0;

  // If the number is zero...
  if (num == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  // If the number is negative...
  if (num < 0 && base == 10) {
    isNegative = 1;
    num = -num;
  }

  while (num != 0) {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  // If the number is negative...
  if (isNegative)
    str[i++] = '-';

  str[i] = '\0';

  // Reverse the string
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

  // Copy format string until we find %d
  while (format[fmt_pos] != '\0') {
    if (format[fmt_pos] == '%' && format[fmt_pos + 1] == 'd') {
      // Found %d, convert number and copy it
      char num_str[20];
      itoa(number, num_str, 10);

      // Copy the number string
      int i = 0;
      while (num_str[i] != '\0') {
        dest[dest_pos++] = num_str[i++];
      }

      fmt_pos += 2;  // Skip the %d
    } else {
      // Copy regular character
      dest[dest_pos++] = format[fmt_pos++];
    }
  }

  // Null terminate
  dest[dest_pos] = '\0';
}