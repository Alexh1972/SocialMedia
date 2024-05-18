### Team for homework 3 SD:

* alexandru.cazacu05 312CA
* andrei.giurgiu0801 315CA

## Title + homework number

* Social Network - Homework 3 Data Structures and Algorithms

### Description

* In this homework, we implemented SDAgram, an application that simulates a social media network.
* At the moment, the application works only in the terminal and it doesn't have a GUI, which will be implemented in the future.
* The homework is divided into three functionalities : **friends**, **posts** and **feed**.

### First functionality - Friends

* In the first task, we had to implement the functionality of having friendship connections between users.
* For this, we have used a graph where each node represents a user and each edge represents the fact that the two users are friends.
* Every friendship connection is mutual, so the graph is unoriented.
* The graph was implemented using a resizable array of linked lists.
* The friend network has the following functionalities : add friend connection, remove friend connection, show common friends between two users, show suggested friends for a user etc.

### Second functionality - Posts

* In the second task, we had to implement the functionality of letting users post different messages.
* These posts can get likes from other users, and every user can see who liked his posts.
* Also, every post can get reposted by other users.
* A repost will have all the attributes of a post, but as soon as the original post is deleted, all reposts will also be deleted.
* All the posts will be held in a linked list and each post will have a unique identifier.

### Third functionality - Feed

* In the third task, we had to implement the functionality of a user having a feed of posts.
* The feed will provide the profiles of the friends and the posts and reposts of the user's friends.
* Also, the feed has a very impressive functionality : the **clique**. This shows the user his biggest group of friends.

### Specialised libraries

* For an easy implementation of this social network, the program contains specialised libraries for linked lists, both simple and double, hashmaps, queues and graphs, both unoriented and oriented.

### List library

* The files **linked_list.h** and **linked_list.c** contain the implementation of the basic operations for working with simple and double linked lists: memory allocation, adding a node to a certain position, removing a node from a certain position, freeing memory, etc.

### Hashmap library

* The files **hashtable.h** and **hashtable.c** contain the implementation of the basic operations for working with hashmap: memory allocation, hash functions, adding to the hashmap, removing from the hashmap, obtaining a value based on a key, etc.
* The implementation of the hashmap is done by means of an array of pointers to a simple linked list.

### Queue library

* The files **queue.h** and **queue.c** contain the implementation of operations for working with the queue: memory allocation, push, pop, peek, free memory etc.
* The implementation of the queue is done by means of a simple linked list.

### Graph library

* The files **list_graph.h** and **list_graph.c** contain the implementation of basic and advanced operations for working with graphs, both unoriented and oriented: memory allocation, add node to the graph, add edge to the graph, remove edge from the graph, finding the lowest common ancestor.

### Resources

1. List, Hashmap, Queue and Graph implementations from Lambdachecker.
