
#include <lib.h>
#include <stdint.h>

#define MAX_SEMAPHORES 255
#define MAX_SEM_LENGTH 16
#define MAX_SEMS_ERROR -2

#define INVALID_VALUE_ERROR -3

typedef int sem; /* Sem IDs can be a max of 255 */
typedef char *sem_name;

/*
    @brief Opens a semaphore
    @param semName The name of the semaphore to open
    @param initValue The initial value of the semaphore
    @returns The semaphore
*/
int sem_open(sem_name semName, int initValue);

/*
    @brief Closes a semaphore
    @param sem The semaphore to close
    @returns 0 if successful, -1 if error
*/
int sem_close(sem sem);

/*
    @brief Posts a semaphore
    @param sem The semaphore to post
    @returns 0 if successful, -1 if error
*/
int sem_post(sem sem);

/*
    @brief Waits for a semaphore
    @param sem The semaphore to wait for
    @returns 0 if successful, -1 if error
*/
int sem_wait(sem sem);

/*
    @brief Gets the value of a semaphore
    @param sem The semaphore to get the value of
    @returns The value of the semaphore
*/
int sem_value(sem sem);

/*
    @brief Gets a semaphore
    @param semName The name of the semaphore to get
    @returns The semaphore
*/
sem sem_get(sem_name semName);
