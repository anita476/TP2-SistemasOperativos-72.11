#include <sys/types.h>
#include <semaphores.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>


/* for critical region entering */
extern void acquire(uint8_t * lock);
extern void release(uint8_t * lock);

uint8_t lock = 1; // global lock to access semaphore resources and to guarantee no two processes try to grab resources at the same time

typedef struct Semaphore{
	sem_name name;
	int sem_value;
	uint8_t lock;
    pid interestedProcesses[MAX_PROCESSES];
    /* 
    not necesary to use queues because max quantity of processes is 10, 
    maybe we can chaange it later tho */
    int numberInterestedProcesses; 
}Semaphore;

Semaphore semaphoreList[MAX_SEMAPHORES] = {{0}};
int active = 0;



static int grabSemaphore(sem sem) {
    acquire(&lock);

    if (sem < 0 || sem > MAX_SEMAPHORES ) {
        release(&lock);
        return INVALID_VALUE_ERROR;
    }
    acquire(&(semaphoreList[sem].lock));
    release(&lock);
    return 0;
}



int sem_open(sem_name semName, int initValue ){
	pid currentPid = getpid();
	sem semId = sem_get(semName);
	
	if(semId == -1){ //need to create sem 
		print("Didnt find name\n");
		if(active == MAX_PROCESSES){
			return MAX_SEMS_ERROR;
		}
		if(initValue < 0){
			return INVALID_VALUE_ERROR;
		}
		// i traverse the array to find the first available spot 
		for( int i = 0; i< MAX_SEMAPHORES; i++){
			if(semaphoreList[i].numberInterestedProcesses == 0) {//if all processes have closed the semaphore, w can step on the name and reuse the id
				semaphoreList[i].name = malloc(strlen(semName )+1);
				if(semaphoreList[i].name == NULL){
					return -1;
				}
				if(strcpy(semaphoreList[i].name, semName) == NULL){
					return -1;
				}
				semaphoreList[i].sem_value = initValue;
				semaphoreList[i].interestedProcesses[0] = currentPid;
				semaphoreList[i].numberInterestedProcesses++;
				semaphoreList[i].lock = 1;
				active++;
				return i;
			}
		}
	}
	else{
		print("Sem already exists\n");
		//sem already exists -> init value is ignored because the sem was NOT really created and it has its own value;
		for(int i = 0; i< semaphoreList[semId].numberInterestedProcesses; i++){
			if(semaphoreList[semId].interestedProcesses[i] == currentPid){
				return 0; //pid has already opened the semaphore and not closed it
			}
		}
		semaphoreList[semId].interestedProcesses[semaphoreList[semId].numberInterestedProcesses] = currentPid; 
		semaphoreList[semId].numberInterestedProcesses++;
	}
	return semId;
}


int sem_close(sem  sem){
	pid currentPid = getpid();
	if(sem < 0 ){
		return INVALID_VALUE_ERROR;
	}
	for(int i = 0; i< semaphoreList[sem].numberInterestedProcesses; i++) {
		if(semaphoreList[sem].interestedProcesses[i] == currentPid){  //first process is always the calling process
			semaphoreList[sem].numberInterestedProcesses--;
			if(semaphoreList[sem].numberInterestedProcesses == 0){
				print("I have freed the name\n");
				free(semaphoreList[sem].name);
				semaphoreList[sem].sem_value = 0;
			}
			else{
			//advance all
				for(int j = i +1 ; j < semaphoreList[sem].numberInterestedProcesses; j++){
					semaphoreList[sem].interestedProcesses[j-1] =  semaphoreList[sem].interestedProcesses[j];
				}
			}
			return 0;
		}
	}
	return -1;
}

int sem_post(sem  sem){
	if (grabSemaphore(sem) != 0) {
        return -1;
    }
    semaphoreList[sem].sem_value ++;
	if(semaphoreList[sem].numberInterestedProcesses >1){
		int currentProc = semaphoreList[sem].interestedProcesses[0];
		for(int i = 0;i< semaphoreList[sem].numberInterestedProcesses ; i++ ){
			semaphoreList[sem].interestedProcesses[i] = semaphoreList[sem].interestedProcesses[i +1];
		}
		semaphoreList[sem].interestedProcesses[semaphoreList[sem].numberInterestedProcesses -1] = currentProc;
		unblock( semaphoreList[sem].interestedProcesses[0] );
	}
	
    release(&semaphoreList[sem].lock);
    return 0;
}

int sem_wait ( sem sem){
	if(grabSemaphore(sem) != 0){
		return -1;
	}

	if(semaphoreList[sem].sem_value > 0){
		semaphoreList[sem].sem_value--; 
	}
	else{
		pid currentPid = getpid();
		block(currentPid); 
		release(&(semaphoreList[sem].lock));
		yield();
		return 0;
	}
	release(&(semaphoreList[sem].lock));
	return 0;
}

int sem_value( sem sem){
    if(sem < 0 || sem > MAX_PROCESSES){
		return -1;
	}
	return semaphoreList[sem].sem_value;
} 

sem sem_get(sem_name semName){
	for(int i = 0; i< active; i++){
		if(strcmp(semaphoreList[i].name, semName) == 0){
			return i; //number in list is the "id" we use
		}
	}
	return -1;
}
