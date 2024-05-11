#include "list_graph.h"
#include <stdlib.h>
#include <math.h>

static int is_node_in_graph(int n, int nodes)
{
    return n >= 0 && n < nodes;
}

list_graph_t *
lg_create()
{
    list_graph_t *g = malloc(sizeof(*g));
    DIE(!g, "malloc graph failed");

    // g->neighbors = malloc(nodes * sizeof(*g->neighbors));
    // DIE(!g->neighbors, "malloc neighbours failed");

    // for (i = 0; i != nodes; ++i)
    //     g->neighbors[i] = ll_create(sizeof(int));
	g->neighbors = NULL;
    g->nodes = 0;

    return g;
}

void lg_add_node(list_graph_t *graph, void *data, unsigned int data_size) {
	if (!graph)
		return;
	
	graph->nodes++;
	if (graph->nodes > 1) {
		graph->neighbors = realloc(graph->neighbors, graph->nodes * sizeof(*graph->neighbors));
		graph->data = realloc(graph->data, graph->nodes * sizeof(void *));
	}
	else {
		graph->neighbors = malloc(graph->nodes * sizeof(*graph->neighbors));
		graph->data = malloc(graph->nodes * sizeof(void *));
	}
	graph->neighbors[graph->nodes - 1] = ll_create(sizeof(int));
    graph->data[graph->nodes - 1] = malloc(data_size);
	memcpy(graph->data[graph->nodes - 1], data, data_size);
} 

void lg_add_edge(list_graph_t *graph, int src, int dest)
{
    if (!graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return;

    ll_add_nth_node(graph->neighbors[src], graph->neighbors[src]->size, &dest);
}

ll_node_t *find_node(linked_list_t *ll, int node, unsigned int *pos)
{
    ll_node_t *crt = ll->head;
    unsigned int i;

    for (i = 0; i != ll->size; ++i)
    {
        if (node == *(int *)crt->data)
        {
            *pos = i;
            return crt;
        }

        crt = crt->next;
    }

    return NULL;
}

int lg_has_edge(list_graph_t *graph, int src, int dest)
{
    unsigned int pos;

    if (
        !graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return 0;

    return find_node(graph->neighbors[src], dest, &pos) != NULL;
}

linked_list_t *
lg_get_neighbours(list_graph_t *graph, int node)
{
    if (!graph || !graph->neighbors || !is_node_in_graph(node, graph->nodes))
        return NULL;

    return graph->neighbors[node];
}

void lg_remove_edge(list_graph_t *graph, int src, int dest)
{
    unsigned int pos;

    if (
        !graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return;

    if (!find_node(graph->neighbors[src], dest, &pos))
        return;

    ll_remove_nth_node(graph->neighbors[src], pos);
}

void lg_free(list_graph_t *graph)
{
    int i;

    for (i = 0; i != graph->nodes; ++i) {
        free(graph->data[i]);
        ll_free(graph->neighbors + i);
    }

    free(graph->neighbors);
    free(graph->data);
    free(graph);
}

static void __lq_lowest_common_ancestor_search(list_graph_t *graph, int *level, int *nodes, int *count, int current_node, int current_level) {
    linked_list_t *list = lg_get_neighbours(graph, current_node);
    ll_node_t *node = list->head;
    (*count)++;
    level[*count] = current_level;
    nodes[*count] = current_node;

    while (node) {
        __lq_lowest_common_ancestor_search(graph, level, nodes, count, *((int *)node->data), current_level + 1);
        (*count)++;
        level[*count] = current_level;
        nodes[*count] = current_node;

        node = node->next;
    }
}

unsigned int lg_lowest_common_ancestor(list_graph_t *graph, int first, int second) {
    unsigned int length = ceil(graph->nodes * log(graph->nodes));
    int level[length];
    int nodes[length];
    int count = -1;
    __lq_lowest_common_ancestor_search(graph, level, nodes, &count, 0, 0);
    int first_index = 0, second_index = 0;
    int first_found = 0, second_found = 0;
    for (int i = 0; i < count; i++) {
        if (nodes[i] == first || nodes[i] == second) {
            if (first_index == 0) {
                first_index = i;
                if (nodes[i] == first)
                    first_found = 1;
                else
                    second_found = 1;
            } else {
                if ((nodes[i] == second && first_found == 1) || (nodes[i] == first && second_found == 1))
                    second_index = i;
            }
        }

        if (first_index && second_index)
            break;
    }

    int minimum = -1, associated_node = 0;
    for (int i = first_index; i <= second_index; i++) {
        if (minimum == -1 || minimum > level[i]) {
            minimum = level[i];
            associated_node = nodes[i];
        }
    }

    return associated_node;
}


