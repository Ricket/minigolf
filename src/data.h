/**
 * data.h
 * Functions for loading/parsing and debug-printing a hole definition file, and
 * data structures for holding this data.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#ifndef DATA_H
#define DATA_H

#include "linkedlist.h"

#define ARROW_SPIN_SPEED 0.04f

#define FILETOKEN " \n"
#define ERRORIFNULL(s,err) { \
	if(!(s)) { \
		printf(err "\n"); \
		return NULL; \
	} \
}
#define INVALIDTILEDEFINITION "Invalid tile definition"
#define INVALIDTEEDEFINITION "Invalid tee definition"
#define INVALIDCUPDEFINITION "Invalid cup definition"
#define INVALIDPARDEFINITION "Invalid par definition"
#define INVALIDNAMEDEFINITION "Invalid name definition"
#define READTOKENINT(tok, dest, err) { \
	tok = strtok(NULL, FILETOKEN); \
	ERRORIFNULL(tok, err); \
	dest = atoi(tok); \
}
#define READTOKENFLOAT(tok, dest, err) { \
	tok = strtok(NULL, FILETOKEN); \
	ERRORIFNULL(tok, err); \
	dest = (float) atof(tok); \
}
#define READTOKENSTR(tok, dest, err) { \
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

struct course;
struct hole;
struct tile;
struct tile_vertex;
struct tile_neighbor;
struct tee;
struct cup;

struct hole * load_hole(char *filename);
void print_hole(struct hole *h);

struct course {
	char *name;
	int num_holes;
	struct linkedlist *holes;
};

struct hole {
	struct linkedlist *tiles;
	struct tee *tee;
	struct cup *cup;
	int par;
	char *name;
};

struct tile {
	int id;
	
	int num_edges;
	struct tile_vertex *vertices;
	struct tile_neighbor *neighbors;
	float **edgeRotMat; /* array of matrices of rotation around the edges */

	float norm_x, norm_y, norm_z;
	
	/* Note matrices use mathematical matrix indexing; that is, mat[1][2] is
	   the element in the second row, third column. */
	float rotMat[3][3]; /* matrix of rotation ARROW_SPIN_SPEED degrees around norm */
	float projMat[3][3]; /* matrix of rotation to bring tile's verts along XY axis */
	struct tile_vertex *proj_vertices;
};

struct tile_vertex {
	float x, y, z;
};

struct tile_neighbor {
	int id;
	
	struct tile *tile;
};

struct tee {
	int tile_id;
	float x, y, z;
	
	struct tile *tile;
};

struct cup {
	int tile_id;
	float x, y, z;
	
	struct tile *tile;
};

#endif
