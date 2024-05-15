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

void lg_add_edge_unoriented(list_graph_t *graph, int src, int dest)
{
    if (!graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return;

    ll_add_nth_node(graph->neighbors[src], graph->neighbors[src]->size, &dest);
    ll_add_nth_node(graph->neighbors[dest], graph->neighbors[dest]->size, &src);
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

    ll_node_t *rm_node = ll_remove_nth_node(graph->neighbors[src], pos);
    free(rm_node->data);
    free(rm_node);
}

void lg_remove_edge_unoriented(list_graph_t *graph, int src, int dest)
{
    unsigned int pos_src;
    unsigned int pos_dest;

    if (
        !graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return;

    if (!find_node(graph->neighbors[src], dest, &pos_src))
        return;
    
    if (!find_node(graph->neighbors[dest], src, &pos_dest))
        return;

    ll_node_t *rm_node_src = ll_remove_nth_node(graph->neighbors[src], pos_src);
    ll_node_t *rm_node_dest = ll_remove_nth_node(graph->neighbors[dest], pos_dest);
    free(rm_node_src->data);
    free(rm_node_src);
    free(rm_node_dest->data);
    free(rm_node_dest);
}

void lg_free(list_graph_t *graph)
{
    if (graph == NULL)
        return;
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
    int first_index = -1, second_index = -1;
    int first_found = 0, second_found = 0;
    for (int i = 0; i <= count; i++) {
        if (nodes[i] == first || nodes[i] == second) {
            if (first_index == -1) {
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

        if (first_index != -1 && second_index != -1)
            break;
    }

    int minimum = -1, associated_node = -1;
    for (int i = first_index; i <= second_index; i++) {
        if (minimum == -1 || minimum > level[i]) {
            minimum = level[i];
            associated_node = nodes[i];
        }
    }

    return associated_node;
}

static void free_info(void *data) {
    info *info = data;
    free(info->key);
    free(info->value);
}

static void __lg_maximal_clique(list_graph_t *graph, unsigned int **maximal_clique, unsigned int *clique_size, unsigned int *build_clique, unsigned int *build_clique_size, hashtable_t *ht) {
    if (*build_clique_size > *clique_size) {
        for (unsigned int i = 0; i < *build_clique_size; i++)
            (*maximal_clique)[i] = build_clique[i];
        *clique_size = *build_clique_size;
    }

    int i = build_clique[*build_clique_size - 1] + 1;
    if (*build_clique_size == 1)
        i = 0;
    
    for (; i < graph->nodes; i++) {
        if (!ht_get(ht, &i)) {
            unsigned int node_can_be_added = 1;

            for (unsigned int k = 0; k < *build_clique_size; k++) {
                int j = build_clique[k];
                if (!lg_has_edge(graph, i, j)) {
                    node_can_be_added = 0;
                }
            }

            if (node_can_be_added) {
                ht_put(ht, &i, sizeof(int), &i, sizeof(int));
                build_clique[(*build_clique_size)++] = i;
                __lg_maximal_clique(graph, maximal_clique, clique_size, build_clique, build_clique_size, ht);
                (*build_clique_size)--;
                ht_remove_entry(ht, &i);
            }
        }
    }
}

unsigned int *lg_maximal_clique_containing_node(list_graph_t *graph, unsigned int node, unsigned int *size) {
    hashtable_t *nodes_table = ht_create(graph->nodes, hash_function_int, compare_function_ints, free_info);

    ht_put(nodes_table, &node, sizeof(int), &node, sizeof(int));

    unsigned int *clique_nodes = malloc(sizeof(int) * graph->nodes);
    unsigned int clique_size = 1;
    clique_nodes[clique_size - 1] = node;
    unsigned int build_clique[graph->nodes];
    unsigned int build_clique_size = 1;
    build_clique[build_clique_size - 1] = node;
    __lg_maximal_clique(graph, &clique_nodes, &clique_size, build_clique, &build_clique_size, nodes_table);

    if (size)
        *size = clique_size;

    ht_free(nodes_table);
    return clique_nodes;
}


