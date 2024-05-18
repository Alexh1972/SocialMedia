#ifndef POSTS_H
#define POSTS_H

#include "list_graph.h"

typedef struct post_t {
	unsigned int id;
	char *title;
	unsigned int user_id;
	list_graph_t *events;
	doubly_linked_list_t *likes;
} post_t;

typedef struct profile_post_t
{
	post_t *post;
	char *original_post_title;
} profile_post_t;

/**
 * @brief Function that will parse and execute the given query
 * @param input The query that will be parsed and executed
 */
void handle_input_posts(char *input);

/**
 * @brief This function will add a new post to a user's profile
 * @param username The user that creates the post
 * @param title The title of the post
 * @return 1, if the post was created successfully, 0 otherwise
 */
unsigned int add_post(char *username, char *title);

/**
 * @brief This function will create a post
 *
 * @param id The id of the post
 * @param title The title of the post
 * @param user_id The user that creates the post
 * @param is_repost Whether the post is a repost or not
 * @return The created post
 */
post_t
*create_post(unsigned int id, char *title, unsigned int user_id,
			unsigned int is_repost);

/**
 * @brief This function will print all existing posts
 */
void print_posts(void);

/**
 * @brief This function will print only one post
 * @param post The post to be printed
 */
void print_post(post_t *post);

/**
 * @brief This function frees the memory allocated for the posts
 */
void free_posts(void);

/**
 * @brief This function prints the elements from the posts graph
 * @param graph The posts graph
 */
void lg_print_post(list_graph_t *graph);

/**
 * @brief This function will try to find the common repost, if exists
 *
 * @param post The id of the original post
 * @param first_repost The id of the first repost
 * @param second_repost The id of the second repost
 * @return The id of the common repost
 */
int find_common_repost(unsigned int post, unsigned int first_repost,
					   unsigned int second_repost);

/**
 * @brief This function prints the common repost, if exists
 * @param anc Whether the common repost exists
 * @param fst The id of the first repost
 * @param sec The id of the second repost
 */
void print_common_repost(int anc, unsigned int fst, unsigned int sec);

/**
 * @brief This function adds a new repost to a user's profile
 * @param name User created the new repost
 * @param post_id The id of the original post
 * @param repost_id The id of the repost
 * @return 1, if the repost was created successfully, 0 otherwise
 */
unsigned int add_repost(char *name, unsigned int post_id,
						unsigned int repost_id);

/**
 * @brief This function will get the post from a node in the post graph
 * @param node The node in the graph
 * @return Pointer to the post
 */
post_t *get_post_from_node(ll_node_t *node);

/**
 * @brief This function gets the repost index from the repost graph associated
 * to every post
 * @param post The original post
 * @param repost_id The id of the repost to be searched
 * @return The repost index if it was found, -1 otherwise
 */
unsigned int get_repost_index(post_t *post, unsigned int repost_id);

/**
 * @brief This function adds a new user in the likes list of a post/repost
 * or removes a user from it
 * @param name The username of the user
 * @param post_id The id of the post
 * @param repost_id The id of the repost
 * @return 1, if the user was added or 0, if the user was removed
 */
unsigned int
update_post_likes(char *name, unsigned int post_id, unsigned int repost_id);

/**
 * @brief This function gets the post from the posts lists with the provided
 * id
 * @param post_id The id of the post
 * @return Pointer to the post
 */
post_t *get_post_by_id(unsigned int post_id);

/**
 * @brief Function to get the title of the post with the id of the post
 * @param post_id The id of the post
 * @return The title of the post
 */
char *get_post_title_by_id(unsigned int post_id);

/**
 * @brief This function will print the liked post/repost by a user
 * @param name The username
 * @param rep_id The id of the repost, or 0 if it is a post
 * @param post_id The id of the post
 */
void print_liked_posts(char *name, int rep_id, int post_id);

/**
 * @brief This function will print the unliked post/repost by a user
 * @param name The username
 * @param rep_id The id of the repost, or 0 if it is a post
 * @param post_id The id of the post
 */
void print_unliked_posts(char *name, int rep_id, int post_id);

/**
 * @brief Get the number of likes for the post/repost
 * @param post_id The id of the post
 * @param repost_id The id of the repost, or 0 if it is a post
 * @return The number of likes of the post/repost
 */
unsigned int get_post_likes(unsigned int post_id, unsigned int repost_id);

/**
 * @brief This function gets the node associated with the user id
 * from the likes list
 * @param post The post
 * @param user_id The id of the user
 * @return A pointer to the node or NULL if it doesn't exist
 */
dll_node_t *get_user_like_node(post_t *post, unsigned int user_id);

/**
 * @brief This function gets the id of the repost with the most likes
 * @param post_id The id of the original post
 * @return The id of the repost with the most likes
 */
unsigned int get_maximum_likes_repost_id(unsigned int post_id);

/**
 * @brief This function gets the number of reposts of a post
 * @param post The original post
 * @return Number of reposts
 */
unsigned int get_number_reposts(post_t *post);

post_t *get_repost_by_id(unsigned int repost_id, unsigned int *index);

/**
 * @brief This function prints the repost of a post
 * @param post_id The id of the original post
 * @param repost_id The id of the repost
 */
void print_reposts(unsigned int post_id, unsigned int repost_id);

/**
 * @brief This function removes a repost from a post graph
 * @param post_id The id of the original post
 * @param repost_id The id of the repost
 */
void delete_repost(unsigned int post_id, unsigned int repost_id);

/**
 * @brief This function removes a post from the list of posts
 * and deletes all reposts of it
 * @param post_id The id of the post
 */
void remove_post(unsigned int post_id);

/**
 * @brief This function deletes a repost of a post and all associated
 * reposts of that repost
 * @param original_post The original post
 * @param repost_index The index of the repost
 */
void delete_reposts_with_parent_repost(post_t *original_post,
									   unsigned int repost_index);

/**
 * @brief Function to get the all the posts from the posts list
 * @return Pointer to the posts list
 */
linked_list_t *get_all_posts(void);

/**
 * @brief Function to get all posts from a users profile
 * @param user_id Id of the user
 * @param no_posts The number of posts
 * @return profile_post_t**
 */
profile_post_t
**get_user_profile_posts(unsigned int user_id, unsigned int *no_posts);

/**
 * @brief Function to create a new profile post
 * @param post The post that will be the new profile post
 * @param original_post_title The original title of the post
 * @return The profile post created
 */
profile_post_t *create_profile_post(post_t *post, char *original_post_title);

/**
 * @brief Function to free the memory allocated for a profile post
 * @param profile_post The profile post to be freed
 */
void free_profile_post(profile_post_t *profile_post);

/**
 * @brief Function to get a repost by index
 * @param post The original post
 * @param index The index of the repost
 * @return post_t*
 */
post_t *get_repost_by_index(post_t *post, unsigned int index);

#endif
