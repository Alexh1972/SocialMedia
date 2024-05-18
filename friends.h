#ifndef FRIENDS_H
#define FRIENDS_H

#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

#include "list_graph.h"

/**
 * @brief Function that will parse and execute the given query
 * @param input The query that will be parsed and executed
 */
void handle_input_friends(char *input);

/**
 * @brief Function that will add new entries in the user database
 * @param src_user First user to be added, must not be NULL
 * @param dest_user Second user to be added, could also be NULL
 */
void update_database(char *src_user, char *dest_user);

/**
 * @brief This function will create a new connection between two users
 * @param src_user The first user involved in the connection
 * @param dest_user The second user involved in the connection
 */
void add_friend_request(char *src_user, char *dest_user);

/**
 * @brief This function will remove an existing connection between two users
 * @param src_user The first user involved in the connection
 * @param dest_user The second user involved in the connection
 */
void remove_friendship(char *src_user, char *dest_user);

/**
 * @brief This function will print the number of friends of a user
 * @param user The user involved
 */
void print_nb_friends(char *user);

/**
 * @brief This function will print the username of the most popular
 * friend of user
 * @param user The user involved
 */
void print_popularity_king(char *user);

/**
 * @brief This function will print the common friends of two users
 * @param src_user The first user involved
 * @param dest_user The second user involved
 */
void print_common_friends(char *src_user, char *dest_user);

/**
 * @brief This function will print suggestions of friends for a user
 * @param user The user involved
 */
void print_suggestions(char *user);

/**
 * @brief This function aplies a Breadth First Search to find the minimum
 * distance between two users in the friend_network graph
 * @param src The index from the user database of the starting user
 * @param dest The index from the user database of the destination user
 * @return The minimum distance or 0, if there isn't any path between the two
 * users.
 */
int shortest_path(int src, int dest);

/**
 * @brief This function will print the shortest distance between two users
 * in the friend_network graph
 * @param src The source user
 * @param dest The destination user
 */
void print_distance(char *src, char *dest);

/**
 * @brief This function resets the number of users in the graph and frees
 * the memory allocated for the friends_network graph and the user database
 */
void free_friends(void);

/**
 * @brief This function gets all the friends of a user
 * @param user The user involved
 * @return All the friends of a user as a linked list
 */
linked_list_t *get_user_friends(char *user);

/**
 * @brief This function checks if a user is in another user's friend list
 * @param user_id The id of the user whose friend list will be searched
 * @param searched_user The id of the user that will be searched in the
 * friend list
 * @return 1 if the user was found, 0 if it wasn't found
 */
unsigned int
is_user_in_friends_list(unsigned int user_id, unsigned int searched_user);

/**
 * @brief Function that will get the username from the graph, based on the
 * index
 * @param index The index of the user
 * @return The username of the user
 */
char *get_username_by_index(unsigned int index);

/**
 * @brief This function will print everything posted by a user
 * @param name The username
 */
void print_user_posts(char *name);

/**
 * @brief This function will print the username of the friends that reposted
 * a user's post
 * @param name The owner of the original post
 * @param post_id The id of the original post
 */
void print_friends_name_reposts(char *name, unsigned int post_id);

/**
 * @brief This function will query the user database to get the index of a user
 * in the friends_network graph
 * @param user The searched user
 * @return The index of the searched user
 */
unsigned int get_user_network_index(char *user);

/**
 * @brief This function builds a maximal complete subgraph
 * @param name The starting node
 * @param size The size of the subgraph
 * @return An array of id's of the users in the subgraph
 */
unsigned int *get_friends_clique(char *name, unsigned int *size);

#endif
