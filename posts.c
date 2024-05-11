#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "users.h"
#include "posts.h"
#include "queue.h"

static linked_list_t *posts = NULL;
static unsigned int current_post_id = 1;

void handle_input_posts(char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;
	if (!strcmp(cmd, "create")) {
		char *name = strtok(NULL, "\n ");
		char *title = strtok(NULL, "\n");
		if (title[0] == '"') {
			title++;
			title = strtok(title, "\"");
		}
		add_post(name, title);
	}
	else if (!strcmp(cmd, "repost")) {
		char *name = strtok(NULL, "\n ");
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		unsigned int repost_id = 0;
		char *auxiliar = strtok(NULL, "\n ");
		if (auxiliar)
			repost_id = atoi(auxiliar);
		add_repost(name, post_id, repost_id);
	}
	else if (!strcmp(cmd, "common-repost")) {
		unsigned int post = atoi(strtok(NULL, "\n "));
		unsigned int first_repost = atoi(strtok(NULL, "\n "));
		unsigned int second_repost = atoi(strtok(NULL, "\n "));
		printf("%u\n", find_common_repost(post, first_repost, second_repost));
	}
	else if (!strcmp(cmd, "like"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "ratio"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "delete"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-reposts"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
	print_posts();
	free(commands);
}

void add_post(char *username, char *title) {
	if (posts == NULL) {
		posts = ll_create(sizeof(post_t));
	}

	post_t *post = create_post(current_post_id, title, get_user_id(username), 0);
	lg_add_node(post->events, &current_post_id, sizeof(int));
	current_post_id++;
	ll_add_nth_node(posts, 0, post);
	free(post);
}

post_t *create_post(unsigned int id, char *title, unsigned int user_id, unsigned int is_repost) {
	post_t *post = malloc(sizeof(post_t));
	if (!is_repost) {
		post->events = lg_create();
		post->title = strdup(title);
	}
	else {
		post->events = NULL;
		post->title = NULL;
	}
	// lg_add_node(post->events, &id, sizeof(int));
	post->user_id = user_id;
		
	post->id = id;
	return post;
}

unsigned int find_common_repost(unsigned int post_id, unsigned int first_repost, unsigned int second_repost) {
	ll_node_t *post_node = posts->head;
	post_t *root_post = NULL;
	while (post_node) {
		post_t *post = get_post_from_node(post_node);
		if (post->id == post_id) {
			root_post = post;
			break;
		}
		post_node = post_node->next;
	}

	unsigned int ancestor = lg_lowest_common_ancestor(root_post->events, first_repost, second_repost);
	if (ancestor == 0) {
		printf("No common reposts for %u and %u\n", first_repost, second_repost);
	} else {
		printf("The first common repost of %u and %u is %u\n", first_repost, second_repost,(*((post_t **)root_post->events->data[ancestor]))->id);
	}

	return ancestor;
}

void add_repost(char *name, unsigned int post_id, unsigned int repost_id) {
	post_t *repost = create_post(current_post_id, NULL, get_user_id(name), 1);
	ll_node_t *post_node = posts->head;
	post_t *reposted_post = NULL;
	while (post_node) {
		post_t *post = get_post_from_node(post_node);
		if (post->id == post_id) {
			reposted_post = post;
			break;
		}
		post_node = post_node->next;
	}

	lg_add_node(reposted_post->events, &repost, sizeof(post_t *));	
	
	if (repost_id == 0) {
		lg_add_edge(reposted_post->events, 0, reposted_post->events->nodes - 1);
	} else {
		unsigned int repost_index = find_repost_index(reposted_post, repost_id);
		lg_add_edge(reposted_post->events, repost_index, reposted_post->events->nodes - 1);
	}

	current_post_id++;
}

post_t *get_post_from_node(ll_node_t *node) {
	return node->data;
}

unsigned int find_repost_index(post_t *post, unsigned int repost_id) {
	queue_t *queue = init_queue(sizeof(unsigned int));
	list_graph_t *graph = post->events;
	unsigned int repost_index = -1;
	unsigned int current_node = 0;
	push_queue(queue, &current_node);
	while (!is_empty_queue(queue)) {
		current_node = *((unsigned int *)peek_queue(queue));
		linked_list_t *list = lg_get_neighbours(graph, current_node);
		ll_node_t *node = list->head;
		while (node) {
			unsigned int next = *((unsigned int *)node->data);
			push_queue(queue, &node);
			ll_node_t *rm_node = pop_queue(queue);
			free(rm_node->data);
			free(rm_node);
			
			if ((*((post_t **)graph->data[next]))->id == repost_id) {
				repost_index = next;
				goto repost_index_found;
			}

			node = node->next;
		}
	}
repost_index_found:
	destroy_queue(&queue);
	return repost_index;
}

void print_posts() {
	if (posts) {
		printf("\n--------POSTS--------\n");
		ll_node_t *node = posts->head;
		while (node) {
			print_post(node->data);
			node = node->next;
		}
		printf("\n--------END POSTS--------\n");
	}
}

void print_post(post_t *post) {
	printf("\n--------POST--------\n");
	printf("--------ID - %u--------\n", post->id);
	printf("--------TITLE - %s--------\n", post->title);
	printf("--------USER ID - %u--------\n", post->user_id);
	lg_print_post(post->events);
}

void free_posts() {
	ll_node_t *node = posts->head;
	while (node) {
		ll_node_t *next = node->next;
		node = ll_remove_nth_node(posts, 0);
		post_t *post = node->data;
		free(post->title);
		for (int i = 1; i < post->events->nodes; i++) {
			post_t *graph_post = (*((post_t **)post->events->data[i]));
			free(graph_post);
		}

		lg_free(post->events);
		free(node->data);
		free(node);
		node = next;
	}
	ll_free(&posts);
}

void lg_print_post(list_graph_t *graph) {
	printf("\n--------GRAPH - index - 0--------\n");
	for (int i = 0; i < graph->nodes; i++) {
		if (i == 0)
			printf("No. node: %d - info. node: %d : \n", i, *((int *)graph->data[i]));
		else
			printf("No. node: %d - info. node: %d : \n", i, (*((post_t **)graph->data[i]))->id);
		ll_node_t *node = graph->neighbors[i]->head;
		while (node) {
			printf("%d, ", *((int *)node->data));
			node = node->next;
		}
		printf("\n");
	}
}

