#include "ringBuffer.h"
#include "graph.h"
#include <time.h> 

Graph *graph;
int *colors;

int random_color();
void generate_color_set();

/**
 * @details Sets global variable 'quit' to 1 so the programm can safely close after all connections have been served
 * 
 * @param signal the singal beeing handled
 */
void handle_soft_exit(int signal) {
    local_quit++;
}


/** @brief closes resources and exits with EXIT_SUCCSESS */
void soft_exit() {    
    fprintf(stderr, "\r[%s] Closing.\n", pname);
    fflush(stderr);
    freeGraph(graph);
    disconnect_shm();
    exit(EXIT_SUCCESS);
}

/** @brief prints the usage message and exits with EXIT_FAILURE */
void usage() {
    exitErr("\t Error:\nSYNOPSIS\n\tgenerator EDGE1...\nEXAMPLE\n\tgenerator 0-1 0-2 0-3 1-2 1-3 2-3\n");
}

/** 
 * @brief parses the arguments from main() and generates the Graph
 * 
 * @param argc argument counter
 * @param argv argument vector
 */
void parse_inputs(int argc, char* argv[]) {
    pname = argv[0];
    if (argc < 2)
        usage();
    graph = newGraph();

    for (int i = 1; i < argc; i++) {
        //printf("%s\n", argv[i]);
        edge e;
        if (sscanf(argv[i], "%i-%i", &e.src, &e.dest) != 2) usage();
        if (e.src < 0 || e.dest < 0) usage();

        addEdge(graph, e.src, e.dest);
    }
}

/** @brief generates a random color set for all vertices */
void generate_color_set() {
    colors = malloc(sizeof(int)*graph->max_vertex+1);
    for (size_t i = 0; i <= graph->max_vertex; i++)
        colors[i] = random_color();
}

/** @brief generates a random number from 0 to 1-X_COLOR */
int random_color() {
    return rand() % X_COLOR;
}

/** 
 * @brief generates a colorset and a solution for it. If the solution is good enough, it gets written to the ring buffer.
 * 
 * @details Solutions are calculated, by iterating through all source verticies and compares their color to all connected destinations. 
 * If the color is the same, the edge gets added to the solution and the counter is incremented. 
 * 
 * If the counter surpasses MAX_EDGE, the solution is deemed too bad and is discarded (it is vital to free the colors, as the heap would be flooded otherwise).
 * Else the solution is posted to the ring buffer.
 */
void generate_solution() {
    generate_color_set();

    solution s;
    s.removed = 0;
    for (G_edge_list *el = graph->v_head; el != NULL; el = el->next_vertex) {
        for (G_edge *e = el->e_head; e != NULL; e = e->next_edge) {
            if (colors[el->src] == colors[e->dest]) {
                if (s.removed >= MAX_EDGE) {
                    free(colors);
                    return;
                }
                s.edges[s.removed].src = el->src;
                s.edges[s.removed].dest = e->dest;
                s.removed++;
            }
        }
    }

    if (s.removed <= MAX_EDGE) {
        printGraphC(graph, colors);
        write_buf(s);
    }
    free(colors);
}


int main(int argc, char* argv[]) {
    parse_inputs(argc, argv);
    load_shm();

    /* each worker ideally has a different seed, else all workers output the same solutions */
    srand(time(0) + ring_buf->workers * 0xBEEF);

    /* Set signal handler */
    struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_soft_exit;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* main controll loop, the worker can either be terminated itself or be terminated by the supervisor 
        in case it gets terminated, all other workers and the supervisor continue. */
    while (!ring_buf->quit && !local_quit){
        generate_solution();
    }

    soft_exit();
}