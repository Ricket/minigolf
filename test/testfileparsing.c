#include "testfileparsing.h"

#include "../src/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void* dotest_fileparsing();

void test_fileparsing() {
	dotest_fileparsing();
}

#define NEWREADTOKENSTR(tok, dest, err) { \
	size_t len; \
	char *newline, *quote, *end; \
	char *tokcpy; \
	tokcpy = tok + strlen(tok) + 1; /* skip over the token to get the rest of line */ \
	/* tokcpy points to the first character of the next token, i.e. one past the null */ \
	ERRORIFNULL(tokcpy, err); \
	if(tokcpy[0] == '"') { \
		tokcpy += 1; \
	} \
	newline = strchr(tokcpy, '\n'); \
	quote = strchr(tokcpy, '"'); \
	if(quote != NULL && (newline == NULL || quote < newline)) { \
		end = quote; /* end character is not included so this will not include quote char */ \
	} else if(newline != NULL) { \
		end = newline; \
	} else { \
		end = strchr(tokcpy, '\0'); \
	} \
	len = end - tokcpy; \
	dest = (char*) calloc(1, len+1); \
	dest[len] = '\0'; \
	strncpy(dest, tokcpy, len); \
	tok = tokcpy;\
}

#define NEWREADTOKENSTR(tok, dest, err) { \
	size_t len; \
	char *newline, *quote, *end; \
	char *tokcpy; \
	tokcpy = tok + strlen(tok) + 1; /* skip over the token to get the rest of line */ \
	/* tokcpy points to the first character of the next token, i.e. one past the null */ \
	ERRORIFNULL(tokcpy, err); \
	if(tokcpy[0] == '"') { \
		tokcpy += 1; \
	} \
	newline = strchr(tokcpy, '\n'); \
	quote = strchr(tokcpy, '"'); \
	if(quote != NULL && (newline == NULL || quote < newline)) { \
		end = quote; /* end character is not included so this will not include quote char */ \
	} else if(newline != NULL) { \
		end = newline; \
	} else { \
		end = strchr(tokcpy, '\0'); \
	} \
	len = end - tokcpy; \
	dest = (char*) calloc(1, len+1); \
	dest[len] = '\0'; \
	strncpy(dest, tokcpy, len); \
	tok = tokcpy;\
}

static void* dotest_fileparsing() {
	char *buffer, *tok;

	int num1, num2, num3;
	char *str1, *str2;

	buffer = (char*)calloc(1,100);

	strcpy(buffer, "command 3 \"one string\" 45 \"two string\" 7\n");

	printf("about to strtok\n");
	tok = strtok(buffer, FILETOKEN);

	READTOKENINT(tok, num1, INVALIDTILEDEFINITION);
	printf("%d should be 3\n", num1);
	NEWREADTOKENSTR(tok, str1, INVALIDNAMEDEFINITION);
	printf("%s should be 'one string'\n", str1);
	READTOKENINT(tok, num2, INVALIDTILEDEFINITION);
	printf("%d should be 45\n", num2);
	NEWREADTOKENSTR(tok, str2, INVALIDNAMEDEFINITION);
	printf("%s should be 'two string'\n", str2);
	READTOKENINT(tok, num3, INVALIDTILEDEFINITION);
	printf("%d should be 7\n", num3);

	return NULL;
}
