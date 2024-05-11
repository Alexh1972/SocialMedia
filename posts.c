#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "users.h"
#include "posts.h"
#include "queue.h"

static linked_list_t *posts = NULL;
static unsigned int current_post_id = 0;

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
		printf("Created \"%s\" for %s\n", title, name);
	}
	else if (!strcmp(cmd, "repost")) {
		char *name = strtok(NULL, "\n ");
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		unsigned int repost_id = 0;
		char *auxiliar = strtok(NULL, "\n ");
		if (auxiliar)
			repost_id = atoi(auxiliar);
		unsigned int new_repost_id = add_repost(name, post_id, repost_id);
		printf("Created Repost #%d for %s\n", new_repost_id, name);
	}
	else if (!strcmp(cmd, "common-repost")) {
		unsigned int post = atoi(strtok(NULL, "\n "));
		unsigned int first_repost = atoi(strtok(NULL, "\n "));
		unsigned int second_repost = atoi(strtok(NULL, "\n "));
		unsigned int ancestor = find_common_repost(post, first_repost, second_repost);
		if (ancestor == 0) {
			printf("No common reposts for %u and %u\n", first_repost, second_repost);
		} else {
			printf("The first common repost of %u and %u is %u\n", first_repost, second_repost, ancestor);
		}
	}
	else if (!strcmp(cmd, "like")) {
		char *name = strtok(NULL, "\n ");
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		unsigned int repost_id = 0;
		char *auxiliar = strtok(NULL, "\n ");
		if (auxiliar)
			repost_id = atoi(auxiliar);
		unsigned int liked = update_post_likes(name, post_id, repost_id);
		if (liked)
			printf("%s liked ???\n", name);
		else
			printf("%s unliked ???\n", name);
	}
	else if (!strcmp(cmd, "ratio")) {
		unsigned int post = atoi(strtok(NULL, "\n "));
		unsigned int maximum_likes_post_id = get_maximum_likes_repost_id(post);

		if (maximum_likes_post_id != post) {
			printf("Post %u got ratio'd by repost %u\n", post, maximum_likes_post_id);
		} else {
			printf("The original post is the highest rated\n");
		}
	}
	else if (!strcmp(cmd, "delete"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes")) {
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		unsigned int repost_id = 0;
		char *auxiliar = strtok(NULL, "\n ");
		if (auxiliar)
			repost_id = atoi(auxiliar);
		unsigned int likes = get_post_likes(post_id, repost_id);
		if (repost_id)
			printf("Repost #%u has %u likes\n", repost_id, likes);
		else
			printf("Repost \"%s\" has %u likes\n", get_post_title_by_id(post_id), likes);
	}
	else if (!strcmp(cmd, "get-reposts")) {
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		print_reposts(post_id);
	}
	print_posts();
	free(commands);
}

char *get_post_title_by_id(unsigned int post_id) {
	post_t *post = get_post_by_id(post_id);
	return post->title;
}

unsigned int add_post(char *username, char *title) {
	if (posts == NULL) {
		posts = ll_create(sizeof(post_t));
	}

	post_t *post = create_post(current_post_id, title, get_user_id(username), 0);
	lg_add_node(post->events, &current_post_id, sizeof(int));
	current_post_id++;
	ll_add_nth_node(posts, 0, post);
	unsigned int new_post_id = post->id;
	free(post);
	return new_post_id;
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
	post->likes = dll_create(sizeof(unsigned int));
		
	post->id = id;
	return post;
}

unsigned int find_common_repost(unsigned int post_id, unsigned int first_repost, unsigned int second_repost) {
	post_t *root_post = get_post_by_id(post_id);

	unsigned int ancestor = lg_lowest_common_ancestor(root_post->events, get_repost_index(root_post, first_repost), get_repost_index(root_post, second_repost));
	if (ancestor != 0)
		return (*((post_t **)root_post->events->data[ancestor]))->id;
	else
		return 0;
}

post_t *get_post_by_id(unsigned int post_id) {
	ll_node_t *post_node = posts->head;
	while (post_node) {
		post_t *post = get_post_from_node(post_node);
		if (post->id == post_id)
			return post;

		post_node = post_node->next;
	}

	return NULL;
}

unsigned int get_number_reposts(post_t *post) {
	return post->events->nodes;
}

unsigned int add_repost(char *name, unsigned int post_id, unsigned int repost_id) {
	post_t *repost = create_post(current_post_id, NULL, get_user_id(name), 1);
	post_t *reposted_post = get_post_by_id(post_id);

	lg_add_node(reposted_post->events, &repost, sizeof(post_t *));	
	
	if (repost_id == 0) {
		lg_add_edge(reposted_post->events, 0, get_number_reposts(reposted_post) - 1);
	} else {
		unsigned int repost_index = get_repost_index(reposted_post, repost_id);
		lg_add_edge(reposted_post->events, repost_index, get_number_reposts(reposted_post) - 1);
	}

	current_post_id++;
	return repost->id;
}

post_t *get_post_from_node(ll_node_t *node) {
	return node->data;
}

unsigned int get_repost_index(post_t *post, unsigned int repost_id) {
	queue_t *queue = init_queue(sizeof(unsigned int));
	list_graph_t *graph = post->events;
	unsigned int repost_index = -1;
	unsigned int current_node = 0;
	push_queue(queue, &current_node);
	while (!is_empty_queue(queue)) {
		current_node = *((unsigned int *)peek_queue(queue));
		ll_node_t *rm_node = pop_queue(queue);
		free(rm_node->data);
		free(rm_node);
		linked_list_t *list = lg_get_neighbours(graph, current_node);
		ll_node_t *node = list->head;
		while (node) {
			
			unsigned int next = *((unsigned int *)node->data);
			push_queue(queue, &next);
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

post_t *get_repost_by_index(post_t *post, unsigned int index) {
	if (index == 0)
		return post;
	return *((post_t **)post->events->data[index]);
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
	for (unsigned int i = 0; i < get_number_reposts(post); i++) {
		post_t *liked_post = NULL;
		if (i == 0)
			liked_post = post;
		else
			liked_post = get_repost_by_index(post, i);
		dll_node_t *like_node = liked_post->likes->head;
		printf("--------LIKES - %u - ID - %u--------\n", dll_get_size(liked_post->likes), liked_post->id);
		while (like_node) {
			printf("%s\n", get_user_name(*((int *)like_node->data)));
			like_node = dll_get_next_node(liked_post->likes, like_node);
		}
	}
}

void free_posts() {
	ll_node_t *node = posts->head;
	while (node) {
		ll_node_t *next = node->next;
		node = ll_remove_nth_node(posts, 0);
		post_t *post = node->data;
		free(post->title);
		dll_free(&post->likes);
		for (unsigned int i = 1; i < get_number_reposts(post); i++) {
			post_t *graph_post = (*((post_t **)post->events->data[i]));
			dll_free(&graph_post->likes);
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

unsigned int update_post_likes(char *name, unsigned int post_id, unsigned int repost_id) {
	unsigned int user_id = get_user_id(name);
	post_t *post = get_post_by_id(post_id);
	if (repost_id == 0) {
		dll_node_t *liked_node = get_user_like_node(post, user_id);
		if (liked_node == NULL) {
			dll_add_tail(post->likes, &user_id);
			return 1;
		} else {
			dll_node_t *rm_node = dll_remove_node(post->likes, liked_node);
			free(rm_node);
			return 0;
		}
	} else {
		post_t *repost = get_repost_by_index(post, get_repost_index(post, repost_id));
		dll_node_t *liked_node = get_user_like_node(repost, user_id);
		if (liked_node == NULL) {
			dll_add_tail(repost->likes, &user_id);
			return 1;
		} else {
			dll_node_t *rm_node = dll_remove_node(repost->likes, liked_node);
			free(rm_node);
			return 0;
		}
	}
}

dll_node_t *get_user_like_node(post_t *post, unsigned int user_id) {
	dll_node_t *node = post->likes->head;
	while (node) {
		unsigned int node_user_id = *((int *)node->data);
		if (node_user_id == user_id)
			return node;
		node = dll_get_next_node(post->likes, node);
	}

	return NULL;
}

unsigned int get_post_likes(unsigned int post_id, unsigned int repost_id) {
	post_t *post = get_post_by_id(post_id);
	if (repost_id == 0) {
		return dll_get_size(post->likes);
	} else {
		post_t *repost = get_repost_by_index(post, get_repost_index(post, repost_id));
		return dll_get_size(repost->likes);
	}
}

unsigned int get_maximum_likes_repost_id(unsigned int post_id) {
	post_t *post = get_post_by_id(post_id);
	unsigned int maximum_likes = dll_get_size(post->likes), maximum_likes_id = post_id;

	for (unsigned int i = 1; i < get_number_reposts(post); i++) {
		post_t *repost = (*((post_t **)post->events->data[i]));
		unsigned int likes = dll_get_size(repost->likes);
		if (likes > maximum_likes) {
			maximum_likes = likes;
			maximum_likes_id = repost->id;
		}
	}

	return maximum_likes_id;
}

post_t *get_repost_by_id(unsigned int repost_id, unsigned int *index) {
	ll_node_t *post_node = posts->head;
	while (post_node) {
		post_t *post = get_post_from_node(post_node);
		if (post->id == repost_id) {
			if (index != NULL)
				*index = 0;
			return post;
		}
		
		for (unsigned int i = 1; i < get_number_reposts(post); i++) {
			post_t *repost = get_repost_by_index(post, i);
			if (repost->id == repost_id) {
				if (index != NULL)
					*index = i;
				return post;
			}
		}
		post_node = post_node->next;
	}

	return NULL;
}

static void __print_reposts(post_t *post, unsigned int current_node) {
	list_graph_t *repost_graph = post->events;
	linked_list_t *list = lg_get_neighbours(repost_graph, current_node);
	ll_node_t *node = list->head;
	post_t *repost = get_repost_by_index(post, current_node);
	if (current_node != 0)
		printf("Repost #%u by %s\n", repost->id, get_user_name(repost->user_id));
	else
		printf("\"%s\" - Post #%u by %s\n", repost->title, repost->id, get_user_name(repost->user_id));

	while (node) {
		unsigned int next = *((int *)node->data);
		__print_reposts(post, next);
		node = node->next;
	}
}

void print_reposts(unsigned int post_id) {
	unsigned int index = 0;
	post_t *post = get_repost_by_id(post_id, &index);

	__print_reposts(post, index);
}

