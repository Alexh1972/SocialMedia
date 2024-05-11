#ifndef POSTS_H
#define POSTS_H

#include "list_graph.h"

typedef struct post_t{
     unsigned int id;
     char *title;
	 unsigned int user_id;
	 list_graph_t *events;
} post_t;

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input);

void add_post(char *username, char *title);

post_t *create_post(unsigned int id, char *title, unsigned int user_id, unsigned int is_repost);

void print_posts();

void print_post(post_t *post);

void free_posts();

void lg_print_post(list_graph_t *graph);

void add_repost(char *name, unsigned int post_id, unsigned int repost_id);

post_t *get_post_from_node(ll_node_t *node);

unsigned int find_repost_index(post_t *post, unsigned int repost_id);

#endif // POSTS_H
