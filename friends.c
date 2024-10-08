#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "friends.h"
#include "users.h"
#include "hashtable.h"
#include "queue.h"

static int network_user_cnt;
// hashmap that will hold username as key, and user_id as value
static hashtable_t *user_database;
// static graph that will represent the relationships between users
static list_graph_t *friend_network;

void handle_input_friends(char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "add")) {
		// If the network hasn't been created yet
		if (!friend_network)
			friend_network = lg_create();

		// If the database hasn't been created yet
		if (!user_database)
			user_database = ht_create(MAX_PEOPLE, hash_function_string,
									  compare_function_ints,
									  key_val_free_function);
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		char *dest_user = strtok(NULL, "\n");
		add_friend_request(src_user, dest_user);

	} else if (!strcmp(cmd, "remove")) {
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		char *dest_user = strtok(NULL, "\n");
		remove_friendship(src_user, dest_user);
	} else if (!strcmp(cmd, "suggestions")) {
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		print_suggestions(src_user);
	} else if (!strcmp(cmd, "distance")) {
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		char *dest_user = strtok(NULL, "\n");
		print_distance(src_user, dest_user);
	} else if (!strcmp(cmd, "common")) {
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		char *dest_user = strtok(NULL, "\n");
		print_common_friends(src_user, dest_user);
	} else if (!strcmp(cmd, "friends")) {
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		print_nb_friends(src_user);
	} else if (!strcmp(cmd, "popular")) {
		(void)cmd;
		char *src_user = strtok(NULL, "\n ");
		print_popularity_king(src_user);
	}
	free(commands);
}

void update_database(char *src_user, char *dest_user)
{
	bool src_in_db = false, dest_in_db = false;

	// check if source-user is already in hashmap
	if (ht_get(user_database, src_user))
		src_in_db = true;

	// check if destination-user is already in hashmap
	if (dest_user && ht_get(user_database, dest_user))
		dest_in_db = true;

	if (!src_in_db) {
		// add user in hashmap and in graph
		ht_put(user_database, src_user, strlen(src_user) + 1,
			   &network_user_cnt, sizeof(int));
		lg_add_node(friend_network, src_user, strlen(src_user) + 1);
		network_user_cnt++;
	}

	if (dest_user && !dest_in_db) {
		// add user in hashmap and in graph
		ht_put(user_database, dest_user, strlen(dest_user) + 1,
			   &network_user_cnt, sizeof(int));
		lg_add_node(friend_network, dest_user, strlen(dest_user) + 1);
		network_user_cnt++;
	}
}

void add_friend_request(char *src_user, char *dest_user)
{
	update_database(src_user, dest_user);

	int src_id = *(int *)ht_get(user_database, src_user);
	int dest_id = *(int *)ht_get(user_database, dest_user);
	if (!lg_has_edge(friend_network, src_id, dest_id)) {
		// if relationship doesn't exists
		lg_add_edge_unoriented(friend_network, src_id, dest_id);
		printf("Added connection %s - %s\n", src_user, dest_user);
	} else {
		printf("%s and %s are already friends\n", src_user, dest_user);
	}
}

void remove_friendship(char *src_user, char *dest_user)
{
	update_database(src_user, dest_user);

	int src_id = *(int *)ht_get(user_database, src_user);
	int dest_id = *(int *)ht_get(user_database, dest_user);
	if (lg_has_edge(friend_network, src_id, dest_id)) {
		// if relationship exists
		lg_remove_edge_unoriented(friend_network, src_id, dest_id);
		printf("Removed connection %s - %s\n", src_user, dest_user);
	} else {
		printf("Nothing to remove\n");
	}
}

void print_nb_friends(char *username)
{
	update_database(username, NULL);

	int user_id = *(int *)ht_get(user_database, username);
	printf("%s has %d friends\n", username,
		   friend_network->neighbors[user_id]->size);
}

linked_list_t *get_user_friends(char *user) {
	int *user_id = (int *)ht_get(user_database, user);
	if (user_id)
		return lg_get_neighbours(friend_network, *user_id);

	return NULL;
}

char *get_username_by_index(unsigned int index) {
	return friend_network->data[index];
}

unsigned int
is_user_in_friends_list(unsigned int user_id, unsigned int searched_user_id) {
	linked_list_t *friends_list = get_user_friends(get_user_name(user_id));
	if (friends_list) {
		ll_node_t *friends_node = friends_list->head;
		while (friends_node) {
			unsigned int fidx = *((int *)friends_node->data); // friend index
			unsigned int friends_id = get_user_id(get_username_by_index(fidx));
			if (searched_user_id == friends_id)
				return 1;
			friends_node = friends_node->next;
		}
	}

	return 0;
}

unsigned int get_user_network_index(char *user) {
	return *(int *)ht_get(user_database, user);
}

static int compare_user_id_order(const void *a, const void *b) {
	unsigned int a_id = get_user_id(get_username_by_index(*((int *)a)));
	unsigned int b_id = get_user_id(get_username_by_index(*((int *)b)));
	return a_id - b_id;
}

unsigned int *get_friends_clique(char *name, unsigned int *clique_size) {
	unsigned int size = 0;
	unsigned int *clique;
	clique = lg_maximal_clique_containing_node(friend_network,
											   get_user_network_index(name),
											   &size);
	qsort(clique, size, sizeof(unsigned int), compare_user_id_order);
	for (unsigned int i = 0; i < size; i++)
		clique[i] = get_user_id(get_username_by_index(clique[i]));

	if (clique_size)
		*clique_size = size;

	return clique;
}

void print_popularity_king(char *user)
{
	update_database(user, NULL);

	int u_id = *(int *)ht_get(user_database, user);
	unsigned int max_friends = friend_network->neighbors[u_id]->size;
	int min_id = network_user_cnt;
	char max_friend[MAX_USERNAME_LENGTH] = "";
	strcpy(max_friend, user);

	ll_node_t *curr = friend_network->neighbors[u_id]->head;

	for (unsigned int i = 0; i < friend_network->neighbors[u_id]->size; i++) {
		int f_id = *(int *)curr->data;
		char *username = (char *)friend_network->data[f_id];
		int shadow_friend_id = get_user_id(username);
		if (friend_network->neighbors[f_id]->size > max_friends) {
			max_friends = friend_network->neighbors[f_id]->size;
			strcpy(max_friend, username);
			min_id = shadow_friend_id;
		} else {
			if (friend_network->neighbors[f_id]->size == max_friends) {
				if (strcmp(max_friend, user) != 0) {
					if (shadow_friend_id < min_id) {
						max_friends = friend_network->neighbors[f_id]->size;
						strcpy(max_friend, username);
						min_id = shadow_friend_id;
					}
				}
			}
		}
		curr = curr->next;
	}

	if (strcmp(max_friend, user) == 0)
		printf("%s is the most popular\n", user);
	else
		printf("%s is the most popular friend of %s\n", max_friend, user);
}

void print_common_friends(char *src_user, char *dest_user)
{
	update_database(src_user, dest_user);
	char common_friends[MAX_PEOPLE][MAX_USERNAME_LENGTH];
	int cnt = 0;
	int s_id = *(int *)ht_get(user_database, src_user);
	int d_u_id = *(int *)ht_get(user_database, dest_user);
	ll_node_t *curr = friend_network->neighbors[s_id]->head;

	for (unsigned int i = 0; i < friend_network->neighbors[s_id]->size; i++) {
		int friend_id = *(int *)curr->data;
		unsigned int node_position = 0;
		ll_node_t *common_friend = find_node(friend_network->neighbors[d_u_id],
											 friend_id, &node_position);
		if (common_friend) {
			char *friend_name = (char *)friend_network->data[friend_id];
			strcpy(common_friends[cnt++], friend_name);
		}
		curr = curr->next;
	}

	if (!cnt) {
		printf("No common friends for %s and %s\n", src_user, dest_user);
		return;
	}

	// sort by id's in ascending order
	for (int i = 0; i < cnt - 1; i++)
		for (int j = i + 1; j < cnt; j++) {
			int shadow_id_i = get_user_id(common_friends[i]);
			int shadow_id_j = get_user_id(common_friends[j]);

			if (shadow_id_i > shadow_id_j) {
				char tmp[MAX_USERNAME_LENGTH];
				strcpy(tmp, common_friends[i]);
				strcpy(common_friends[i], common_friends[j]);
				strcpy(common_friends[j], tmp);
			}
		}

	printf("The common friends between %s and %s are:\n", src_user, dest_user);

	for (int i = 0; i < cnt; i++)
		printf("%s\n", common_friends[i]);
}

void print_suggestions(char *user)
{
	update_database(user, NULL);

	int u_id = *(int *)ht_get(user_database, user);

	if (friend_network->neighbors[u_id]->size == 0) {
		printf("There are no suggestions for %s\n", user);
		return;
	}

	char suggested_friends[MAX_PEOPLE][MAX_USERNAME_LENGTH];
	int cnt = 0;

	ll_node_t *curr  = friend_network->neighbors[u_id]->head;

	for (unsigned int i = 0 ; i < friend_network->neighbors[u_id]->size; i++) {
		int f_id = *(int *)curr->data;
		ll_node_t *curr_friend = friend_network->neighbors[f_id]->head;
		for (unsigned int j = 0;
			 j < friend_network->neighbors[f_id]->size; j++) {
			int possible_friend_id = *(int *)curr_friend->data;
			unsigned int dummy = 0;
			ll_node_t *exists = find_node(friend_network->neighbors[u_id],
										  possible_friend_id, &dummy);
			if (!exists) {
				char *sugg_f_name;
				sugg_f_name = (char *)friend_network->data[possible_friend_id];
				if (strcmp(sugg_f_name, user)) {
					int found = 0;
					for (int k = 0; k < cnt && !found; k++)
						found = !strcmp(suggested_friends[k], sugg_f_name);
					if (!found)
						strcpy(suggested_friends[cnt++], sugg_f_name);
				}
			}
			curr_friend = curr_friend->next;
		}
		curr = curr->next;
	}

	if (!cnt) {
		printf("There are no suggestions for %s\n", user);
		return;
	}

	printf("Suggestions for %s:\n", user);

	for (int i = 0; i < cnt - 1; i++)
		for (int j = i + 1; j < cnt; j++) {
			int shadow_id_i = get_user_id(suggested_friends[i]);
			int shadow_id_j = get_user_id(suggested_friends[j]);

			if (shadow_id_i > shadow_id_j) {
				char tmp[MAX_USERNAME_LENGTH];
				strcpy(tmp, suggested_friends[i]);
				strcpy(suggested_friends[i], suggested_friends[j]);
				strcpy(suggested_friends[j], tmp);
			}
		}

	for (int i = 0; i < cnt; i++)
		printf("%s\n", suggested_friends[i]);
}

int shortest_path(int src, int dest)
{
	int *visited, *length;
	visited = (int *)calloc(friend_network->nodes, sizeof(int));
	length = (int *)calloc(friend_network->nodes, sizeof(int));

	DIE(!visited || !length, "calloc() betrayed you...\n");

	queue_t *bfs_queue = init_queue(sizeof(int));

	visited[src] = 1;
	length[src] = 0;
	push_queue(bfs_queue, &src);

	while (!is_empty_queue(bfs_queue)) {
		int curr_node = *(int *)peek_queue(bfs_queue);
		ll_node_t *junk = pop_queue(bfs_queue);
		free(junk->data);
		free(junk);
		ll_node_t *curr = friend_network->neighbors[curr_node]->head;
		unsigned int list_size = friend_network->neighbors[curr_node]->size;
		for (unsigned int i = 1; i <= list_size; i++) {
			int neighbor = *(int *)curr->data;
			if (!visited[neighbor]) {
				visited[neighbor] = 1;
				length[neighbor] = length[curr_node] + 1;
				push_queue(bfs_queue, &neighbor);
			}
			curr = curr->next;
		}
	}

	int dist = length[dest];
	free(visited);
	free(length);
	destroy_queue(&bfs_queue);
	return dist;
}

void print_distance(char *src, char *dest)
{
	update_database(src, dest);

	int src_id = *(int *)ht_get(user_database, src);
	int dest_id = *(int *)ht_get(user_database, dest);

	if (lg_has_edge(friend_network, src_id, dest_id)) {
		printf("The distance between %s - %s is 1\n", src, dest);
		return;
	}

	int distance = shortest_path(src_id, dest_id);

	if (!distance) {
		printf("There is no way to get from %s to %s\n", src, dest);
		return;
	}

	printf("The distance between %s - %s is %d\n", src, dest, distance);
}

void free_friends(void)
{
	ht_free(user_database);
	lg_free(friend_network);
	network_user_cnt = 0;
}
