/**
 * physics.cpp
 * Physics functions
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

#include "physics.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "data.h"
#include "object.h"
#include "ballcuptee.h"

bool ball_in_tile(struct ball *ball) {
	int i, j;
	bool inTile = false;
	
	if(ball->tile == NULL) {
		printf("Warning: ball_in_tile called with tile == NULL, returning false");
		return false;
	}
	
	/* point-in-polygon algorithm found at:
	   http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	*/
	for (i = 0, j = ball->tile->num_edges-1; i < ball->tile->num_edges; j = i++) {
		if ( ((ball->tile->proj_vertices[i].z > ball->z) != (ball->tile->proj_vertices[j].z > ball->z)) &&
				(ball->x < (ball->tile->proj_vertices[j].x - ball->tile->proj_vertices[i].x) * (ball->z - ball->tile->proj_vertices[i].z) / (ball->tile->proj_vertices[j].z - ball->tile->proj_vertices[i].z) + ball->tile->proj_vertices[i].x) ) {
			inTile = !inTile;
		}
	}
	return inTile;
}

bool ball_in_obj(struct ball *ball, struct object *obj) {
	int i, j;
	struct boundingbox *bbox;
	bool inTile = false;

	bbox = obj->bbox;
	if(bbox == NULL || bbox->num_points < 3) {
		return false;
	}
	
	/* point-in-polygon algorithm found at:
	   http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	*/
	for (i = 0, j = bbox->num_points-1; i < bbox->num_points; j = i++) {
		if ( ((bbox->z[i] > ball->z) != (bbox->z[j] > ball->z)) &&
				(ball->x < (bbox->x[j] - bbox->x[i]) * (ball->z - bbox->z[i]) / (bbox->z[j] - bbox->z[i]) + bbox->x[i]) ) {
			inTile = !inTile;
		}
	}
	return inTile;
}

static float distance_sq(float x0, float y0, float z0, float x1, float y1, float z1) {
	return (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) + (z1-z0)*(z1-z0);
}

static float dot(float x0, float y0, float z0, float x1, float y1, float z1) {
	return x0*x1 + y0*y1 + z0*z1;
}

static float get_shortest_distance_sq(struct tile_vertex *p0, struct tile_vertex *p1, float x, float y, float z) {
	float lsq, t;
	
	lsq = distance_sq(p0->x, p0->y, p0->z, p1->x, p1->y, p1->z);
	if(lsq == 0.0f) {
		/* p0 and p1 are the same; invalid tile, but oh well */
		return distance_sq(x, y, z, p0->x, p0->y, p0->z);
	}
	
	t = dot(x - p0->x, y - p0->y, z - p0->z, p1->x - p0->x, p1->y - p0->y, p1->z - p0->z);
	t /= lsq;
	if(t <= 0.0f) {
		return distance_sq(x, y, z, p0->x, p0->y, p0->z);
	} else if(t >= 1.0f) {
		return distance_sq(x, y, z, p1->x, p1->y, p1->z);
	} else {
		return distance_sq(x, y, z,
			p0->x + t * (p1->x - p0->x),
			p0->y + t * (p1->y - p0->y),
			p0->z + t * (p1->z - p0->z)
			);
	}
}

static float get_shortest_distance_sq_obj(float x0, float y0, float z0, float x1, float y1, float z1, float x, float y, float z) {
	float lsq, t;
	
	lsq = distance_sq(x0, y0, z0, x1, y1, z1);
	if(lsq == 0.0f) {
		/* p0 and p1 are the same; invalid tile, but oh well */
		return distance_sq(x, y, z, x0, y0, z0);
	}
	
	t = dot(x - x0, y - y0, z - z0, x1 - x0, y1 - y0, z1 - z0);
	t /= lsq;
	if(t <= 0.0f) {
		return distance_sq(x, y, z, x0, y0, z0);
	} else if(t >= 1.0f) {
		return distance_sq(x, y, z, x1, y1, z1);
	} else {
		return distance_sq(x, y, z,
			x0 + t * (x1 - x0),
			y0 + t * (y1 - y0),
			z0 + t * (z1 - z0)
			);
	}
}

float get_distance_sq_to_edge(struct ball *ball, int edge) {
	struct tile_vertex *p0, *p1;
	
	assert(ball->tile != NULL);
	assert(edge < ball->tile->num_edges);
	
	p0 = &(ball->tile->vertices[edge]);
	p1 = &(ball->tile->vertices[(edge+1) % ball->tile->num_edges]);
	
	return get_shortest_distance_sq(p0, p1, ball->x, ball_y(ball), ball->z);
}

float get_distance_sq_to_obj_edge(struct ball *ball, struct object *obj, int edge) {
	int edge2;
	
	edge2 = (edge+1) % obj->bbox->num_points;
	
	/* return get_shortest_distance_sq(p0, p1, ball->x, ball->y, ball->z); */
	return get_shortest_distance_sq_obj(
		obj->bbox->x[edge], obj->bbox->y[edge], obj->bbox->z[edge],
		obj->bbox->x[edge2], obj->bbox->y[edge2], obj->bbox->z[edge2],
		ball->x, ball_y(ball), ball->z
		);
}

int get_closest_edge(struct ball *ball) {
	int closestEdge = 0;
	float closestEdgeDist = get_distance_sq_to_edge(ball, 0);
	int i;
	float d;
	
	for(i = 1; i < ball->tile->num_edges; i++) {
		d = get_distance_sq_to_edge(ball, i);

		if(d < closestEdgeDist) {
			closestEdge = i;
			closestEdgeDist = d;
		}
	}
	
	return closestEdge;
}

int get_closest_edge_obj(struct ball *ball, struct object *obj) {
	int closestEdge = 0;
	float closestEdgeDist = get_distance_sq_to_obj_edge(ball, obj, 0);
	int i;
	float d;
	
	for(i = 1; i < ball->tile->num_edges; i++) {
		d = get_distance_sq_to_obj_edge(ball, obj, i);

		if(d < closestEdgeDist) {
			closestEdge = i;
			closestEdgeDist = d;
		}
	}
	
	return closestEdge;
}

void bounce_ball(struct ball *ball, int edge) {
	struct tile_vertex *p0;
	float x, y, z;
	float balldy;
	
	p0 = &(ball->tile->vertices[edge]);
	
	x = ball->x;
	y = ball_y(ball);
	z = ball->z;
	
	x -= p0->x;
	y -= p0->y;
	z -= p0->z;
	
	x = ball->tile->edgeRotMat[edge][0]*x + ball->tile->edgeRotMat[edge][1]*y + ball->tile->edgeRotMat[edge][2]*z;
	y = ball->tile->edgeRotMat[edge][3]*x + ball->tile->edgeRotMat[edge][4]*y + ball->tile->edgeRotMat[edge][5]*z;
	z = ball->tile->edgeRotMat[edge][6]*x + ball->tile->edgeRotMat[edge][7]*y + ball->tile->edgeRotMat[edge][8]*z;
	
	x += p0->x;
	y += p0->y;
	z += p0->z;
	
	ball->x = x;
	ball->z = z;
	
	/* now the velocity */
	balldy = ball_dy(ball);
	x = ball->tile->edgeRotMat[edge][0]*ball->dx + ball->tile->edgeRotMat[edge][1]*balldy + ball->tile->edgeRotMat[edge][2]*ball->dz;
	y = ball->tile->edgeRotMat[edge][3]*ball->dx + ball->tile->edgeRotMat[edge][4]*balldy + ball->tile->edgeRotMat[edge][5]*ball->dz;
	z = ball->tile->edgeRotMat[edge][6]*ball->dx + ball->tile->edgeRotMat[edge][7]*balldy + ball->tile->edgeRotMat[edge][8]*ball->dz;
	
	ball->dx = x;
	ball->dz = z;
}

void bounce_ball_bbox(struct ball *ball, struct boundingbox *bbox, int edge) {
	float x, y, z;
	float balldy;
	
	x = ball->x;
	y = ball_y(ball);
	z = ball->z;
	
	x -= bbox->x[edge];
	y -= bbox->y[edge];
	z -= bbox->z[edge];
	
	x = bbox->edgeRotMat[edge][0]*x + bbox->edgeRotMat[edge][1]*y + bbox->edgeRotMat[edge][2]*z;
	y = bbox->edgeRotMat[edge][3]*x + bbox->edgeRotMat[edge][4]*y + bbox->edgeRotMat[edge][5]*z;
	z = bbox->edgeRotMat[edge][6]*x + bbox->edgeRotMat[edge][7]*y + bbox->edgeRotMat[edge][8]*z;
	
	x += bbox->x[edge];
	y += bbox->y[edge];
	z += bbox->z[edge];
	
	ball->x = x;
	ball->z = z;
	
	/* now the velocity */
	balldy = ball_dy(ball);
	x = bbox->edgeRotMat[edge][0]*ball->dx + bbox->edgeRotMat[edge][1]*balldy + bbox->edgeRotMat[edge][2]*ball->dz;
	y = bbox->edgeRotMat[edge][3]*ball->dx + bbox->edgeRotMat[edge][4]*balldy + bbox->edgeRotMat[edge][5]*ball->dz;
	z = bbox->edgeRotMat[edge][6]*ball->dx + bbox->edgeRotMat[edge][7]*balldy + bbox->edgeRotMat[edge][8]*ball->dz;
	
	ball->dx = x;
	ball->dz = z;
}

void transfer_ball(struct ball *ball, int edge) {
	struct tile_vertex *p0, *p1;
	struct tile *tile0, *tile1;
	float tx,ty,tz,theta,cosTheta,sinTheta;
	float x,y,z,x2,y2,z2;
	float u[3],umag;
	
	assert(ball->tile->neighbors[edge].id != 0);
	assert(ball->tile != ball->tile->neighbors[edge].tile);
	
	p0 = &(ball->tile->vertices[edge]);
	p1 = &(ball->tile->vertices[(edge+1) % ball->tile->num_edges]);
	
	u[0] = p1->x - p0->x;
	u[1] = p1->y - p0->y;
	u[2] = p1->z - p0->z;
	umag = sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
	u[0] /= umag;
	u[1] /= umag;
	u[2] /= umag;
	
	tile0 = ball->tile;
	tile1 = ball->tile->neighbors[edge].tile;
	
	tx = tile0->norm_y*tile1->norm_z - tile0->norm_z*tile1->norm_y;
	ty = tile0->norm_z*tile1->norm_x - tile0->norm_x*tile1->norm_z;
	tz = tile0->norm_x*tile1->norm_y - tile0->norm_y*tile1->norm_x;
	theta = asin(sqrt(tx*tx + ty*ty + tz*tz));
	if(dot(u[0], u[1], u[2], tx, ty, tz) < 0.0f) {
		theta = -theta;
	}
	cosTheta = cos(theta);
	sinTheta = sin(theta);
	
	x = ball->x;
	y = ball_y(ball);
	z = ball->z;
	
	x -= p0->x;
	y -= p0->y;
	z -= p0->z;
	
	/* rotate (x,y,z) theta degrees around (p1-p0), store in (x2,y2,z2) */
	x2 = (cosTheta + u[0]*u[0]*(1-cosTheta)) * x + (u[0]*u[1]*(1-cosTheta) - u[2]*sinTheta) * y + (u[0]*u[2]*(1-cosTheta)+u[1]*sinTheta) * z;
	y2 = (u[1]*u[0]*(1-cosTheta) + u[2]*sinTheta) * x + (cosTheta + u[1]*u[1]*(1-cosTheta)) * y + (u[1]*u[2]*(1-cosTheta) - u[0]*sinTheta) * z;
	z2 = (u[2]*u[0]*(1-cosTheta) - u[1]*sinTheta) * x + (u[2]*u[1]*(1-cosTheta) + u[0]*sinTheta) * y + (cosTheta + u[2]*u[2]*(1-cosTheta)) * z;
	
	x2 += p0->x;
	y2 += p0->y;
	z2 += p0->z;
	
	ball->x = x2;
	ball->z = z2;
	
	/* now rotate the direction by the same amount */
	x = ball->dx;
	y = ball_dy(ball);
	z = ball->dz;
	ball->dx = (cosTheta + u[0]*u[0]*(1-cosTheta)) * x + (u[0]*u[1]*(1-cosTheta) - u[2]*sinTheta) * y + (u[0]*u[2]*(1-cosTheta)+u[1]*sinTheta) * z;
	/*ball->dy = (u[1]*u[0]*(1-cosTheta) + u[2]*sinTheta) * x + (cosTheta + u[1]*u[1]*(1-cosTheta)) * y + (u[1]*u[2]*(1-cosTheta) - u[0]*sinTheta) * z;*/
	ball->dz = (u[2]*u[0]*(1-cosTheta) - u[1]*sinTheta) * x + (u[2]*u[1]*(1-cosTheta) + u[0]*sinTheta) * y + (cosTheta + u[2]*u[2]*(1-cosTheta)) * z;
	
	/* and update the tile */
	ball->tile = tile1;
	ball->tile_id = tile1->id;
}

void apply_gravity_tick(struct ball *ball) {
	float gx,gy,gz;
	float balldy;

	/* gravity (down) is [0,-1,0] */
	/* tile norm is upwards */
	/* tile_norm - gravity = influence of gravity on ball */
	gx = GRAVITY_MAGNITUDE * ball->tile->norm_x;
	gy = GRAVITY_MAGNITUDE * (ball->tile->norm_y - 1);
	gz = GRAVITY_MAGNITUDE * ball->tile->norm_z;

	ball->dx *= ball->speed;
	ball->dz *= ball->speed;
	
	ball->dx += gx;
	ball->dz += gz;

	balldy = ball_dy(ball);
	ball->speed = sqrt(
		ball->dx * ball->dx + 
		balldy * balldy + 
		ball->dz * ball->dz );

	ball->dx /= ball->speed;
	ball->dz /= ball->speed;
}

void physics_test_static_functions() {
	assert( abs(distance_sq(0,0,0,1,1,1) - 3.0f) < 0.0001f );
	assert( abs(distance_sq(0,1,2,3,4,5) - 27.0f) < 0.0001f );
	
	assert( abs(dot(0,0,0,9,18,27) - 0.0f) < 0.0001f );
	assert( abs(dot(0,1,2,3,4,5) - 14.0f) < 0.0001f );
	assert( abs(dot(2,3,4,5,6,7) - 56.0f) < 0.0001f );
}
