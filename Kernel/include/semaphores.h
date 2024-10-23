
#include <stdint.h>
#include <lib.h>

#define MAX_SEMAPHORES 255
#define MAX_SEM_LENGTH 16
#define MAX_SEMS_ERROR -2

#define INVALID_VALUE_ERROR -3

typedef int sem; /* sem ids can be a max of 255 */
typedef char * sem_name;

/* make initial value be 1 always */

int sem_open(sem_name semName, int initValue ); /* if id does not exist, create the id and return numerical id for future reference */
/* if it exists, add calling process to participating processes id */
/* default value for creating semaphore is 1 */

int sem_close(sem  sem); /* delete calling process from queue of participating processes*/

int sem_post(sem  sem);

int sem_wait ( sem sem);

int sem_value( sem sem); 

sem sem_get(sem_name semName);

/* for all processes check that calling process is in the interested processes list, so that unwanted processes cant access*/