// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libSysCalls.h> // getChar
#include <test_util.h> // for satoi
#include <commands.h>
#include <stdint.h>
#include "phylo.h"

/*
    conditions: 
    1. no deadlocks 
    2. no starvation (using even-odd method, there is possibility of starvation 
    for philosopher 0 if num_philosophers = ODD 
    since the last philosopher would have an even index 
    therefore they would need the same fork at the same time)
    3. no race conditions: can be avoided by using semaphores -> 
    when adding a philosopher, we have to lock/block the philosophers that are right and left of the new philosopher
*/

typedef int pid; 
typedef int sem_t; 

#define MAX_PHILOSOPHERS 10
#define MIN_PHILOSOPHERS 3
#define THINK_TIME 1000
#define EAT_TIME 800

#define MUTEX "phylo_mutex"

#define LEFT(id) (((id) + num_philosophers - 1) % num_philosophers)
#define RIGHT(id) (((id) + 1) % num_philosophers)

typedef enum {NONE = 0, THINKING, HUNGRY, EATING} State;

static char *PHILOSOPHER_NAMES[] = {
    "Plato", "Aristotle", "Socrates", "Kant", "Nietzsche",
    "Confucius", "Buddha", "Descartes", "Hume", "Locke",
    "Marx", "Sartre", "Wittgenstein", "Russell", "Hegel",
    "Spinoza", "Voltaire", "Epicurus", "Pythagoras", "Seneca"
};

static char *HELLO_MESSAGES[] = {
    "arrives at the table!",
    "joins the feast!",
    "takes a seat!",
    "appears!",
    "joins in!"
};

static char *GOODBYE_MESSAGES[] = {
    "has reached enlightenment...",
    "went to find the meaning of life...",
    "left to question reality elsewhere...",
    "went to think, therefore to be...",
    "departed to contemplate existence...",
};



typedef struct philosopher {
    pid pid;
    State state;
    State prev; 
    sem_t sem;
} philosopher_t;

static philosopher_t philosophers[MAX_PHILOSOPHERS];
static sem_t mutex; // For protecting shared state
static int num_philosophers = 0; // should this int be protected as well? 
static int running = 1; 

static void print_welcome(int id) {
    fprintf(STDOUT, "\n*** Philosopher ");
    fprintf(STDOUT, PHILOSOPHER_NAMES[id]);
    fprintf(STDOUT, " ");
    fprintf(STDOUT, HELLO_MESSAGES[id % 5]);
    fprintf(STDOUT, " *\n");
}

static void print_goodbye(int id) {
    fprintf(STDOUT, "\n>>> Philosopher ");
    fprintf(STDOUT, PHILOSOPHER_NAMES[id]);
    fprintf(STDOUT, " ");
    fprintf(STDOUT, GOODBYE_MESSAGES[id % 5]);
    fprintf(STDOUT, " <\n");
}

void sprintf(char *dest, const char *format, int number) {
    int dest_pos = 0;
    int fmt_pos = 0;
    
    // Copy format string until we find %d
    while(format[fmt_pos] != '\0') {
        if(format[fmt_pos] == '%' && format[fmt_pos + 1] == 'd') {
            // Found %d, convert number and copy it
            char num_str[20];
            itoa(number, num_str, 10);
            
            // Copy the number string
            int i = 0;
            while(num_str[i] != '\0') {
                dest[dest_pos++] = num_str[i++];
            }
            
            fmt_pos += 2;  // Skip the %d
        } else {
            // Copy regular character
            dest[dest_pos++] = format[fmt_pos++];
        }
    }
    
    // Null terminate
    dest[dest_pos] = '\0';
}


// static void cleanup_semaphores() {
//     sem_close(sem_open("phylo_mutex", 0)); 
//     for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
//         char sem_name[8];
//         sprintf(sem_name, "phy_%d", i);
//         sem_close(sem_open(sem_name, 0));
//     }
// }

// update_semaphore() {
//     if (num_philosophers >= MIN_PHILOSOPHERS) {

//     }
// }


static void display_table() {  
    if (num_philosophers < MIN_PHILOSOPHERS) return;

    int state_changed = 0;

    for (int i = 0; i < num_philosophers && !state_changed; i++) {
        // if (philosophers[i].state == NONE) return;
        if ((philosophers[i].state == EATING && philosophers[i].prev != EATING) || (philosophers[i].state != EATING && philosophers[i].prev == EATING)) {
            state_changed = 1; 
            break;
        }
    }

    if (state_changed) {
        // fprintf(STDOUT, "\n=== The Great Philosophical Dinner ===\n");
        char count_str[50];
        sprintf(count_str, "Philosophers: %d: ", num_philosophers);
        fprintf(STDOUT, count_str);        
        for (int i = 0; i < num_philosophers; i++) {
            philosophers[i].prev = philosophers[i].state;
            if (philosophers[i].state == EATING) {
                fprintf(STDOUT, "E  ");  
            } else {
                fprintf(STDOUT, ".  ");             
            }
        }
        fprintf(STDOUT, "\n");    
    }
}

static void try_to_eat(int phil_id) {
    // char buffer[50];
    // if (getpid() != philosophers[phil_id].pid) {
    //     sprintf(buffer, "ERR! Philosopher %d is trying to eat but is not the current process\n", phil_id);
    //     fprintf(STDERR, buffer);
    //     return;
    // }
    if (philosophers[phil_id].state == HUNGRY && philosophers[LEFT(phil_id)].state != EATING && philosophers[RIGHT(phil_id)].state != EATING) {
            philosophers[phil_id].state = EATING; 
            display_table();
            if (sem_post(philosophers[phil_id].sem) < 0) {
                char buffer[50];
                sprintf(buffer, "ERROR!!! Cannot post semaphore for philosopher %d\n", phil_id);
                fprintf(STDERR, buffer);
                sprintf(buffer, "Current pid is: %d\n", getpid());
                fprintf(STDERR, buffer);
            }
    }
}

static int philosopher_action(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(STDERR, "ERROR: Cannot create philosopher action because ID was not given\n");
        return -1;
    }
    int phil_id = satoi(argv[0]);
    char sem_name[8];
    sprintf(sem_name, "phy_%d", phil_id);

    if ((philosophers[phil_id].sem = sem_open(sem_name, 0)) < 0) {
        fprintf(STDERR, "ERROR: Cannot create new philosopher semaphore\n");
        return -1; 
    }
    
    if (sem_open(MUTEX, 1) != mutex) {
        fprintf(STDERR, "ERROR: Cannot open mutex semaphore\n");
        return -1;
    }

    sem_wait(mutex); 
    philosophers[phil_id].state = THINKING;
    philosophers[phil_id].prev = NONE;
    // display_table(); 
    sem_post(mutex); 

    // char buffer[50];
    // sprintf(buffer, "Philosopher %d joins the table\n", phil_id);
    // fprintf(STDOUT, buffer);
    int last_num_philosophers = num_philosophers;

    while (running) {
        sprintf(sem_name, "phy_%d", LEFT(phil_id));

        if (last_num_philosophers != num_philosophers) {
            // Open left neighbor's semaphore if it exists
            char buffer[50]; 
            sprintf(buffer, "Philosopher %d is trying to open ", phil_id); 
            fprintf(STDOUT, buffer);
            fprintf(STDOUT, "left semaphore ");
            fprintf(STDERR, sem_name);
            fprintf(STDOUT, "\n");
            if (LEFT(phil_id) < num_philosophers && LEFT(phil_id) >= 0) {
                if (sem_open(sem_name, 0) < 0) {
                    fprintf(STDERR, "ERROR: Cannot open left philosopher semaphore\n");
                    return -1;
                }
            }

            sprintf(sem_name, "phy_%d", RIGHT(phil_id));
            // Open right neighbor's semaphore if it exists
            sprintf(buffer, "Philosopher %d is trying to open ", phil_id); 
            fprintf(STDOUT, buffer);
            fprintf(STDOUT, "right semaphore ");
            fprintf(STDERR, sem_name);
            fprintf(STDOUT, "\n");
            
            if (RIGHT(phil_id) < num_philosophers) {
                if (sem_open(sem_name, 0) < 0) {
                    fprintf(STDERR, "ERROR: Cannot open right philosopher semaphore\n");
                    return -1;
                }
            }
            last_num_philosophers = num_philosophers;
        }        

        wait(THINK_TIME);
        
        // get hungry --> take forks 
        sem_wait(mutex); 
        philosophers[phil_id].state = HUNGRY;
        display_table();
        try_to_eat(phil_id); 
        sem_post(mutex); 

        // get permission to eat 
        sem_wait(philosophers[phil_id].sem);
        // permission granted, therefore can eat yum yum :p

        wait(EAT_TIME);

        // done eating, back to thinking 
        sem_wait(mutex); 
        philosophers[phil_id].state = THINKING;
        display_table();

        // see if neighbors can eat
        // char buffer[50];
        // sprintf(buffer, "Philosopher %d is done eating\n", phil_id);
        // fprintf(STDERR, buffer); 
        // sprintf(buffer, "Left tries to eat: %d\n", LEFT(phil_id));
        // fprintf(STDOUT, buffer);
        try_to_eat(LEFT(phil_id));
        // sprintf(buffer, "Right tries to eat: %d\n", RIGHT(phil_id));
        // fprintf(STDOUT, buffer);
        try_to_eat(RIGHT(phil_id));
        sem_post(mutex);

    }   
    return 0;
}


static int add_philosopher(int id) {
    if (num_philosophers >= MAX_PHILOSOPHERS) {
        fprintf(STDERR, "ERROR: ERROR: Maximum number of philosophers reached\n");
        return -1;
    }
    sem_wait(mutex);

    philosophers[id].state = NONE; 
    philosophers[id].prev = NONE;   

    char sem_name[8];
    sprintf(sem_name, "phy_%d", id);
    if ((philosophers[id].sem = sem_open(sem_name, 0)) < 0) {
        fprintf(STDERR, "ERROR: Cannot create new philosopher semaphore\n");
        return -1;
    }

    char name[8]; 
    sprintf(name, "phylo_%d", id);
    char id_str[10];
    itoa(id, id_str, 10);
    char *args[] = { id_str, NULL };
    // Create philosopher process
    
    int fg_flag = isForeground(getpid());

    createProcessInfo info = {
        .name = name,
        .fg_flag = fg_flag,
        .priority = DEFAULT_PRIORITY,
        .start = (ProcessStart)philosopher_action,
        .argc = 1,
        .argv = (const char *const *)args,
        .input = STDIN,
        .output = STDOUT
    };
    
    print_welcome(id);

    if ((philosophers[id].pid = createProcess(&info)) < 0) {
        // sem_close(philosophers[id].sem);
        fprintf(STDERR, "ERROR: Unable to create process\n");
        sem_post(mutex);
        return -1;
    }  
    // fprintf(STDERR, "ERROR: Added Philosopher \n");
    num_philosophers++;
    wait(THINK_TIME);
    display_table();
    sem_post(mutex);
    return 0;
}


static int remove_philosopher(int running_check) {
    if (num_philosophers <= MIN_PHILOSOPHERS && running_check) {
        fprintf(STDERR, "ERROR: Minimum number of philosophers reached\n");
        return -1;
    }

    sem_wait(mutex);
    int id = num_philosophers - 1;

    // Add debug prints
    // char buffer[50];

    // sprintf(buffer, "Attempting to remove philosopher %d\n", id);
    // fprintf(STDOUT, buffer); 
    // sprintf(buffer, "PID: %d\n", philosophers[id].pid);
    // fprintf(STDOUT, buffer);
    // sprintf(buffer, "State: %d\n", philosophers[id].state);
    // fprintf(STDOUT, buffer);

    if (philosophers[id].pid <= 0) {
        sem_post(mutex); 
        fprintf(STDERR, "ERROR: Invalid philosopher PID\n");
        return -1;
    }

    pid pid_to_kill = philosophers[id].pid;

    while (philosophers[LEFT(id)].state == EATING && philosophers[RIGHT(id)].state == EATING) {
        sem_post(mutex); 
        sem_wait(philosophers[id].sem);
        sem_wait(mutex);
    }

    if (kill(pid_to_kill) < 0) {
        fprintf(STDERR, "ERROR: Failed to kill philosopher\n");
        return -1;
    }

    // wait_pid ?? 
    // waitForChildren();

    if (sem_close(philosophers[id].sem) < 0) {
        fprintf(STDERR, "ERROR: Failed to close semaphore\n");
        return -1;
    } // im curious whether any process will be able to sem_close a sempahore that it didnt create
    
    philosophers[id].state = NONE;
    philosophers[id].prev = NONE;
    philosophers[id].pid = -1; 
    philosophers[id].sem = -1; 
    num_philosophers--;

    print_goodbye(id);

    display_table();

    // waitForChildren(); 
    
    sem_post(mutex); 

    // sprintf(buffer, "Killing process with PID %d\n", pid_to_kill);
    // fprintf(STDOUT, buffer);

    return 0;
}

static void cleanup_philosopher(int id) {
    if (id < 0 || id >= MAX_PHILOSOPHERS) return;
    
    // Close semaphore if it exists
    if (philosophers[id].sem >= 0) {
        sem_close(philosophers[id].sem);
    }
    
    // Reset all fields
    philosophers[id].pid = -1;
    philosophers[id].sem = -1;
    philosophers[id].state = NONE;
    philosophers[id].prev = NONE;
}

uint64_t phylo(uint64_t argc, char *argv[]) {
    if (argc != 1) {
        fprintf(STDERR, "ERROR: Usage: phylo <num_philosophers>\n");
        return -1;
    }

    running = 1; 
    num_philosophers = 0;

    int initial_philosophers = satoi(argv[0]);
    if (initial_philosophers < MIN_PHILOSOPHERS || initial_philosophers > MAX_PHILOSOPHERS) {
        fprintf(STDERR, "ERROR: Invalid number of philosophers (must be between 3 and 20 inclusive)\n");
        return -1;
    }

    if ((mutex = sem_open(MUTEX, 1)) < 0) {
        fprintf(STDERR, "ERROR: ERROR: Failed to create mutex\n");
        return -1;
    }
    
    fprintf(STDOUT, "Initial number of philosophers: "); 
    fprintf(STDOUT, argv[0]);
    fprintf(STDOUT, "\n");

    fprintf(STDOUT, "\n*****************************************\n");
    fprintf(STDOUT, "* Welcome to the Great Philosophical Feast! *\n");
    fprintf(STDOUT, "*     Where great minds eat and think     *\n");
    fprintf(STDOUT, "*****************************************\n\n");
    fprintf(STDOUT, "Available Actions:\n");
    fprintf(STDOUT, "  'a' - Invite another philosopher to dine\n");
    fprintf(STDOUT, "  'r' - Excuse a philosopher from the table\n");
    fprintf(STDOUT, "  's' - Check on our distinguished guests\n");
    fprintf(STDOUT, "  'c' - View the dinner table state\n");
    fprintf(STDOUT, "  'q' - End the philosophical feast\n\n");


    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        philosophers[i].pid = -1; 
        philosophers[i].state = NONE;
        philosophers[i].prev = NONE; 
    }

    for (int i = 0; i < initial_philosophers; i++) {
        if (add_philosopher(i) < 0) {
            fprintf(STDERR, "ERROR: Failed to create initial philosophers\n");

            while (--i >= 0) {
                remove_philosopher(0);
            }
            sem_close(mutex);
            return -1;
        }
    }

    // for (int i = 0; i < initial_philosophers; i++) {
    //     if (add_philosopher(i) < 0) {
    //         fprintf(STDERR, "ERROR: Failed to create initial philosophers\n");
    //         return -1;
    //     }
    // }

    
    char cmd = 0;
    while ( running && ((cmd = getChar()) != 'q')) {
        if (cmd == 'q') {
            running = 0;
            break;
        }
        switch (cmd) {
            case 'a':
                if (add_philosopher(num_philosophers) < 0) fprintf(STDERR, "ERROR: Failed to add philosopher\n");
                break;
            case 'r':
                if (remove_philosopher(1) < 0) { 
                    fprintf(STDERR, "ERROR: Failed to remove philosopher\n");
                } 
                // yield();
                break;
            case 's':
            sem_wait(mutex);
                ps();
                fprintf(STDOUT, "\n");
            sem_post(mutex);
                break;
            case 'c': 
                sem_wait(mutex);
                display_table();
                sem_post(mutex);
                break;
        }
        // yield();
    }
    running = 0;

    // cleanup 
    fprintf(STDOUT, "Cleaning up philosophers\n");
    while(num_philosophers > 0) {
        remove_philosopher(0);  
        // yield();
    }
    sem_close(mutex);
    fprintf(STDOUT, "Done\n");

    return 0;
}