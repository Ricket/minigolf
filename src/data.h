/**
 * data.h
 * Functions for loading/parsing and debug-printing a hole definition file, and
 * data structures for holding this data.
 *
 * Richard Carter
 * 2012/02/17
 * CSC 462 Assignment 2: Ball Physics
 */

#ifndef TILE_H
#define TILE_H

#include "linkedlist.h"

#define ARROW_SPIN_SPEED 0.05f

struct hole;
struct tile;
struct tile_vertex;
struct tile_neighbor;
struct tee;
struct cup;

struct hole * load_hole(char *filename);
void print_hole(struct hole *h);


struct hole {
	struct linkedlist *tiles;
	struct tee *tee;
	struct cup *cup;
};

struct tile {
	int id;
	
	int num_edges;
	struct tile_vertex *vertices;
	struct tile_neighbor *neighbors;

	float norm_x, norm_y, norm_z;
	
	float rotMat[3][3]; /* matrix of rotation ARROW_SPIN_SPEED degrees around norm */
	/* Note rotMat uses mathematical matrix indexing; that is, rotMat[1][2] is
	   the element in the second row, third column. */
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
