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
	float *x, *y, *z;
	int *tile_ids;
	struct tile *tiles;
};

struct object {

};

#endif
