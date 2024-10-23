#include <sys/types.h>
#include <semaphores.h>
#include <processes.h>


/* for critical region entering */
extern void acquire(uint8_t * lock);
extern void release(uint8_t * lock);

uint8_t lock = 1;; 

typedef struct Semaphore{
	unsigned int sem_id;
	unsigned int sem_value;
	unsigned int lock;
    pid interestedProcesses[MAX_PROCESSES];
    /* 
    not necesary to use queues because max quantity of processes is 10, 
    maybe we can chaange it later tho */
    int numberInterestedProcesses; 
}Semaphore;

int sem_open(sem_name semName, int initValue ){

}

int sem_close(sem  sem){

}
int sem_post(sem  sem){

}

int sem_wait ( sem sem){

}

int sem_value( sem sem){
    
} 

sem sem_get(sem_name semName){

}

/* 
if(sem_id == 0)				// 0 is reserved to denote empty record
		return INVALID_SEM_ID;
	if(active_sem == MAX_SEMAPHORES)
		return ERROR_NO_MORE_SPACE;

	int freePos = -1;
	for(int i=0; i<MAX_SEMAPHORES; i++){
		if(freePos == -1 && sem_info[i].sem_id == 0){
			freePos = i;
		}
		if(sem_info[i].sem_id == sem_id){
			return INVALID_SEM_ID;
		}
	}
 */
