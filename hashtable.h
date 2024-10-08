#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "linked_list.h"

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Array of pointers to linked list */
	/* The current number of nodes in the buckets */
	unsigned int size;
	unsigned int hmax; /* Number of buckets */
	/* Pointer to a function to calculate the hash of the argument
	 */
	unsigned int (*hash_function)(void *argument);
	/* Pointer to a function to compare two keys */
	int (*compare_function)(void *arg_one, void *arg_two);
	/* Pointer to a function to free the memory allocated for a pair of
	 * (key, value) */
	void (*key_val_free_function)(void *arg);
};

int compare_function_ints(void *a, void *b);
int compare_function_strings(void *a, void *b);
unsigned int hash_function_int(void *a);
unsigned int hash_function_string(void *a);
void key_val_free_function(void *data);
void destroy_nodes(void *data);
hashtable_t *ht_create(unsigned int hmax,
					   unsigned int (*hash_function)(void *),
					   int (*compare_function)(void *, void *),
					   void (*key_val_free_function)(void *));
int ht_has_key(hashtable_t *ht, void *key);
void *ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size, void *value,
			unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);

#endif
