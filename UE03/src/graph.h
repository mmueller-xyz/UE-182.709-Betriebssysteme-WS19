/**
 * This represents an undirected Graph as a modified adjacency list, 
 * where the adjacency is stored at the "smaller" vertex to save space 
 * and querry time.
 */

#include <stdio.h> 
#include <stdlib.h> 

/** @brief Represents the edge between the source, which is saved in G_edge_list, and the destination */
typedef struct G_edge {
    int dest;                   /** Destination vertex */
    struct G_edge* next_edge;   /** Next Entry in the linked list */
} G_edge;

/** @brief  Holds all verticies leaving from a paticular source*/
typedef struct G_edge_list {
    int src;                            /** Source of an edge. */
    struct G_edge* e_head;              /** First destination element */
    struct G_edge_list* next_vertex;    /** Next source element */
} G_edge_list;

/** @brief  */
typedef struct Graph {
    int max_vertex;                 /** index of the "biggest" vertex */
    struct G_edge_list* v_head;     /** Start of the adjacency list */
} Graph;


/** 
 * @brief Generates a new Graph
 * 
 * @return ponter to a new Graph struct
 */
Graph* newGraph();

/** @brief frees a Graph and all its resources */
void freeGraph(Graph *g);

/** 
 * @brief adds an edge to the Datastructure
 * @details adds an edge to the Datastructure, duplicates are ignored. src is allways the smaller integer (automatic)
 * 
 * @param graph the graph to add the edge to
 * @param src index of the src vertex
 * @param dest index if the dest vertex
 */
void addEdge(Graph* graph, int src, int dest);

/** @brief prints the entire graph as it is saved in memory */
void printGraph(Graph* graph);

/** @brief prints the entire graph as it is saved in memory and colors the verticies*/
void printGraphC(Graph* graph, int *colors);