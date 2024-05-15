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
		if (new_repost_id)
			printf("Created repost #%d for %s\n", new_repost_id, name);
	}
	else if (!strcmp(cmd, "common-repost")) {
		unsigned int post = atoi(strtok(NULL, "\n "));
		unsigned int first_repost = atoi(strtok(NULL, "\n "));
		unsigned int second_repost = atoi(strtok(NULL, "\n "));
		int ancestor = find_common_repost(post, first_repost, second_repost);
		if (ancestor == -1) {
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
		if (liked) {
			if (repost_id)
				printf("User %s liked repost \"%s\"\n", name, get_post_title_by_id(post_id));
			else
				printf("User %s liked post \"%s\"\n", name, get_post_title_by_id(post_id));
		}
		else {
			if (repost_id)
				printf("User %s unliked repost \"%s\"\n", name, get_post_title_by_id(post_id));
			else
				printf("User %s unliked post \"%s\"\n", name, get_post_title_by_id(post_id));
		}
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
	else if (!strcmp(cmd, "delete")) {
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		unsigned int repost_id = 0;
		char *auxiliar = strtok(NULL, "\n ");
		if (auxiliar)
			repost_id = atoi(auxiliar);
		delete_repost(post_id, repost_id);
		printf("Deleted repost #%d of post \"%s\"\n", repost_id, get_post_title_by_id(post_id));
	}
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
			printf("Post \"%s\" has %u likes\n", get_post_title_by_id(post_id), likes);
	}
	else if (!strcmp(cmd, "get-reposts")) {
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		unsigned int repost_id = 0;
		char *auxiliar = strtok(NULL, "\n ");
		if (auxiliar)
			repost_id = atoi(auxiliar);
		print_reposts(post_id, repost_id);
	}
	// print_posts();
	free(commands);
}

linked_list_t *get_all_posts() {
	return posts;
}

char *get_post_title_by_id(unsigned int post_id) {
	post_t *post = get_post_by_id(post_id);
	return post->title;
}

profile_post_t *create_profile_post(post_t *post, char *original_post_title) {
	profile_post_t *profile_post = malloc(sizeof(profile_post_t));
	profile_post->post = post;
	profile_post->original_post_title = original_post_title;
	return profile_post;
}

void free_profile_post(profile_post_t *profile_post) {
	free(profile_post);
}

static int compare_user_profile_posts_by_creation_order(const void *a, const void *b)
{
   const profile_post_t *first = *((profile_post_t **)a);
   const profile_post_t *second = *((profile_post_t **)b);

   if (first->original_post_title == NULL && second->original_post_title != NULL) {
		return 1;
   } else if (first->original_post_title != NULL && second->original_post_title == NULL) {
		return -1;
   }

	return first->post->id - second->post->id;

}

profile_post_t **get_user_profile_posts(unsigned int user_id, unsigned int *no_posts) {
	ll_node_t *post_node = posts->head;
	profile_post_t **user_profile_posts = NULL;
	unsigned int number_posts = 0;
	while (post_node) {
		post_t *post = get_post_from_node(post_node);
		if (post->user_id == user_id) {
			if (!number_posts) {
				number_posts++;
				user_profile_posts = malloc(number_posts * sizeof(profile_post_t *));
				user_profile_posts[number_posts - 1] = create_profile_post(post, NULL);
			} else {
				number_posts++;
				user_profile_posts = realloc(user_profile_posts, number_posts * sizeof(profile_post_t *));
				user_profile_posts[number_posts - 1] = create_profile_post(post, NULL);
			}
		}

		unsigned int node = 0;
		queue_t *queue = init_queue(sizeof(int));
		push_queue(queue, &node);
		while (!is_empty_queue(queue)) {
			int node = *((int *)peek_queue(queue));
			ll_node_t *rm_node = pop_queue(queue);
			free(rm_node->data);
			free(rm_node);
			linked_list_t *list = lg_get_neighbours(post->events, node);
			ll_node_t *repost_node = list->head;
			while (repost_node) {
				int next = *((int *)repost_node->data);
				post_t *repost = get_repost_by_index(post, next);
				if (repost->user_id == user_id) {
					if (!number_posts) {
						number_posts++;
						user_profile_posts = malloc(number_posts * sizeof(profile_post_t *));
						user_profile_posts[number_posts - 1] = create_profile_post(post, post->title);
					} else {
						number_posts++;
						user_profile_posts = realloc(user_profile_posts, number_posts * sizeof(profile_post_t *));
						user_profile_posts[number_posts - 1] = create_profile_post(post, post->title);
					}
				}
				push_queue(queue, &next);
				repost_node = repost_node->next;
			}
		}

		destroy_queue(&queue);
		post_node = post_node->next;
	}

	if (user_profile_posts)
		qsort(user_profile_posts, number_posts, sizeof(profile_post_t *), compare_user_profile_posts_by_creation_order);
	if (no_posts)
		*no_posts = number_posts;

	return user_profile_posts;
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

int find_common_repost(unsigned int post_id, unsigned int first_repost, unsigned int second_repost) {
	post_t *root_post = get_post_by_id(post_id);

	unsigned int ancestor = lg_lowest_common_ancestor(root_post->events, get_repost_index(root_post, first_repost), get_repost_index(root_post, second_repost));
	if (ancestor != (unsigned int)-1 && ancestor != 0)
		return (*((post_t **)root_post->events->data[ancestor]))->id;
	else if (ancestor == 0)
		return root_post->id;
	else
		return -1;
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

	if (reposted_post) {
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

	return 0;
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
	if (posts) {
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
}

void remove_post(unsigned int post_id) {
	unsigned int i = 0;
	ll_node_t *node = posts->head;
	while (node) {
		ll_node_t *next = node->next;
		if (((post_t *)node->data)->id == post_id) {
			node = ll_remove_nth_node(posts, i);
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
			break;
		}
		node = next;
		i++;
	}
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
			free(rm_node->data);
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
			free(rm_node->data);
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
		printf("\"%s\" - Post by %s\n", repost->title, get_user_name(repost->user_id));

	while (node) {
		unsigned int next = *((int *)node->data);
		__print_reposts(post, next);
		node = node->next;
	}
}

void print_reposts(unsigned int post_id, unsigned int repost_id) {
	post_t *post = get_post_by_id(post_id);
	unsigned int index = 0;
	if (repost_id)
		index = get_repost_index(post, repost_id); 

	if (post) {
		__print_reposts(post, index);
	}
	else {
		post = get_repost_by_id(post_id, &index);
		__print_reposts(post, index);
	}
}

void delete_repost(unsigned int post_id, unsigned int repost_id) {
	if (repost_id == 0) {
		remove_post(post_id);
	} else {
		unsigned int current_node = 0;
		post_t *post = get_post_by_id(post_id);
		queue_t *queue = init_queue(sizeof(int));
		unsigned int repost_node = get_repost_index(post, repost_id);
		push_queue(queue, &current_node);
		unsigned int repost_parent_found = 0;
		while (!is_empty_queue(queue) && !repost_parent_found) {
			current_node = *((int *)peek_queue(queue));
			ll_node_t *rm_node = pop_queue(queue);
			free(rm_node->data);
			free(rm_node);
			linked_list_t *list = lg_get_neighbours(post->events, current_node);
			ll_node_t *node = list->head;
			while (node && !repost_parent_found) {
				unsigned int next = *((int *)node->data);
				if (next == repost_node) {
					repost_parent_found = 1;
					lg_remove_edge(post->events, current_node, next);
					break;
				} else {
					push_queue(queue, &next);
				}
				node = node->next;
			}
		}

		destroy_queue(&queue);
		delete_reposts_with_parent_repost(post, repost_node);
	}
}

void delete_reposts_with_parent_repost(post_t *original_post, unsigned int repost_index) {
	unsigned int current_node = repost_index;
	queue_t *queue = init_queue(sizeof(int));
	push_queue(queue, &current_node);
	unsigned int removed_nodes[get_number_reposts(original_post)];
	unsigned int removed_counter = 0;
	while (!is_empty_queue(queue)) {
		current_node = *((int *)peek_queue(queue));
		ll_node_t *rm_node = pop_queue(queue);
		free(rm_node->data);
		free(rm_node);
		linked_list_t *list = lg_get_neighbours(original_post->events, current_node);
		ll_node_t *node = list->head;
		while (node) {
			ll_node_t *next_node = node->next;
			unsigned int next = *((int *)node->data);
			push_queue(queue, &next);
			lg_remove_edge(original_post->events, current_node, next);
			node = next_node;
		}

		post_t *rm_repost = (*((post_t **)original_post->events->data[current_node]));
		free(rm_repost->title);
		dll_free(&rm_repost->likes);
		free(rm_repost);
        ll_free(&list);
		free(original_post->events->data[current_node]);
		original_post->events->data[current_node] = NULL;
		removed_nodes[removed_counter++] = current_node;
	}

	while (removed_counter) {
		unsigned int removed_node = removed_nodes[removed_counter - 1];
		for (unsigned int i = removed_node; i < get_number_reposts(original_post) - 1; i++) {
			original_post->events->data[i] = original_post->events->data[i + 1];
			original_post->events->neighbors[i] = original_post->events->neighbors[i + 1];
		}
		original_post->events->nodes--;
		original_post->events->data = realloc(original_post->events->data, sizeof(void *) * original_post->events->nodes);
		for (unsigned int i = 0; i < get_number_reposts(original_post) - 1; i++) {
			if (original_post->events->data[i] != NULL) {
				linked_list_t *list = lg_get_neighbours(original_post->events, i);
				ll_node_t *node = list->head;
				while (node) {
					unsigned int *next = node->data;
					if (*next > removed_node)
						(*next)--;
					node = node->next;
				}
			}
		}
		removed_counter--;
	}

	destroy_queue(&queue);
}
