/**
 * objects.h
 * Cup and tee functions; create the OpenGL texture(s) and draw the quads.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#ifndef OBJECTS_H
#define OBJECTS_H

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
	float dx,dy,dz,speed;
	
	struct tile *tile;
};

float get_ball_px(struct ball *);
float get_ball_pz(struct ball *);

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