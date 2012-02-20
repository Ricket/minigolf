/**
 * linkedlist.c
 * Linked list function definitions.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#include "linkedlist.h"

#include <stdlib.h>

void ll_push_front(struct linkedlist *list, void *thing) {
	struct listnode *newnode;
	
	newnode = (struct listnode *) calloc(1, sizeof(struct listnode));
	newnode->ptr = thing;
	
	if(list->first == NULL) {
		list->first = list->last = newnode;
	} else {
		newnode->next = list->first;
		list->first = newnode;
		newnode->next->prev = newnode;
	}
	
	list->length++;
}

void ll_push_back(struct linkedlist *list, void *thing) {
	struct listnode *newnode;
	
	newnode = (struct listnode *) calloc(1, sizeof(struct listnode));
	newnode->ptr = thing;
	
	if(list->last == NULL) {
		list->last = list->first = newnode;
	} else {
		newnode->prev = list->last;
		list->last = newnode;
		newnode->prev->next = newnode;
	}
	
	list->length++;
}

void ll_remove(struct linkedlist *list, void *thing) {
	struct listnode *curr;
	
	curr = list->first;
	while(curr != NULL) {
		if(curr->ptr == thing) {
			if(curr == list->first) {
				list->first = curr->next;
			}
			if(curr == list->last) {
				list->last = curr->prev;
			}
			if(curr->prev != NULL) {
				curr->prev->next = curr->next;
			}
			if(curr->next != NULL) {
				curr->next->prev = curr->prev;
			}
			free(curr);
			list->length--;
		}
		curr = curr->next;
	}
}

void* ll_pop_front(struct linkedlist *list) {
	struct listnode *node;
	void *data;
	
	node = list->first;
	
	if(node != NULL) {
		list->first = node->next;
		if(list->first != NULL) {
			list->first->prev = NULL;
		}
		if(list->last == node) {
			list->last = node->next; // == NULL in all cases, I think
			if(list->last != NULL) {
				list->last->next = NULL;
			}
		}
		data = node->ptr;
		free(node);
		list->length--;
		return data;
	} else {
		return NULL;
	}
}

void* ll_pop_back(struct linkedlist *list) {
	struct listnode *node;
	void *data;
	
	node = list->last;
	
	if(node != NULL) {
		list->last = node->prev;
		if(list->last != NULL) {
			list->last->next = NULL;
		}
		if(list->first == node) {
			list->first = node->prev; // == NULL in all cases, I think
			if(list->first != NULL) {
				list->first->prev = NULL;
			}
		}
		data = node->ptr;
		free(node);
		list->length--;
		return data;
	} else {
		return NULL;
	}
}

void ll_empty(struct linkedlist *list) {
	while(ll_pop_front(list) != NULL);
}
