// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <interrupts.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>

#define BUFFER_SIZE 1024
#define EOF_CHAR    4

// Assembly function
extern int getKey();

static const char keyboard[256] = {
    0,   1,   '1', '2', '3', '4',  '5', '6',  '7', '8', '9', '0', '\'', '<', '\b', '\t', 'q', 'w', 'e', 'r', 't',
    'y', 'u', 'i', 'o', 'p', '\\', '+', '\n', 0,   'a', 's', 'd', 'f',  'g', 'h',  'j',  'k', 'l', ';', '{', '|',
    5,   '}', 'z', 'x', 'c', 'v',  'b', 'n',  'm', ',', '.', '-', 0,    '*', 0,    ' ',  0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,    0,   0,    0,   17,  0,   0,   18,   0,   19,   0,    0,   20,  0,
};

// Buffer variables
static char buffer[BUFFER_SIZE] = {0};
static int writeIndex = 0;
static int readIndex = 0;
static int lastIndexFlag = 0;
static char ctrlFlag = 0;

// Flags
char enterFlag = 0;
char shiftFlag = 0;
char capsLockFlag = 0;

char isAlpha(char c) { return (c >= 'a' && c <= 'z'); }

void addToBuffer(char c) {
  // Resets the index if the buffer is full
  if (writeIndex >= BUFFER_SIZE)
    writeIndex = 0;
  buffer[writeIndex++] = c;
  lastIndexFlag = 1;
}

void keyboardHandler() {
  if (buffer[readIndex - 1] == '\n')
    cleanBuffer();
  if (buffer[readIndex - 1] == '\b')
    removeCharFromBuffer();

  unsigned char scancodeKey = getKey();
  char ASCIIkey = keyboard[scancodeKey];

  // Left Ctrl pressed
  if (scancodeKey == 0x1D) {
    ctrlFlag = 1;
    return;
  }
  // Left Ctrl released
  else if (scancodeKey == 0x9D) {
    ctrlFlag = 0;
    return;
  }

  if (ctrlFlag) {
    // Ctrl+C
    if (ASCIIkey == 'c' || ASCIIkey == 'C') {
      for (int i = 1; i < MAX_PROCESSES; i++) {
        if (isForeground(i)) {
          kill(i);
          print("^C\n");
          cleanBuffer();
          print("caOS>");
          return;
        }
      }
    }
    // Ctrl+D
    else if (ASCIIkey == 'd' || ASCIIkey == 'D') {
      addToBuffer(EOF_CHAR);
      print("^D\n");
      // cleanBuffer();
      print("caOS>");
      return;
    }
  }

  // Alternate capslock
  if (scancodeKey == 0x3A)
    capsLockFlag = !capsLockFlag;
  // Shift released
  else if (scancodeKey == 0xAA)
    shiftFlag = 0;
  else {
    switch (ASCIIkey) {
    // Key is not valid
    case 0:
      break;
    // Shift pressed
    case 5:
      shiftFlag = 1;
      break;
    // Key is 'enter' ot 'backspace'
    case '\n':
    case '\b':
      addToBuffer(ASCIIkey);
      break;
    // Key is valid
    default:
      // Caps
      if (isAlpha(ASCIIkey) && ((capsLockFlag != shiftFlag)))
        addToBuffer(ASCIIkey - 'a' + 'A');
      // Not caps
      else
        addToBuffer(ASCIIkey);
      break;
    }
  }
}

void removeCharFromBuffer() {
  if (writeIndex > 1) {
    // Removes '\b' and the character
    buffer[--writeIndex] = '\0';
    buffer[--writeIndex] = '\0';
  }
}

void cleanBuffer() {
  memset(buffer, '\0', writeIndex);
  writeIndex = 0;
  readIndex = 0;
}

void cleanRead() { readIndex = 0; }

char getFromBuffer() {
  if (readIndex == writeIndex)
    return 0;
  if (readIndex >= BUFFER_SIZE)
    readIndex = 0;
  return buffer[readIndex++];
}

char getLastChar() {
  if (lastIndexFlag == 1) {
    lastIndexFlag = 0;
    return buffer[writeIndex - 1];
  }
  return 0;
}