CC=gcc
CFLAGS=-Wall -Wextra -Werror -g -lm # -fsanitize=address,undefined

.PHONY: build clean

all: build

build: friends posts feed

# Any other object file that is needed for the other files
# add it here, separated by space
UTILS = users.o linked_list.o queue.o list_graph.o hashtable.o

friends: $(UTILS) friends.o social_media_friends.o
	$(CC) -o $@ $^ $(CFLAGS)

posts: $(UTILS) posts.o social_media_posts.o
	$(CC) -o $@ $^ $(CFLAGS)
	
feed: $(UTILS) posts.o friends.o feed.o social_media_feed.o
	$(CC) -o $@ $^ $(CFLAGS)

social_media_friends.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -o $@ social_media.c

social_media_posts.o:
	$(CC) $(CFLAGS) -c -D TASK_2 -o $@ social_media.c

social_media_feed.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -D TASK_2 -D TASK_3 -o $@ social_media.c

run_posts_valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./posts < checker/input/00-posts.in

run_debug_friends: friends run clean

run_feed_valgrind: feed
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./feed < checker/input/00-feed.in

run_friends_valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./friends < checker/input/00-friends.in

run_debug: posts run_feed_valgrind clean

clean:
	rm -rf *.o friends posts feed