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

#define ARROW_SPIN_SPEED 0.037f

#define FILETOKEN " \n"
#define ERRORIFNULL(s,err) { \
	if(!(s)) { \
		printf(err "\n"); \
		return NULL; \
	} \
}
#define INVALIDCOURSEDEFINITION "Invalid course definition"
#define INVALIDTILEDEFINITION "Invalid tile definition"
#define INVALIDTEEDEFINITION "Invalid tee definition"
#define INVALIDCUPDEFINITION "Invalid cup definition"
#define INVALIDPARDEFINITION "Invalid par definition"
#define INVALIDNAMEDEFINITION "Invalid name definition"
#define INVALIDOBJECTDEFINITION "Invalid object definition"
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
	char *end; \
	int len, origlen; \
	\
	tok = strtok(NULL, FILETOKEN); \
	ERRORIFNULL(tok, err); \
	if(tok[0] == '"') { \
		/* change the null terminator back to a space */ \
		origlen = strlen(tok); \
		tok[origlen] = ' '; \
		/* read up to the close quote */ \
		end = strchr(tok+1, '"'); /* tok is quote so search after start quote */ \
		len = end - tok + 1; /* calculate the strlen including quotes */ \
		dest = (char*)calloc(1, len+1-2); /* add 1 to len for a null char, sub 2 for quotes */ \
		dest[len-2] = '\0'; /* put that null char in the dest */ \
		strncpy(dest, tok+1, len-2); \
		/* put a null character over the end quote to prepare for strrchr */ \
		tok[len-1] = '\0'; \
		end = strrchr(tok, ' '); /* find the last space before quote */ \
		/* now put the end quote back */ \
		tok[len-1] = '"'; \
		/* put the null terminator back */ \
		tok[origlen] = '\0'; \
		/* if there is no space then just end */ \
		if(end != NULL) { \
			if(end < tok+origlen) { \
				end = tok+origlen; \
			} \
			/* eat future segments to prepare for the next strtok */ \
			while(tok != NULL && tok < end) { \
				tok = strtok(NULL, FILETOKEN); \
			} \
			/* TODO can probably call strtok with '"' as parameter */ \
		} \
	} else { \
		/* read one word */ \
		len = strlen(tok); \
		dest = (char*)calloc(1, len+1);\
		strncpy(dest, tok, len); \
		dest[len] = '\0'; \
	} \
}

struct course;
struct hole;
struct tile;
struct tile_vertex;
struct tile_neighbor;
struct tee;
struct cup;

struct course * load_course(char *filename);
void free_course(struct course *);
void print_hole(struct hole *h);

struct course {
	char *name;
	int num_holes;
	struct linkedlist *holes;
};

struct hole {
	struct linkedlist *tiles;
	struct linkedlist *objects;
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
