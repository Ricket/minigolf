/**
 * physics.h
 * Physics functions
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#define bool int

#define GRAVITY_MAGNITUDE 0.022f

struct ball;

bool ball_in_tile(struct ball *);
bool ball_in_obj(struct ball *, struct object *);
int get_closest_edge(struct ball *);
int get_closest_edge_obj(struct ball *, struct object *);
int ball_crosses_obj(struct ball *, float, float, float, struct object *);
void bounce_ball(struct ball *, int);
void bounce_ball_bbox(struct ball *, struct boundingbox *, int);
void transfer_ball(struct ball *, int);

void apply_gravity_tick(struct ball *);


void physics_test_static_functions();

#endif