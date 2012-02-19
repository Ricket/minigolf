/**
 * data.c
 * Functions for loading/parsing and debug-printing a hole definition file.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#include "data.h"
#include "linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define TILE_THICKNESS 0.3f

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

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

static void calculate_normal(struct tile *tile) {
	float x1, y1, z1, x2, y2, z2, len;

	if(tile->num_edges >= 3) {
		x1 = tile->vertices[1].x - tile->vertices[0].x;
		y1 = tile->vertices[1].y - tile->vertices[0].y;
		z1 = tile->vertices[1].z - tile->vertices[0].z;
		
		x2 = tile->vertices[2].x - tile->vertices[0].x;
		y2 = tile->vertices[2].y - tile->vertices[0].y;
		z2 = tile->vertices[2].z - tile->vertices[0].z;
		
		tile->norm_x = y1 * z2 - z1 * y2;
		tile->norm_y = z1 * x2 - x1 * z2;
		tile->norm_z = x1 * y2 - y1 * x2;
		
		/* normalize */
		len = (float) sqrt( (tile->norm_x * tile->norm_x) + (tile->norm_y * tile->norm_y) + (tile->norm_z * tile->norm_z) );
		tile->norm_x /= len;
		tile->norm_y /= len;
		tile->norm_z /= len;
		
		/* calculate rotation matrix */
		tile->rotMat[0][0] = cos(ARROW_SPIN_SPEED) + tile->norm_x*tile->norm_x*(1-cos(ARROW_SPIN_SPEED));
		tile->rotMat[0][1] = tile->norm_x*tile->norm_y*(1-cos(ARROW_SPIN_SPEED)) - tile->norm_z*sin(ARROW_SPIN_SPEED);
		tile->rotMat[0][2] = tile->norm_x*tile->norm_z*(1-cos(ARROW_SPIN_SPEED)) + tile->norm_y*sin(ARROW_SPIN_SPEED);
		tile->rotMat[1][0] = tile->norm_y*tile->norm_x*(1-cos(ARROW_SPIN_SPEED)) + tile->norm_z*sin(ARROW_SPIN_SPEED);
		tile->rotMat[1][1] = cos(ARROW_SPIN_SPEED) + tile->norm_y*tile->norm_y*(1-cos(ARROW_SPIN_SPEED));
		tile->rotMat[1][2] = tile->norm_y*tile->norm_z*(1-cos(ARROW_SPIN_SPEED)) - tile->norm_x*sin(ARROW_SPIN_SPEED);
		tile->rotMat[2][0] = tile->norm_z*tile->norm_x*(1-cos(ARROW_SPIN_SPEED)) - tile->norm_y*sin(ARROW_SPIN_SPEED);
		tile->rotMat[2][1] = tile->norm_z*tile->norm_y*(1-cos(ARROW_SPIN_SPEED)) + tile->norm_x*sin(ARROW_SPIN_SPEED);
		tile->rotMat[2][2] = cos(ARROW_SPIN_SPEED) + tile->norm_z*tile->norm_z*(1-cos(ARROW_SPIN_SPEED));
	}
}

static void fix_height(struct hole *hole) {
	struct listnode *node;
	struct tile *tile;
	float miny = FLT_MAX;
	int i;
	
	/* step 1: determine the minimum height */
	
	node = hole->tiles->first;
	while(node != NULL) {
		tile = (struct tile *)node->ptr;
		
		for(i = 0; i < tile->num_edges; i++) {
			miny = MIN(miny, tile->vertices[i].y);
		}
		
		node = node->next;
	}
	miny = MIN(miny, hole->tee->y);
	miny = MIN(miny, hole->cup->y);
	
	/* step 2: shift every vertex by the miny and 1.0f */
	
	node = hole->tiles->first;
	while(node != NULL) {
		tile = (struct tile *)node->ptr;
		
		for(i = 0; i < tile->num_edges; i++) {
			tile->vertices[i].y = tile->vertices[i].y - miny + TILE_THICKNESS;
		}
		
		node = node->next;
	}
	hole->tee->y = hole->tee->y - miny + TILE_THICKNESS;
	hole->cup->y = hole->cup->y - miny + TILE_THICKNESS;
	
	/* now the tile's lowest point is y=1.0f */
}

static struct tile * get_tile_by_id(int id, struct linkedlist *tiles) {
	if(id == 0) {
		return NULL;
	}
	
	struct listnode *node = tiles->first;
	
	while(node != NULL) {
		if(((struct tile *)node->ptr)->id == id) {
			break;
		}
		node = node->next;
	}
	
	if(node != NULL) {
		return (struct tile *)node->ptr;
	} else {
		printf("Warning: tile id not found: %d\n", id);
		return NULL;
	}
}

static void find_tile_pointers(struct hole *hole) {
	struct listnode *node = hole->tiles->first;
	struct tile *tile;
	int i;
	
	/* tile_neighbors */
	while(node != NULL) {
		tile = (struct tile *)node->ptr;
		
		for(i = 0; i < tile->num_edges; i++) {
			tile->neighbors[i].tile = get_tile_by_id(tile->neighbors[i].id, hole->tiles);
		}
		
		node = node->next;
	}
	
	/* tee */
	hole->tee->tile = get_tile_by_id(hole->tee->tile_id, hole->tiles);
	
	/* cup */
	hole->cup->tile = get_tile_by_id(hole->cup->tile_id, hole->tiles);
}

struct hole * load_hole(char *filename) {
	FILE *fr;
	char buffer[5001];
	char *tok;
	struct hole *hole;
	struct tile *tile;
	int i;

	buffer[5000] = 0; // it should always end in a null char
	
	fr = fopen(filename, "r");
	if(fr == NULL) {
		printf("Error opening file\n");
		return NULL;
	} else {
		hole = (struct hole *) calloc(1, sizeof(struct hole));
		hole->tiles = (struct linkedlist *) calloc(1, sizeof(struct linkedlist));
		
		while(fgets(buffer, 5000, fr) != NULL) {
			// each line of the file...
			
			tok = strtok(buffer, FILETOKEN);
			
			if(strcmp(tok, "tile") == 0) {
				tile = (struct tile *) calloc(1, sizeof(struct tile));
				
				READTOKENINT(tok, tile->id, INVALIDTILEDEFINITION);
				READTOKENINT(tok, tile->num_edges, INVALIDTILEDEFINITION);
				
				tile->vertices = (struct tile_vertex *) calloc(tile->num_edges, sizeof(struct tile_vertex));
				tile->neighbors = (struct tile_neighbor *) calloc(tile->num_edges, sizeof(struct tile_neighbor));
				
				for(i = 0; i < tile->num_edges; i++) {
					READTOKENFLOAT(tok, tile->vertices[i].x, INVALIDTILEDEFINITION);
					READTOKENFLOAT(tok, tile->vertices[i].y, INVALIDTILEDEFINITION);
					READTOKENFLOAT(tok, tile->vertices[i].z, INVALIDTILEDEFINITION);
				}
				
				for(i = 0; i < tile->num_edges; i++) {
					READTOKENINT(tok, tile->neighbors[i].id, INVALIDTILEDEFINITION);
				}
				
				if(tile->num_edges < 3) {
					printf("Warning: tile with < 3 edges (id=%d), continuing anyway\n", tile->id);
				}
				
				calculate_normal(tile);
				
				ll_push_back(hole->tiles, tile);
				tile = NULL;
			} else if(strcmp(tok, "tee") == 0) {
				if(hole->tee != NULL) {
					printf("Warning: more than one tee definition; replacing previous one\n");
					free(hole->tee);
				}
				hole->tee = (struct tee *) calloc(1, sizeof(struct tee));
				
				READTOKENINT(tok, hole->tee->tile_id, INVALIDTEEDEFINITION);
				
				READTOKENFLOAT(tok, hole->tee->x, INVALIDTEEDEFINITION);
				READTOKENFLOAT(tok, hole->tee->y, INVALIDTEEDEFINITION);
				READTOKENFLOAT(tok, hole->tee->z, INVALIDTEEDEFINITION);
			} else if(strcmp(tok, "cup") == 0) {
				if(hole->cup != NULL) {
					printf("Warning: more than one cup definition; replacing previous one\n");
					free(hole->cup);
				}
				hole->cup = (struct cup *) calloc(1, sizeof(struct cup));
				
				READTOKENINT(tok, hole->cup->tile_id, INVALIDCUPDEFINITION);
				
				READTOKENFLOAT(tok, hole->cup->x, INVALIDCUPDEFINITION);
				READTOKENFLOAT(tok, hole->cup->y, INVALIDCUPDEFINITION);
				READTOKENFLOAT(tok, hole->cup->z, INVALIDCUPDEFINITION);
			} else {
				printf("Ignoring unknown command: %s\n", tok);
			}
			
			// use up the rest of the string
			while(tok != NULL) {
				tok = strtok(NULL, FILETOKEN);
			}
		}
		fclose(fr);
		
		/* postprocessing */
		fix_height(hole);
		find_tile_pointers(hole);
		
		return hole;
	}
}

/* printing */
void print_tile(struct tile *t);
void print_tile_vertex(struct tile_vertex *v);
void print_tile_neighbor(struct tile_neighbor *n);
void print_tee(struct tee *t);
void print_cup(struct cup *c);

void print_hole(struct hole *h) {
	struct listnode *node;
	
	node = h->tiles->first;
	while(node != NULL) {
		print_tile((struct tile *)node->ptr);
		node = node->next;
	}
	
	print_tee(h->tee);
	print_cup(h->cup);
}

void print_tile(struct tile *t) {
	int i;
	
	printf("tile %d %d ", t->id, t->num_edges);
	for(i = 0; i < t->num_edges; i++) {
		print_tile_vertex(&t->vertices[i]);
	}
	for(i = 0; i < t->num_edges; i++) {
		print_tile_neighbor(&t->neighbors[i]);
	}
	printf("\n");
}

void print_tile_vertex(struct tile_vertex *v) {
	printf("%f %f %f ", v->x, v->y, v->z);
}

void print_tile_neighbor(struct tile_neighbor *n) {
	printf("%d ", n->id);
}

void print_tee(struct tee *t) {
	printf("tee %d %f %f %f\n", t->tile_id, t->x, t->y, t->z);
}

void print_cup(struct cup *c) {
	printf("cup %d %f %f %f\n", c->tile_id, c->x, c->y, c->z);
}
