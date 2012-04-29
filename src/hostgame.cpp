/**
 * hostgame.cpp
 * Host game dialog creation functions.
 *
 * Richard Carter
 * 2012/04/29
 * CSC 462 Course Project (Minigolf)
 */

#include "hostgame.h"

#include <GL/glui.h>

static GLUI *hostgameDialog = NULL;

static void close_hostgame(int);

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
