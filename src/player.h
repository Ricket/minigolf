#ifndef PLAYER_H
#define PLAYER_H

struct ball;

struct player {
	char *name;
	int score;
	int enabled;

	int done;
	struct ball *ball;
};

#endif