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

#include <stdbool.h>

bool pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);

#endif