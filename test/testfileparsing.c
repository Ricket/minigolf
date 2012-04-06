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

	strcpy(buffer, "command \"zero string\" 3 \"one string\" 45 twostring threestring \"four str ing\" 7 \"fivestring\"\n");

	printf("about to strtok\n");
	tok = strtok(buffer, FILETOKEN);
	printf("%s should be command\n", tok);

	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be zero string\n", str);
	READTOKENINT(tok, num, INVALIDTILEDEFINITION);
	printf("%d should be 3\n", num);
	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be one string\n", str);
	READTOKENINT(tok, num, INVALIDTILEDEFINITION);
	printf("%d should be 45\n", num);
	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be twostring\n", str);
	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be threestring\n", str);
	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be four str ing\n", str);
	READTOKENINT(tok, num, INVALIDTILEDEFINITION);
	printf("%d should be 7\n", num);
	READTOKENSTR(tok, str, INVALIDNAMEDEFINITION);
	printf("%s should be fivestring\n", str);

	return NULL;
}
