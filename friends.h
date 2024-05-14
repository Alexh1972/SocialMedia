#ifndef FRIENDS_H
#define FRIENDS_H

#define MAX_COMMAND_LEN 100
#define MAX_PEOPLE 550

#include "list_graph.h"


/**
 * Function that handles the calling of every command from task 1
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_friends(char *input);

void update_database(char *src_user, char *dest_user);

void add_friend_requst(char *src_user, char *dest_user);

void remove_friendship(char *src_user, char *dest_user);

void print_nb_friends(char *user);

void print_popularity_king(char *user);

void print_common_friends(char *src_user, char *dest_user);

void print_suggestions(char *user);

int shortest_path(int src, int dest);

void print_distance(char *src, char *dest);

void free_friends();

linked_list_t *get_user_friends(char *user);

unsigned int is_user_in_friends_list(unsigned int user_id, unsigned int searched_user);

char *get_username_by_index(unsigned int index);

void print_user_posts(char *name);

void print_friends_name_reposts(char *name, unsigned int post_id);

unsigned int get_user_network_index(char *user);

unsigned int *get_friends_clique(char *name, unsigned int *size);

#endif // FRIENDS_H
