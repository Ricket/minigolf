/**
 * testfileparsing.cpp
 * Testing READTOKENSTR to ensure it handles quotes properly.
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

#include "testfileparsing.h"

#include "../src/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void* dotest_fileparsing();

void test_fileparsing() {
	dotest_fileparsing();
}

static void* dotest_fileparsing() {
	char *buffer, *tok;

	int num;
	char *str;

	buffer = (char*)calloc(1,100);

	strcpy(buffer, "course \"NCSU-Express\" 18\n");

	tok = strtok(buffer, FILETOKEN);
	printf("%s should be course\n", tok);

	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be NCSU-Express\n", str);
	READTOKENINT(tok, num, INVALIDTILEDEFINITION);
	printf("%d should be 18\n", num);

	return NULL;
}
