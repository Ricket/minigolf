/**
 * testlinkedlist.cpp
 * Test cases for the linkedlist functions and data structures.
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

#include "testlinkedlist.h"

#include "../src/linkedlist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static void printf_linkedlist_int(struct linkedlist *list);


void test_linkedlist() {
	struct linkedlist *list1;
	int test, test2, test3;
	
	test = 1;
	test2 = 2;
	test3 = 3;
	
	list1 = (struct linkedlist *) calloc(1, sizeof(struct linkedlist));
	ll_push_front(list1, &test);
	printf_linkedlist_int(list1);
	// test
	
	assert(list1->length == 1);
	assert(list1->first->ptr == &test);
	assert(list1->last->ptr == &test);
	assert(list1->first == list1->last);
	assert(list1->first->next == NULL);
	assert(list1->last->prev == NULL);
	
	ll_push_front(list1, &test2);
	printf_linkedlist_int(list1);
	// test2, test
	
	assert(list1->length == 2);
	assert(list1->first->ptr == &test2);
	assert(list1->last->ptr == &test);
	assert(list1->first->next->ptr == &test);
	assert(list1->last->prev->ptr == &test2);
	assert(list1->first->next->next == NULL);
	assert(list1->last->prev->prev == NULL);
	
	ll_push_back(list1, &test3);
	printf_linkedlist_int(list1);
	// test2, test, test3
	
	assert(list1->length == 3);
	assert(list1->first->ptr == &test2);
	assert(list1->last->ptr == &test3);
	assert(list1->first->next->next->ptr == &test3);
	assert(list1->last->prev->prev->ptr == &test2);
	
	ll_remove(list1, &test);
	printf_linkedlist_int(list1);
	// test2, test3
	
	assert(list1->length == 2);
	assert(list1->first->ptr == &test2);
	assert(list1->last->ptr == &test3);
	assert(list1->first->next->ptr == &test3);
	assert(list1->last->prev->ptr == &test2);
	assert(list1->first == list1->last->prev);
	assert(list1->last == list1->first->next);
	
	printf("%d %d\n", *((int*)ll_pop_front(list1)), *((int*)ll_pop_back(list1)));
	printf_linkedlist_int(list1);
	// [empty]
	
	assert(list1->length == 0);
	assert(list1->last == NULL);
	assert(list1->first == NULL);
	
	free(list1);
}

static void printf_linkedlist_int(struct linkedlist *list) {
	struct listnode *node;
	
	if(list == NULL) {
		printf("NULL\n");
		return;
	}
	
	printf("Length: %d\n", list->length);
	
	printf("Forward: ");
	node = list->first;
	while(node != NULL) {
		printf("%d ", *((int*)node->ptr));
		node = node->next;
	}
	printf("\n");
	
	printf("Reverse: ");
	node = list->last;
	while(node != NULL) {
		printf("%d ", *((int*)node->ptr));
		node = node->prev;
	}
	printf("\n");
}