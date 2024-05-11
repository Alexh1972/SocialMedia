#ifndef POSTS_H
#define POSTS_H

#include "list_graph.h"

typedef struct post_t{
     unsigned int id;
     char *title;
	 unsigned int user_id;
	 list_graph_t *events;
	 doubly_linked_list_t *likes;
} post_t;

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input);

unsigned int add_post(char *username, char *title);

post_t *create_post(unsigned int id, char *title, unsigned int user_id, unsigned int is_repost);

void print_posts();

void print_post(post_t *post);

void free_posts();

void lg_print_post(list_graph_t *graph);

unsigned int find_common_repost(unsigned int post, unsigned int first_repost, unsigned int second_repost);

unsigned int add_repost(char *name, unsigned int post_id, unsigned int repost_id);

post_t *get_post_from_node(ll_node_t *node);

unsigned int get_repost_index(post_t *post, unsigned int repost_id);

unsigned int update_post_likes(char *name, unsigned int post_id, unsigned int repost_id);

post_t *get_post_by_id(unsigned int post_id);

char *get_post_title_by_id(unsigned int post_id);

unsigned int get_post_likes(unsigned int post_id, unsigned int repost_id);

dll_node_t *get_user_like_node(post_t *post, unsigned int user_id);

unsigned int get_maximum_likes_repost_id(unsigned int post_id);

unsigned int get_number_reposts(post_t *post);

post_t *get_repost_by_id(unsigned int repost_id, unsigned int *index);

void print_reposts(unsigned int post_id);

#endif // POSTS_H
