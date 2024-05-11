#ifndef LIST_GRAPH_H
#define LIST_GRAPH_H

#include "linked_list.h"

struct list_graph_t
{
	linked_list_t** neighbors;
	int nodes;
};

list_graph_t* lg_create(int nodes);

void lg_add_edge(list_graph_t* graph, int src, int dest);

ll_node_t *find_node(linked_list_t *ll, int node, unsigned int *pos);

int lg_has_edge(list_graph_t* graph, int src, int dest);

linked_list_t* lg_get_neighbours(list_graph_t* graph, int node);

void lg_remove_edge(list_graph_t* graph, int src, int dest);

void lg_free(list_graph_t* graph);

#endif