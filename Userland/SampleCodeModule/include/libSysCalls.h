#include <stdint.h>
#include <unistd.h>
#include <defs.h>

uint64_t readBuffer(char* buffer, uint64_t length);

char getChar();
void putChar(char c);                               
uint64_t print(char* buffer);                       

uint64_t getTime();

void clearScreen();
void clearLine(uint64_t line);

void drawRectangle(uint64_t color, uint64_t posX, uint64_t posY, uint64_t width, uint64_t height);

void scaleUp();
void scaleDown();

void make_sound(int note, int duration, int wait);

uint64_t getMaxHeight();
uint64_t getMaxWidth();

uint64_t getPixelColor(uint64_t x, uint64_t y);

void setCursorToLine(uint64_t line);

char getRegisters(uint64_t * buffer);

void setCursor(uint64_t posx, uint64_t line);

void * malloc(size_t bytes);

void free(void * ptr);

pid createProcess(createProcessInfo * info);
pid getpid();
int kill(pid pid);
int block(pid pid);
int unblock(pid pid);
void yield();
int setPriority(pid pid, priority priority);
int listProcessesInfo(ProcessInfo * processes, int max_proc);
void exit_process();


void waitForChildren();
sem sem_open(sem_name semName, int initValue);
int sem_close(sem sem);
int sem_post(sem sem);
int sem_wait(sem sem);
