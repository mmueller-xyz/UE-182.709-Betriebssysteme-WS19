#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>

#define SHM_NAME "/11810852_RINGBUF"
#define SEM_FREE "/11810852_SEMFREE"
#define SEM_USED "/11810852_SEMUSED"
#define SEM_WRITE "/11810852_SEMWRITE"

#define MAX_EDGE 8 // Max amount of edges to be removed
#define BUF_SIZE 50 // 1 solution /w 8 edges ~ 76B => 4KiB/76B ~= 50 Entries
#define X_COLOR 3 // Specifies how many Colors are used

/** @brief represents an edge by the start and end vertex */
typedef struct edge {
    int src;            /** Source vertex */
    int dest;           /** Destination vertex */
} edge;

/** @brief represents a solution as a list of max. MAX_EDGE edges and the number of removed edges */
typedef struct solution {
    int removed;            /** Amount of edges removed in order to achieve a valid X-coloring */
    edge edges[MAX_EDGE];   /** List of edges that have been removed */
} solution;

typedef struct buffer {
    int read_ind;       /** read offset in the buffer */
    int write_ind;      /** write offset in the buffer */
    int workers;        /** count of workers contributing to the ringbuffer currently */ 
    volatile sig_atomic_t quit; /** Global signal for soft exit */
    solution queue[BUF_SIZE];   /** The Buffer to wirte to and read from */
} buffer;

int shmfd;              /** Filedisciptor of the shared memory */
char* pname;            /** Name of the Programm (argv[0]) */
sem_t *free_sem;        /** Semaphore indicating, how much free space is available in the ring buffer */
sem_t *used_sem;        /** Semaphore indicating, how much space has been used in the ring buffer */
sem_t *write_sem;       /** Semaphore indicating, wether it is safe to write to the ring buffer */
volatile sig_atomic_t local_quit; /** Local signal to quit */
buffer *ring_buf;

/**
 * @brief returns the last solution from the ring buffer and increments the read index
 * 
 * @return the read solution from the buffer
 */
solution read_buf();

/**
 * @brief writes to the top of the ring buffer and increments the write index
 * 
 * @param s the solution to write on the buffer
 */
void write_buf(solution s);

/** @brief initializes the shared memory. */
void setup_shm();

/** @brief Connects to an allready initialized shared memory. */
void load_shm();

/** @brief disconnects the programm from the shared memory. */
void disconnect_shm();

/** @brief Closes and frees the shared memory. */
void close_shm();

/** @brief Prints out one solution */
void printSolution(solution s);

/** @brief prints an error mesage and exits with EXIT_FAILURE */
void exitErr(char *err_msg);

/** @brief closes shm exits with EXIT_SUCCSESS */
void soft_exit();