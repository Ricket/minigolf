/**
 * hostgame.cpp
 * Network game functions and variables.
 *
 * Richard Carter
 * 2012/04/29
 * CSC 462 Course Project (Minigolf)
 */

#include "networkgame.h"

#include <GL/glui.h>

int network_mode = NM_LOCAL;

static GLUI *hostgameDialog = NULL;
static GLUI *joingameDialog = NULL;

static void close_hostgame(int);
static void close_joingame(int);

void show_hostgame_dialog() {
	if(hostgameDialog != NULL) {
		return;
	}

	hostgameDialog = GLUI_Master.create_glui("Host Game");
	/* add stuff here */
	hostgameDialog->add_button("OK", 0, &close_hostgame);
}

static void close_hostgame(int code) {
	hostgameDialog->close();
	hostgameDialog = NULL;
}

void show_joingame_dialog() {
	if(joingameDialog != NULL) {
		return;
	}

	joingameDialog = GLUI_Master.create_glui("Join Game");
	/* add stuff here */
	joingameDialog->add_button("OK", 0, &close_joingame);
}

static void close_joingame(int code) {
	joingameDialog->close();
	joingameDialog = NULL;
}
