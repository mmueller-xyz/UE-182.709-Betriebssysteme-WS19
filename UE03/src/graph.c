#include "graph.h"

/** @brief mallocs an edge and initializes it */
G_edge* newEdge(int dest);

/** @brief mallocs an edgelist and initializes it */
G_edge_list* newEdgeList(int src, int dest);

/** @brief prints ansi colorcodes */
void printColor(int color);

Graph* newGraph(){
    Graph *g = malloc(sizeof(struct Graph));
    g->v_head=NULL;
    g->max_vertex = 0;
    return g;
}


void freeGraph(Graph *g){
    G_edge_list *el = g->v_head;
    free(g);
    while (el != NULL) {
        G_edge *e = el->e_head;

        while (e != NULL) {
            G_edge *e_2 = e->next_edge;
            free(e);
            e=e_2;
        }
        
        G_edge_list *el_2 = el->next_vertex;
        free(el);
        el = el_2;
    }
}


void addEdge(Graph *graph, int src, int dest){
    if (src > dest) {
        int a = src;
        src = dest;
        dest = a;
    }

    if (dest > graph->max_vertex) 
        graph->max_vertex = dest;
    
    G_edge_list *el = graph->v_head;

    /* Graph is empty -> first vertex */
    if (el == NULL){ 
        el = newEdgeList(src, dest);
        graph->v_head = el;
        fflush(stderr);
        return;
    }

    while (el->src != src){ 
        if (el->next_vertex == NULL) {
            /* if vertex has never been a source, add it to the list */
            el->next_vertex = newEdgeList(src, dest);
            el = el->next_vertex;
            fflush(stderr);
            return;
        } else el = el->next_vertex;
    }
    
    /* vertex has allready been a source => add destination to its EdgeList */
    G_edge *e = el->e_head;
    if (e->dest == dest) return;
    while (e->next_edge != NULL) {
        /* Edge allready exists */
        if (e->next_edge->dest == dest) return;
        e = e->next_edge;
    }
    
    e->next_edge = newEdge(dest);
}

G_edge* newEdge(int dest){
    G_edge *e = malloc(sizeof(struct G_edge));
    e->dest = dest;
    e->next_edge = NULL;
    return e;
}

G_edge_list* newEdgeList(int src, int dest){
    G_edge_list *el = malloc(sizeof(struct G_edge_list)); 
    el->src = src;
    el->next_vertex = NULL;
    el->e_head = newEdge(dest);
    return el;
}

void printGraph(Graph* graph){
    fprintf(stderr, "Graph:\n");
    for (G_edge_list *el = graph->v_head; el != NULL; el = el->next_vertex) {
        fprintf(stderr, "%i:\t", el->src);
        for (G_edge *e = el->e_head; e != NULL; e = e->next_edge) {
            fprintf(stderr, "%i -> ", e->dest);
        }
        fprintf(stderr, "NULL\n");
    }
    fprintf(stderr, "NULL\n");
    fflush(stderr);
}

void printColor(int color){
    switch (color)
    {
    case 0:
        fprintf(stderr, "\033[0;31m");
        break;
    case 1:
        fprintf(stderr, "\033[0;32m");
        break;
    case 2:
        fprintf(stderr, "\033[0;34m");
        break;
    default:
        fprintf(stderr, "\033[0m");
        break;
    }
}

void printGraphC(Graph* graph, int *colors){
    fprintf(stderr, "Graph:\n");
    for (G_edge_list *el = graph->v_head; el != NULL; el = el->next_vertex) {
        printColor(colors[el->src]);
        fprintf(stderr, "%i:\t", el->src);
        printColor(-1);
        for (G_edge *e = el->e_head; e != NULL; e = e->next_edge) {
            printColor(colors[e->dest]);
            fprintf(stderr, "%i -> ", e->dest);
            printColor(-1);
        }
        fprintf(stderr, "NULL\n");
    }
    fprintf(stderr, "NULL\n");
    fflush(stderr);
}