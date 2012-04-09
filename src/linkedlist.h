/**
 * linkedlist.h
 * Linked list function declarations and data structures.
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct listnode {
	void *ptr;
	
	struct listnode *next, *prev;
};

struct linkedlist {
	int length;
	struct listnode *first, *last;
};

void ll_push_front(struct linkedlist *list, void *thing);
void ll_push_back(struct linkedlist *list, void *thing);
void ll_remove(struct linkedlist *list, void *thing);
void* ll_pop_front(struct linkedlist *list);
void* ll_pop_back(struct linkedlist *list);
void ll_empty(struct linkedlist *list);

#endif
