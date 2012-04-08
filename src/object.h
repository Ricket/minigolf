#ifndef OBJECT_H
#define OBJECT_H

struct linkedlist;
struct tile;


struct polygon {
	int num_edges;
	float *x, *y, *z; /* arrays holding the x,y,z positions */
	float r,g,b;
	/* TODO NORMAL */
};

struct boundingbox {
	int num_points;
	float *x, *y, *z;

	float **edgeRotMat; /* array of matrices of rotation around the edges */
	float **edgeNorm; /* array of norm vectors */
};

struct object {
	struct linkedlist *polys;
	struct boundingbox *bbox;
	int *tile_ids;
	struct tile *tiles;
	int num_tiles;

	float *transformation;
};

void apply_static_transformation(struct object *);
void render_object(struct object *);

#endif
