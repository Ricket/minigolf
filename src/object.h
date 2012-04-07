#ifndef OBJECT_H
#define OBJECT_H

struct linkedlist;
struct tile;


struct polygon {
	int num_edges;
	float *x, *y, *z; /* arrays holding the x,y,z positions */
	float r,g,b;
};

struct boundingbox {
	int num_points;
	float *x, *y, *z;
};

struct object {
	struct linkedlist *polys;
	struct boundingbox *bbox;
	int *tile_ids;
	struct tile *tiles;
	int num_tiles;
};

#endif
