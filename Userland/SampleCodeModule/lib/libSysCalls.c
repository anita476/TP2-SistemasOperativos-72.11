// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <libSysCalls.h>
#include <_loader.h>

extern uint64_t syscall(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);

uint64_t readBuffer(int fileDes, char *buffer, uint64_t length) {
  return syscall(0, (uint64_t) fileDes, (uint64_t) buffer, length, 0, 0);
}

uint64_t fprintf(fd fileDes, char *buffer) { return syscall(1, fileDes, (uint64_t) buffer, (uint64_t) strlen(buffer), 0, 0); }

void putChar(char c) {
  char aux[2] = {c, 0};
  syscall(1, 1, (uint64_t) aux, 0, 0, 0);
}

uint64_t getTime() { return syscall(2, 0, 0, 0, 0, 0); }

void clearLine(uint64_t line) { syscall(6, line, 0, 0, 0, 0); }

void clearScreen() { syscall(7, 0, 0, 0, 0, 0); }

void drawRectangle(uint64_t color, uint64_t posX, uint64_t posY, uint64_t width, uint64_t height) {
  syscall(9, color, posX, posY, width, height);
}

void scaleUp() { syscall(10, 0, 0, 0, 0, 0); }

void scaleDown() { syscall(11, 0, 0, 0, 0, 0); }

void make_sound(int note, int duration, int wait) {
  syscall((uint64_t) 12, (uint64_t) note, (uint64_t) duration, (uint64_t) wait, 0, 0);
}

uint64_t getMaxHeight() { return syscall(13, 0, 0, 0, 0, 0); }

uint64_t getMaxWidth() { return syscall(14, 0, 0, 0, 0, 0); }

uint64_t getPixelColor(uint64_t x, uint64_t y) { return syscall(15, x, y, 0, 0, 0); }

void setCursorToLine(uint64_t line) { syscall(17, line, 0, 0, 0, 0); }

char getRegisters(uint64_t *buffer) { return syscall(18, (uint64_t) buffer, 0, 0, 0, 0); }

char getChar() {
  char c = syscall(19, 0, 0, 0, 0, 0);
  return c;
}

void setCursor(uint64_t posx, uint64_t line) { syscall(36, posx, line, 0, 0, 0); }

void *malloc(size_t bytes) { return (void *) syscall(20, bytes, 0, 0, 0, 0); }

void free(void *ptr) { syscall(21, (uint64_t) ptr, 0, 0, 0, 0); }

pid createProcess(createProcessInfo *info) { return (pid) syscall(22, (uint64_t) info, 0, 0, 0, 0); }

pid getpid() { return (pid) syscall(23, 0, 0, 0, 0, 0); }

int kill(pid pid) { return syscall(24, (uint64_t) pid, 0, 0, 0, 0); }

int block(pid pid) { return syscall(25, (uint64_t) pid, 0, 0, 0, 0); }

int unblock(pid pid) { return syscall(26, (uint64_t) pid, 0, 0, 0, 0); }

void yield() { syscall(27, 0, 0, 0, 0, 0); }

int setPriority(pid pid, priority priority) { return syscall(28, (uint64_t) pid, (uint64_t) priority, 0, 0, 0); }

int listProcessesInfo(ProcessInfo *processes, int max_proc) {
  return syscall(29, (uint64_t) processes, (uint64_t) max_proc, 0, 0, 0);
}

void exit_process() { syscall(30, 0, 0, 0, 0, 0); }

void waitForChildren() { syscall(31, 0, 0, 0, 0, 0); }

int getProcessInfo(pid pid, ProcessInfo *processInfo) {
  return syscall(32, (uint64_t) pid, (uint64_t) processInfo, 0, 0, 0);
}

int isForeground(pid pid) { return syscall(33, (uint64_t) pid, 0, 0, 0, 0); }

int nice(pid pid, priority newPrio) { return syscall(34, (uint64_t) pid, (uint64_t) newPrio, 0, 0, 0); }

void wait(uint64_t ms) { syscall(35, (uint64_t) ms, 0, 0, 0, 0); }

sem sem_open(sem_name semName, int initValue) {
  return (sem) syscall(37, (uint64_t) semName, (uint64_t) initValue, 0, 0, 0);
}

int sem_close(sem sem) { return syscall(38, (uint64_t) sem, 0, 0, 0, 0); }

int sem_post(sem sem) { return syscall(39, (uint64_t) sem, 0, 0, 0, 0); }

int sem_wait(sem sem) { return syscall(40, (uint64_t) sem, 0, 0, 0, 0); }

void memory_manager_state() { syscall(41, 0, 0, 0, 0, 0); }

int open_pipe(unsigned int pipe_id) { return syscall(42, (uint64_t) pipe_id, 0, 0, 0, 0); }
int close_pipe(unsigned int pipe_id) { return syscall(43, (uint64_t) pipe_id, 0, 0, 0, 0); }

int read_from_pipe(unsigned int pipe_id, char *dest, unsigned int bytes) {
  return syscall(44, (uint64_t) pipe_id, (uint64_t) dest, (uint64_t) bytes, 0, 0);
}
int write_to_pipe(unsigned int pipe_id, char *src, unsigned int bytes) {
  return syscall(45, (uint64_t) pipe_id, (uint64_t) src, (uint64_t) bytes, 0, 0);
}
int get_pipe_info(unsigned int pipe_id, pipeInfo *info) {
  return syscall(46, (uint64_t) pipe_id, (uint64_t) info, 0, 0, 0);
}

void waitForPID(pid pid) { syscall(47, (uint64_t) pid, 0, 0, 0, 0); }
