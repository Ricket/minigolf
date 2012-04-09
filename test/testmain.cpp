/**
 * testmain.cpp
 * Entry point for the test cases.
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

#ifdef _WIN32
/* disable warning 'unreferenced formal parameter' */
#  pragma warning( disable : 4100 )
#endif

#include "testlinkedlist.h"
#include "testphysics.h"
#include "testfileparsing.h"
#include <stdio.h>

int main(int argc, char** argv) {
	printf("== LINKEDLIST ==\n");
	test_linkedlist();
	printf("\n");
	
	printf("== PHYSICS ==\n");
	test_physics();
	printf("\n");

	printf("== FILE PARSING ==\n");
	test_fileparsing();
	printf("\n");
	
	return 0;
}
