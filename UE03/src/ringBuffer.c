#include "ringBuffer.h"

solution read_buf() {
    while (sem_wait(used_sem) < 0)
        if (ring_buf->quit || local_quit) 
            soft_exit();
        else exitErr(strerror(errno));

    solution s = ring_buf->queue[ring_buf->read_ind++];
    ring_buf->read_ind %= BUF_SIZE;

    sem_post(free_sem);
    return s;
}

void write_buf(solution s) {
    while (sem_wait(write_sem) < 0) 
        if (ring_buf->quit || local_quit) 
            soft_exit();
        else exitErr(strerror(errno));

    if (ring_buf->quit || local_quit) {
        sem_post(write_sem);
        soft_exit();
    }
    
    while (sem_wait(free_sem) < 0)
        if (ring_buf->quit || local_quit) {
            sem_post(write_sem); 
            soft_exit();
        }

    ring_buf->queue[ring_buf->write_ind++] = s;
    ring_buf->write_ind %= BUF_SIZE; 

    sem_post(used_sem);
    sem_post(write_sem);
}

void setup_shm() {
    shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (shmfd == -1) 
        exitErr("Failed to setup shm! Check /dev/shm/ if files already exist.");    
    
    if (ftruncate(shmfd, sizeof(struct buffer)) < 0 )
        exitErr("failed to set shm size!");
    
    ring_buf = mmap(NULL, sizeof(*ring_buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (ring_buf == MAP_FAILED)
        exitErr(strerror(errno));

    ring_buf->quit = 0;
    ring_buf->workers = 0;

    free_sem = sem_open(SEM_FREE, O_CREAT|O_EXCL, 0600, BUF_SIZE);
    used_sem = sem_open(SEM_USED, O_CREAT|O_EXCL, 0600, 0);
    write_sem = sem_open(SEM_WRITE, O_CREAT|O_EXCL, 0600, 1);
}

void load_shm() {
    shmfd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shmfd == -1) 
        exitErr("Failed to setup shm! Has the Supervisor been started?");    
    
    ring_buf = mmap(NULL, sizeof(*ring_buf), PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (ring_buf == MAP_FAILED)
        exitErr(strerror(errno));

    free_sem = sem_open(SEM_FREE, 0);
    used_sem = sem_open(SEM_USED, 0);
    write_sem = sem_open(SEM_WRITE, 0);
    if (free_sem == SEM_FAILED || used_sem == SEM_FAILED || write_sem == SEM_FAILED)
        exitErr("Failed to open a semaphore! Has the Supervisor been started?");
    ring_buf->workers++;
}

void disconnect_shm() {
    ring_buf->workers--;
    munmap(ring_buf, sizeof(*ring_buf));
    close(shmfd);
    sem_close(free_sem);
    sem_close(used_sem);
    sem_close(write_sem);
}

void close_shm() {
    disconnect_shm();
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_FREE);
    sem_unlink(SEM_USED);
    sem_unlink(SEM_WRITE);
}

void printSolution(solution s) {
    if (s.removed == 0) 
        printf("\r[%s] The graph is %i-colorable!\n", pname, X_COLOR);
    else {
        printf("\r[%s] Solution with %i edges: ", pname, s.removed);
        for (int i = 0; (i < s.removed) && (i < MAX_EDGE); i++) 
            printf("%i-%i ", s.edges[i].src, s.edges[i].dest);
        printf("\n");
    }
    fflush(stdout);
}

void exitErr(char* exit_msg) {
    fprintf(stderr, "[%s] %s\n", pname, exit_msg);
    fflush(stderr);
    exit(EXIT_FAILURE);
}