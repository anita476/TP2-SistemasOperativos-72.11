// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <pipe.h>
#include <videoDriver.h>

#define START_ID 4

typedef struct Pipe {
  unsigned int pipeID;  // Has to be bigger than 3

  sem read_sem;
  sem write_sem;
  sem_name read_sem_name;
  sem_name write_sem_name;

  unsigned int write_pos;
  unsigned int read_pos;
  uint8_t *pipe;
  unsigned int amount;

  uint8_t eof;

  int interested_processes;
} Pipe;

static Pipe pipeList[MAX_PIPES];
int active_pipes = 0;

// Because we use named semaphores, we need to give unique names to each one
static void create_sem_name(unsigned int pipe_id, char *buf, char *sem) {
  strcpy(buf, sem);
  char id[3];
  int_to_str(pipe_id, id, 10);
  strcat(buf, id);
}

static int find_available_pipe() {
  if (active_pipes == MAX_PIPES) {
    return NO_SPACE;
  }
  for (int i = 0; i < MAX_PIPES; i++) {
    if (pipeList[i].pipeID == 0) {
      int answer = i + START_ID;
      pipeList[i].pipeID = answer;
      return answer;
    }
  }
  return NO_SPACE;
}

static int find_pipe(unsigned int id) {
  if (id <= 3 || (pipeList[id - START_ID].pipeID != id)) {
    return INVALID_PIPE;
  }
  return (id - START_ID);
}

int get_pipe_info(unsigned int pipe_id, pipeInfo *info) {
  int pos = find_pipe(pipe_id);
  if (pos < 0) {
    return INVALID_PIPE;
  }
  info->amount = pipeList[pos].amount;
  info->id = pipeList[pos].pipeID;
  info->eof = pipeList[pos].eof;
  return 0;
}

int signal_eof(unsigned int pipe_id) {
  int pos = find_pipe(pipe_id);
  if (pos < 0) {
    return INVALID_PIPE;
  }
  pipeList[pos].eof = 1;
  return 0;
}

int open_pipe(unsigned int pipe_id) {
  int pos;
  if (pipe_id == 0) {
    int id = find_available_pipe();
    pos = find_pipe(id);
    if (id < 0 || pos < 0) {
      return -1;
    }
    char *bufW;
    char *bufR;
    if ((bufW = malloc(MAX_SEM_LENGTH + 1)) == NULL || (bufR = malloc(MAX_SEM_LENGTH + 1)) == NULL) {
      free(bufR);
      free(bufW);
      return -1;
    }

    create_sem_name(id, bufW, "semwrite");
    create_sem_name(id, bufR, "semread");
    int sem_write = sem_open(bufW, PIPE_SIZE);
    int sem_read = sem_open(bufR, 0);
    if (sem_write < 0 || sem_read < 0) {
      return -1;
    }

    if ((pipeList[pos].pipe = malloc(PIPE_SIZE)) == NULL) {
      free(bufR);
      free(bufW);
      free(pipeList[pos].pipe);
      return NO_SPACE;
    }

    pipeList[pos].pipeID = id;
    pipeList[pos].read_sem = sem_read;
    pipeList[pos].write_sem = sem_write;
    pipeList[pos].read_sem_name = bufR;
    pipeList[pos].write_sem_name = bufW;
    pipeList[pos].write_pos = 0;
    pipeList[pos].read_pos = 0;
    pipeList[pos].amount = 0;
    pipeList[pos].eof = 0;
    pipeList[pos].interested_processes = 1;

    active_pipes++;

    return id;
  } else {
    pos = find_pipe(pipe_id);
    if (pos < 0) {
      return INVALID_PIPE;
    }

    if (sem_open(pipeList[pos].read_sem_name, 1) < 0 || sem_open(pipeList[pos].write_sem_name, 1) < 0) {
      return NO_SPACE;
    }
    pipeList[pos].interested_processes++;
    return pipe_id;
  }
}

int read_from_pipe(unsigned int pipe_id, char *dest, unsigned int bytes) {
  int pos = find_pipe(pipe_id);
  if (pos == INVALID_PIPE)
    return PIPE_ERROR;

  if (pipeList[pos].eof && pipeList[pos].amount == 0) {
    return EOF;
  }
  int i;

  for (i = 0; i < bytes && !(pipeList[pos].eof && (pipeList[pos].amount == 0)); i++) {
    sem_wait(pipeList[pos].read_sem);

    dest[i] = pipeList[pos].pipe[pipeList[pos].read_pos];
    pipeList[pos].read_pos = ((pipeList[pos].read_pos) + 1) % 1024;
    pipeList[pos].amount--;

    sem_post(pipeList[pos].write_sem);
  }

  return i;
}

int write_to_pipe(unsigned int pipe_id, const char *src, unsigned int bytes) {
  int pos = find_pipe(pipe_id);
  if (pos == INVALID_PIPE) {
    return PIPE_ERROR;
  }

  for (int i = 0; i < bytes; i++) {
    sem_wait(pipeList[pos].write_sem);

    pipeList[pos].pipe[pipeList[pos].write_pos] = src[i];
    pipeList[pos].write_pos = ((pipeList[pos].write_pos) + 1) % 1024;
    pipeList[pos].amount++;
    sem_post(pipeList[pos].read_sem);
  }

  return bytes;
}

int close_pipe(unsigned int pipe_id) {
  int pos = find_pipe(pipe_id);
  if (pos == INVALID_PIPE) {
    return INVALID_PIPE;
  }
  sem_close(pipeList[pos].read_sem);
  sem_close(pipeList[pos].write_sem);

  pipeList[pos].interested_processes--;

  if (pipeList[pos].interested_processes == 0) {
    free(pipeList[pos].pipe);
    free(pipeList[pos].read_sem_name);
    free(pipeList[pos].write_sem_name);
    pipeList[pos].read_sem = 0;
    pipeList[pos].write_sem = 0;
    pipeList[pos].pipeID = 0;
    pipeList[pos].write_pos = 0;
    pipeList[pos].read_pos = 0;
    pipeList[pos].amount = 0;
    pipeList[pos].pipe = 0;
    active_pipes--;
  }
  return 0;
}
