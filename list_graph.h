#ifndef LIST_GRAPH_H
#define LIST_GRAPH_H

#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description)            \
    do                                              \
    {                                               \
        if (assertion)                              \
        {                                           \
            fprintf(stderr, "(%s, %d): ", __FILE__, \
                    __LINE__);                      \
            perror(call_description);               \
            exit(errno);                            \
        }                                           \
    } while (0)

typedef struct list_graph_t
{
	linked_list_t** neighbors;
	int nodes;
	void **data;
} list_graph_t;

list_graph_t* lg_create();

void lg_add_node(list_graph_t *graph, void *data, unsigned int data_size);

void lg_add_edge(list_graph_t* graph, int src, int dest);

ll_node_t *find_node(linked_list_t *ll, int node, unsigned int *pos);

int lg_has_edge(list_graph_t* graph, int src, int dest);

linked_list_t* lg_get_neighbours(list_graph_t* graph, int node);

void lg_remove_edge(list_graph_t* graph, int src, int dest);

void lg_free(list_graph_t* graph);

void lg_print(list_graph_t *graph);

unsigned int lg_lowest_common_ancestor(list_graph_t *graph, int first, int second);

#endif