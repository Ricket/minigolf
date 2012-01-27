/**
 * cuptee.h
 * Cup and tee functions; create the OpenGL texture(s) and draw the quads.
 *
 * Richard Carter
 * 2012/01/27
 * CSC 462 Assignment 1: Minigolf Rendering
 */

#ifndef CUPTEE_H
#define CUPTEE_H

struct hole;

struct cup;
struct tee;

void initialize_cuptee(struct hole *);
void draw_cup(struct cup *);
void draw_tee(struct tee *);

#endif