- implemented a priority queue to use for the scheduler 
- used a max-heap implementation of a priority queue due to the fact that the most used operations would be 
    1) enqueue (insert new data into queue) - O(log(n))
    2) dequeue (remove element with highest priority) - O(log(n))
    3) remove an element - O(n)
Even though the dequeieing has a higher time complexity than a priority queue made with a linked list (O(1)), we decided to go for the array implementation because of its ... ? scalability? easy? idk

- priority queue implemented as a max-heap.
- a max heap is where the parent node (first element @index 0) is always greater or equal to its children 

       (8,101)
      /       \
  (6,102)    (7,103)
  /     \    /     \
(3,104)(4,105)(2,106)(5,107)

Inserting (9,108) puts it at the root 

       (9,108)
      /       \
  (8,101)    (7,103)
  /     \    /     \
(6,102)(4,105)(2,106)(5,107)
  /
(3,104)



taken from:
https://www.geeksforgeeks.org/c-program-to-implement-priority-queue/

max-heap priority queue is generally more scalable and efficient for larger systems!!

- added folders for better organization both in kernel and userland and changed their respective Makefiles


- in irqDispatcher added a schedulerTick to the timerTick so that it decrements the priority after each tick 