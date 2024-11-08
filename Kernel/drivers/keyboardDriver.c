// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <interrupts.h>
#include <keyboardDriver.h>


#define BUFFER_SIZE 1024
#define EOF_CHAR    -1

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

char isKeyAvailable() { return (buffer[readIndex] != '\0'); }

// Waiting queue
pid waitingToRead[MAX_PROCESSES] = {0};
int waitingToReadCounter = 0;

void addToBlockingQueueRead(pid pid) {
  waitingToRead[waitingToReadCounter] = pid;
  waitingToReadCounter++;
  block(pid);
  yield();
}

void removeFromBlockingQueue() {
  if (waitingToReadCounter == 0) {
    return;
  }
  pid toUnblock = waitingToRead[0];
  for (int i = 0; i < waitingToReadCounter; i++){
    waitingToRead[i] = waitingToRead[i+1];
  }
  unblock(toUnblock);
}

void addToBuffer(char c) {
  // Resets the index if the buffer is full
  if (writeIndex >= BUFFER_SIZE)
    writeIndex = 0;
  buffer[writeIndex++] = c;
  lastIndexFlag = 1;
  // Here we check if there are processes waiting for read
  removeFromBlockingQueue();
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
      print(STDOUT, "^C\n");
      cleanBuffer();
      for (int i = 1; i < MAX_PROCESSES; i++) {
        if (isForeground(i) > 0) {
          killWithChildren(i);
          return;
        }
      }
      yield();
      return;
    }
    // Ctrl+D
    else if (ASCIIkey == 'd' || ASCIIkey == 'D') {
      addToBuffer(EOF_CHAR);
      print(STDOUT, "^D\n");
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

void cleanRead() { readIndex = writeIndex; }

char getKeyFromBuffer() {
  if (!isKeyAvailable())
    return 0;

  char c = buffer[readIndex];
  buffer[readIndex] = 0;

  readIndex = (readIndex + 1) % BUFFER_SIZE;

  return c;
}

unsigned int getBuffer(char *dest, unsigned int size) {
  int i = 0;
  while (isKeyAvailable() && i < size) {
    char c = getKeyFromBuffer();
    dest[i++] = c;
  }
  return i;
}

char getLastChar() {
  if (lastIndexFlag == 1) {
    lastIndexFlag = 0;
    return buffer[writeIndex - 1];
  }
  return 0;
}