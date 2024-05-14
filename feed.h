#ifndef FEED_H
#define FEED_H

#include "friends.h"
#include "posts.h"
#include "list_graph.h"
#include "queue.h"

/**
 * Function that handles the calling of every command from task 3
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_feed(char *input);

void print_user_feed(char *name, unsigned int feed_size);

post_t **get_friends_reposts(post_t *post, unsigned int user_id, unsigned int *no_posts);

void print_friends_name_reposts(char *name, unsigned int post_id);

void print_friend_clique(char *name);

#endif // FEED_H
