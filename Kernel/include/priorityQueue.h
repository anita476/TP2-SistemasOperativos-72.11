
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "processes.h" // just to include pid type 

#define ERROR -1

#define MAX_QUEUE_SIZE 256

typedef struct {
    pid pid;
    int priority;
} PQItem; 

typedef struct { 
    PQItem heap[MAX_QUEUE_SIZE];
    int size; 
} PriorityQueue; 

PriorityQueue* createPriorityQueue();
void destroyPriorityQueue(PriorityQueue* pq);
bool priorityQueueIsEmpty(PriorityQueue* pq);
bool priorityQueueIsFull(PriorityQueue* pq);
void priorityQueueInsert(PriorityQueue* pq, int priority, pid pid);
pid_t priorityQueuePop(PriorityQueue* pq);
void priorityQueueRemove(PriorityQueue* pq, pid pid);

#endif