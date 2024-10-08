/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"

/**
 * Initializez every task based on which task we are running
*/
void init_tasks(void)
{
	#ifdef TASK_1

	#endif

	#ifdef TASK_2

	#endif

	#ifdef TASK_3

	#endif
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	init_tasks();

	char *input = (char *)malloc(MAX_COMMAND_LEN);
	char *aux = input;
	while (1) {
		input = fgets(input, MAX_COMMAND_LEN, stdin);
		// If fgets returns null, we reached EOF
		if (!input)
			break;

		#ifdef TASK_1
		handle_input_friends(input);
		#endif

		#ifdef TASK_2
		handle_input_posts(input);
		#endif

		#ifdef TASK_3
		handle_input_feed(input);
		#endif
	}

	#ifdef TASK_1
	free_friends();
	#endif

	#ifdef TASK_2
	free_posts();
	#endif
	free_users();
	free(aux);

	return 0;
}
