/**
 * data.h
 * Functions for loading/parsing and debug-printing a hole definition file, and
 * data structures for holding this data.
 *
 * Richard Carter
 * 2012/01/27
 * CSC 462 Assignment 1: Minigolf Rendering
 */

#ifndef TILE_H
#define TILE_H

#include "linkedlist.h"

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
};

struct tile_vertex {
	float x, y, z;
};

struct tile_neighbor {
	int id;
};

struct tee {
	int tile_id;
	float x, y, z;
};

struct cup {
	int tile_id;
	float x, y, z;
};

#endif
