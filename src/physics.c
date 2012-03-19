/**
 * physics.c
 * Physics functions
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#include "physics.h"

#include <stdio.h>
#include <stdlib.h>
#define bool int
#define true 1
#define false 0
#include <assert.h>
#include <math.h>

#include "data.h"
#include "objects.h"

bool ball_in_tile(struct ball *ball) {
	float ballx, ballz;
	int i, j;
	bool inTile = false;
	
	if(ball->tile == NULL) {
		printf("Warning: ball_in_tile called with tile == NULL, returning false");
		return false;
	}
	
	ballx = get_ball_px(ball);
	ballz = get_ball_pz(ball);
	
	/* point-in-polygon algorithm found at:
	   http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	*/
	for (i = 0, j = ball->tile->num_edges-1; i < ball->tile->num_edges; j = i++) {
		if ( ((ball->tile->proj_vertices[i].z > ballz) != (ball->tile->proj_vertices[j].z > ballz)) &&
				(ballx < (ball->tile->proj_vertices[j].x - ball->tile->proj_vertices[i].x) * (ballz-ball->tile->proj_vertices[i].z) / (ball->tile->proj_vertices[j].z - ball->tile->proj_vertices[i].z) + ball->tile->proj_vertices[i].x) ) {
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

float get_distance_sq_to_edge(struct ball *ball, int edge) {
	struct tile_vertex *p0, *p1;
	
	assert(ball->tile != NULL);
	assert(edge < ball->tile->num_edges);
	
	p0 = &(ball->tile->vertices[edge]);
	p1 = &(ball->tile->vertices[(edge+1) % ball->tile->num_edges]);
	
	return get_shortest_distance_sq(p0, p1, ball->x, get_ball_py(ball), ball->z);
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

void bounce_ball(struct ball *ball, int edge) {
	struct tile_vertex *p0, *p1;
	float x, y, z;
	
	p0 = &(ball->tile->vertices[edge]);
	p1 = &(ball->tile->vertices[(edge+1) % ball->tile->num_edges]);
	
	x = ball->x;
	y = get_ball_py(ball);
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
	x = ball->tile->edgeRotMat[edge][0]*ball->dx + ball->tile->edgeRotMat[edge][1]*get_ball_dy(ball) + ball->tile->edgeRotMat[edge][2]*ball->dz;
	y = ball->tile->edgeRotMat[edge][3]*ball->dx + ball->tile->edgeRotMat[edge][4]*get_ball_dy(ball) + ball->tile->edgeRotMat[edge][5]*ball->dz;
	z = ball->tile->edgeRotMat[edge][6]*ball->dx + ball->tile->edgeRotMat[edge][7]*get_ball_dy(ball) + ball->tile->edgeRotMat[edge][8]*ball->dz;
	
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
	y = get_ball_py(ball);
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
	y = get_ball_dy(ball);
	z = ball->dz;
	ball->dx = (cosTheta + u[0]*u[0]*(1-cosTheta)) * x + (u[0]*u[1]*(1-cosTheta) - u[2]*sinTheta) * y + (u[0]*u[2]*(1-cosTheta)+u[1]*sinTheta) * z;
	ball->dz = (u[2]*u[0]*(1-cosTheta) - u[1]*sinTheta) * x + (u[2]*u[1]*(1-cosTheta) + u[0]*sinTheta) * y + (cosTheta + u[2]*u[2]*(1-cosTheta)) * z;
	
	/* and update the tile */
	ball->tile = tile1;
	ball->tile_id = tile1->id;
}

void physics_test_static_functions() {
	assert( abs(distance_sq(0,0,0,1,1,1) - 3.0f) < 0.0001f );
	assert( abs(distance_sq(0,1,2,3,4,5) - 27.0f) < 0.0001f );
	
	assert( abs(dot(0,0,0,9,18,27) - 0.0f) < 0.0001f );
	assert( abs(dot(0,1,2,3,4,5) - 14.0f) < 0.0001f );
	assert( abs(dot(2,3,4,5,6,7) - 56.0f) < 0.0001f );
}
