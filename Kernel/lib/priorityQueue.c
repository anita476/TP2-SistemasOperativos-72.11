#include "priorityQueue.h"

static void swap(PQItem *a, PQItem *b) {
    PQItem temp = *a;
    *a = *b;
    *b = temp;
}

// maintains heap property during insertion 
static void heapifyUp(PriorityQueue* pq, int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2; 
        if (pq->heap[index].priority <= pq->heap[parentIndex].priority) {
            break; 
        }
        swap(&pq->heap[index], &pq->heap[parentIndex]);
        index = parentIndex;
    }
}

static void heapifyDown(PriorityQueue* pq, int index) {
    while (true) {
        int leftChildIndex = 2 * index + 1; 
        int rightChildIndex = 2 * index + 2;
        int largestIndex = index; 

        if (leftChildIndex < pq->size && pq->heap[leftChildIndex].priority > pq->heap[largestIndex].priority) {
            largestIndex = leftChildIndex;
        }

        if (rightChildIndex < pq->size && pq->heap[rightChildIndex].priority > pq->heap[largestIndex].priority) {
            largestIndex = rightChildIndex;
        }

        if (largestIndex == index) {
            break; 
        }

        swap(&pq->heap[index], &pq->heap[largestIndex]);
        index = largestIndex;
    }
} 

PriorityQueue* createPriorityQueue() {
    return (PriorityQueue*) malloc(sizeof(PriorityQueue));
}

void destroyPriorityQueue(PriorityQueue* pq) {
    free(pq);
}

bool priorityQueueIsEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}

bool priorityQueueIsFull(PriorityQueue* pq) {
    if (pq->size == MAX_QUEUE_SIZE) {
        print("Priority Queue is full\n");
        return true; 
    }
}

void priorityQueueInsert(PriorityQueue* pq, int priority, pid pid) {
    if (priorityQueueIsFull(pq)) {
        return; 
    }

    PQItem item = {priority, pid};
    pq->heap[pq->size] = item; 
    heapifyUp(pq, pq->size); 
    pq->size++;
}

// Pops the highest priority item
pid priorityQueuePop(PriorityQueue* pq) {
    if (priorityQueueIsEmpty(pq)) {
        print("Priority Queue is empty\n");
        return ERROR; 
    }

    pid highestPriorityPid = pq->heap[0].pid;
    pq->size--; 
    pq->heap[0] = pq->heap[pq->size];
    heapifyDown(pq, 0);

    return highestPriorityPid;
}

void priorityQueueRemove(PriorityQueue* pq, pid pid) {
    int i;
    for (int i = 0; i < pq->size; i++) {
        if (pq->heap[i].pid == pid) {
            break;
        }
    }

    // pid not found 
    if (i == pq->size) {
        print("PID not found in Priority Queue\n");
        return; 
    }

    pq->size--;
    pq->heap[i] = pq->heap[pq->size];
    heapifyDown(pq, i);
    heapifyUp(pq, i);
}