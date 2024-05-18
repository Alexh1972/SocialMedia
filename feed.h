#ifndef FEED_H
#define FEED_H

#include "friends.h"
#include "posts.h"
#include "list_graph.h"
#include "queue.h"

/**
 * @brief Function that will parse and execute the given query
 * @param input The query that will be parsed and executed
 */
void handle_input_feed(char *input);

/**
 * @brief Function that will print the feed of a user
 * @param name Username of the user
 * @param feed_size The size of the feed
 */
void print_user_feed(char *name, unsigned int feed_size);

/**
 * @brief Function that will print the reposts of the user's friends
 * @param post The original post
 * @param user_id The id of the user
 * @param no_posts The number of posts
 * @return Array of pointers to posts that have all of the user's friends
 * reposts
 */
post_t
**get_friends_reposts(post_t *post, unsigned int user_id,
									unsigned int *no_posts);

/**
 * @brief Function that will print the names of the user's friends that
 * reposted the original post
 * @param name The username of the user
 * @param post_id The id of the original post
 */
void print_friends_name_reposts(char *name, unsigned int post_id);

/**
 * @brief Function that will print the friend clique
 * @param name The username of the starting user
 */
void print_friend_clique(char *name);

#endif
