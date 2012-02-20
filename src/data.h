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
