#include "ringBuffer.h"

solution top_sol; /** the best solution that the supervisor has processed */

/**
 * @details Sets global variable 'quit' to 1 so the programm can safely close after all connections have been served
 * 
 * @param signal the singal beeing handled
 */
void handle_soft_exit(int signal) {
    ring_buf->quit++;
    if (ring_buf->quit > 1){
        close_shm();
        exit(EXIT_FAILURE);
    }
    
}

void soft_exit() {
    fprintf(stderr, "\r[%s] Closing, waiting for %i workers.\n", pname, ring_buf->workers);
    fflush(stderr);
    while (ring_buf->workers > 0){}
    close_shm();
    exit(EXIT_SUCCESS);
}

/** @brief compares the last solution from the ringbuffer with the all-time-best solution and saves the better one */
void compare_solution() {
    solution s = read_buf();
    int i;
    sem_getvalue(free_sem, &i);
    fprintf(stderr, "\r%i\t Indices are free in the Buffer", i);
    if (s.removed < top_sol.removed) {
        top_sol = s;
        printSolution(s);
        if (s.removed == 0) {
            ring_buf->quit++;
        }
        
    }
}

int main(int argc, char* argv[]) {
    pname = argv[0];
    if (argc != 1)
        exitErr("Too many arguments");


    top_sol.removed=__INT_MAX__;

    /* Set signal handler */
    struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	sa.sa_handler = handle_soft_exit;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    setup_shm();

    while (!ring_buf->quit){
        compare_solution();
    }
    fprintf(stderr, "\n");
    soft_exit();
}