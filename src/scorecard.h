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
