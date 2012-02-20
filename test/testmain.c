/**
 * testmain.c
 * Entry point for the test cases.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#ifdef _WIN32
/* disable warning 'unreferenced formal parameter' */
#  pragma warning( disable : 4100 )
#endif

#include "testlinkedlist.h"
#include "testphysics.h"
#include <stdio.h>

int main(int argc, char** argv) {
	printf("== LINKEDLIST ==\n");
	test_linkedlist();
	printf("\n");
	
	printf("== PHYSICS ==\n");
	test_physics();
	printf("\n");
	
	return 0;
}
