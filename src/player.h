/**
 * player.h
 * Definition of player struct.
 *
 * Richard Carter
 * 2012/04/08
 * CSC 462 Assignment 4: External Objects
 */

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
