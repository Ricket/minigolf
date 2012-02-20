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

struct ball;

bool ball_in_tile(struct ball *);
int get_closest_edge(struct ball *);
void bounce_ball(struct ball *, int);
void transfer_ball(struct ball *, int);


void physics_test_static_functions();

#endif