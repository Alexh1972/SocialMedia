#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "users.h"

void handle_input_feed(char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed")) {
		char *name = strtok(NULL, "\n ");
		unsigned int feed_size = atoi(strtok(NULL, "\n "));
		print_user_feed(name, feed_size);
	}
	else if (!strcmp(cmd, "view-profile")) {
		char *name = strtok(NULL, "\n ");
		print_user_posts(name);
	}
	else if (!strcmp(cmd, "friends-repost")) {
		char *name = strtok(NULL, "\n ");
		unsigned int post_id = atoi(strtok(NULL, "\n "));
		print_friends_name_reposts(name, post_id);
	}
	else if (!strcmp(cmd, "common-group")) {
		char *name = strtok(NULL, "\n ");
		print_friend_clique(name);
	}
	free(commands);
}

void print_user_feed(char *name, unsigned int feed_size) {
	unsigned int user_id = get_user_id(name);
	linked_list_t *posts = get_all_posts();
	ll_node_t *post_node = posts->head;
	while (feed_size && post_node) {
		post_t *post = get_post_from_node(post_node);
		if (is_user_in_friends_list(user_id, post->user_id) || user_id == post->user_id) {
			printf("%s: \"%s\"\n", get_user_name(post->user_id), post->title);
			feed_size--;
		}
		post_node = post_node->next;
	}
}


static int compare_reposts_by_creation_order(const void *a, const void *b)
{
   const post_t *first = a;
   const post_t *second = b;

   return first->id - second->id;
}

post_t **get_friends_reposts(post_t *post, unsigned int user_id, unsigned int *no_posts) {
	post_t **friends_reposts = NULL;
	unsigned int number_posts = 0;

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
			if (is_user_in_friends_list(user_id, repost->user_id)) {
				if (!number_posts) {
					number_posts++;
					friends_reposts = malloc(number_posts * sizeof(post_t *));
					friends_reposts[number_posts - 1] = repost;
				} else {
					number_posts++;
					friends_reposts = realloc(friends_reposts, number_posts * sizeof(post_t *));
					friends_reposts[number_posts - 1] = repost;
				}
			}
			push_queue(queue, &next);
			repost_node = repost_node->next;
		}
	}

	destroy_queue(&queue);

	qsort(friends_reposts, number_posts, sizeof(post_t *), compare_reposts_by_creation_order);

	if (no_posts)
		*no_posts = number_posts;

	return friends_reposts;
}

void print_friends_name_reposts(char *name, unsigned int post_id) {
	post_t *post = get_post_by_id(post_id);
	unsigned int number_reposts = 0;
	unsigned int user_id = get_user_id(name);
	post_t **friends_reposts = get_friends_reposts(post, user_id, &number_reposts);
	printf("Friends that reposted:\n");
	for (unsigned int i = 0; i < number_reposts; i++) {
		printf("%s\n", get_user_name(friends_reposts[i]->user_id));
	}

	free(friends_reposts);

}

void print_user_posts(char *name) {
	unsigned int number_posts = 0;
	profile_post_t **posts = get_user_profile_posts(get_user_id(name), &number_posts);
	for (unsigned int i = 0; i < number_posts; i++) {
		if (posts[i]->original_post_title == NULL)
			printf("Posted: \"%s\"\n", posts[i]->post->title);
		else
			printf("Reposted: \"%s\"\n", posts[i]->original_post_title);
		free_profile_post(posts[i]);
	}

	free(posts);
}

void print_friend_clique(char *name) {
	unsigned int size = 0;
	unsigned int *friends_clique_id = get_friends_clique(name, &size);
	printf("The closest friend group of %s is:\n", name);
	for (unsigned int i = 0; i < size; i++) {
		printf("%s\n", get_user_name(friends_clique_id[i]));
	}

	free(friends_clique_id);
}