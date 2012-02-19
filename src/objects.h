/**
 * objects.h
 * Cup and tee functions; create the OpenGL texture(s) and draw the quads.
 *
 * Richard Carter
 * 2012/02/17
 * CSC 462 Assignment 2: Ball Physics
 */

#ifndef CUPTEE_H
#define CUPTEE_H

struct hole;
struct tile;
struct cup;
struct tee;

void initialize_cuptee(struct hole *);
void draw_cup(struct cup *);
void draw_tee(struct tee *);

struct ball {
	int tile_id;
	float x,y,z;
	float vx,vy,vz;
};

struct ball *make_ball(struct tee *);

/*
reset_ball : pass NULL as the tee to only reset the ball velocity; pass the tee into it to reset
             the ball position as well.
*/
void reset_ball(struct ball *, struct tee *);

void update_ball(struct ball *, struct tile *);

void draw_ball(struct ball *);
void draw_arrow();

#endif