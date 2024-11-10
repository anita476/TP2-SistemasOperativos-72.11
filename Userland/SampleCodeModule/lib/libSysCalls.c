// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <_loader.h>
#include <libSysCalls.h>

extern uint64_t syscall(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);

uint64_t sys_read(int fileDes, char *buffer, uint64_t length) {
  return syscall(0, (uint64_t) fileDes, (uint64_t) buffer, length, 0, 0);
}

uint64_t fprintf(fd fileDes, char *buffer) {
  return syscall(1, fileDes, (uint64_t) buffer, (uint64_t) strlen(buffer), 0, 0);
}

void sys_put_char(char c) {
  char aux[2] = {c, 0};
  syscall(1, 1, (uint64_t) aux, 0, 0, 0);
}

uint64_t sys_get_time() { return syscall(2, 0, 0, 0, 0, 0); }

void sys_clear_line(uint64_t line) { syscall(6, line, 0, 0, 0, 0); }

void sys_clear_screen() { syscall(7, 0, 0, 0, 0, 0); }

void sys_draw_rectangle(uint64_t color, uint64_t posX, uint64_t posY, uint64_t width, uint64_t height) {
  syscall(9, color, posX, posY, width, height);
}

void sys_scale_up() { syscall(10, 0, 0, 0, 0, 0); }

void sys_scale_down() { syscall(11, 0, 0, 0, 0, 0); }

void sys_make_sound(int note, int duration, int wait) {
  syscall((uint64_t) 12, (uint64_t) note, (uint64_t) duration, (uint64_t) wait, 0, 0);
}

uint64_t sys_get_max_height() { return syscall(13, 0, 0, 0, 0, 0); }

uint64_t sys_get_max_width() { return syscall(14, 0, 0, 0, 0, 0); }

uint64_t sys_get_pixel_color(uint64_t x, uint64_t y) { return syscall(15, x, y, 0, 0, 0); }

void sys_set_cursor_to_line(uint64_t line) { syscall(17, line, 0, 0, 0, 0); }

char sys_get_registers(uint64_t *buffer) { return syscall(18, (uint64_t) buffer, 0, 0, 0, 0); }

char sys_get_char() {
  char c = syscall(19, 0, 0, 0, 0, 0);
  return c;
}

void sys_set_cursor(uint16_t posx, uint16_t line) { syscall(36, posx, line, 0, 0, 0); }

void *sys_malloc(size_t bytes) { return (void *) syscall(20, bytes, 0, 0, 0, 0); }

void sys_free(void *ptr) { syscall(21, (uint64_t) ptr, 0, 0, 0, 0); }

pid sys_create_process(createProcessInfo *info) { return (pid) syscall(22, (uint64_t) info, 0, 0, 0, 0); }

pid sys_get_pid() { return (pid) syscall(23, 0, 0, 0, 0, 0); }

int sys_kill(pid pid) { return syscall(24, (uint64_t) pid, 0, 0, 0, 0); }

int sys_block(pid pid) { return syscall(25, (uint64_t) pid, 0, 0, 0, 0); }

int sys_unblock(pid pid) { return syscall(26, (uint64_t) pid, 0, 0, 0, 0); }

void sys_yield() { syscall(27, 0, 0, 0, 0, 0); }

int sys_set_priority(pid pid, priority priority) { return syscall(28, (uint64_t) pid, (uint64_t) priority, 0, 0, 0); }

int sys_list_processes_info(ProcessInfo *processes, int max_proc) {
  return syscall(29, (uint64_t) processes, (uint64_t) max_proc, 0, 0, 0);
}

void sys_exit_process() { syscall(30, 0, 0, 0, 0, 0); }

void sys_wait_for_children() { syscall(31, 0, 0, 0, 0, 0); }

int sys_get_process_info(pid pid, ProcessInfo *processInfo) {
  return syscall(32, (uint64_t) pid, (uint64_t) processInfo, 0, 0, 0);
}

int sys_is_foreground(pid pid) { return syscall(33, (uint64_t) pid, 0, 0, 0, 0); }

int sys_nice(pid pid, priority newPrio) { return syscall(34, (uint64_t) pid, (uint64_t) newPrio, 0, 0, 0); }

void sys_wait(uint64_t ms) { syscall(35, (uint64_t) ms, 0, 0, 0, 0); }

sem sys_sem_open(sem_name semName, int initValue) {
  return (sem) syscall(37, (uint64_t) semName, (uint64_t) initValue, 0, 0, 0);
}

int sys_sem_close(sem sem) { return syscall(38, (uint64_t) sem, 0, 0, 0, 0); }

int sys_sem_post(sem sem) { return syscall(39, (uint64_t) sem, 0, 0, 0, 0); }

int sys_sem_wait(sem sem) { return syscall(40, (uint64_t) sem, 0, 0, 0, 0); }

memoryInfo *sys_get_memory_info() { return (memoryInfo *) syscall(41, 0, 0, 0, 0, 0); }

int sys_open_pipe(unsigned int pipe_id) { return syscall(42, (uint64_t) pipe_id, 0, 0, 0, 0); }

int sys_close_pipe(unsigned int pipe_id) { return syscall(43, (uint64_t) pipe_id, 0, 0, 0, 0); }

int sys_read_from_pipe(unsigned int pipe_id, char *dest, unsigned int bytes) {
  return syscall(44, (uint64_t) pipe_id, (uint64_t) dest, (uint64_t) bytes, 0, 0);
}

int sys_write_to_pipe(unsigned int pipe_id, char *src, unsigned int bytes) {
  return syscall(45, (uint64_t) pipe_id, (uint64_t) src, (uint64_t) bytes, 0, 0);
}

int sys_get_pipe_info(unsigned int pipe_id, pipeInfo *info) {
  return syscall(46, (uint64_t) pipe_id, (uint64_t) info, 0, 0, 0);
}

void sys_wait_for_pid(pid pid) { syscall(47, (uint64_t) pid, 0, 0, 0, 0); }
