/**
 * testmain.c
 * Entry point for the test cases.
 *
 * Richard Carter
 * 2012/01/27
 * CSC 462 Assignment 1: Minigolf Rendering
 */

#ifdef WIN32
/* disable warning 'unreferenced formal parameter' */
#  pragma warning( disable : 4100 )
#endif

#include "testlinkedlist.h"

int main(int argc, char** argv) {
	test_linkedlist();
	
	return 0;
}
