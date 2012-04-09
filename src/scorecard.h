/**
 * scorecard.h
 * Create, clear and add data to a scorecard, and display it with GLUI.
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

#ifndef SCORECARD_H
#define SCORECARD_H

#include <GL/glui.h>

struct player;

#define SCORECARD_ROW_WIDTH 5
#define SCORECARD_OK 192939

struct scorecard {
	int * scores;
	int * pars;
	char **playerNames;
	int *playerEnabled;

	int num_holes;
	int num_players;
};

void clear_scorecard(struct scorecard *, int, int);
void set_par(struct scorecard *, int, int);
void set_playername(struct scorecard *, int, char *);
void set_playerenabled(struct scorecard *, int, int);
void add_score(struct scorecard *, int, int, int);

GLUI *create_scorecard(struct scorecard *, GLUI_CB);

#endif
