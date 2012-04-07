#include "highscores.h"

#include <GL/glui.h>

static GLUI *gluiHighscores = NULL;

void show_highscores() {
	GLUI_Panel *panel;

	if(gluiHighscores != NULL) {
		return;
	}

	gluiHighscores = GLUI_Master.create_glui("Highscores");
	panel = gluiHighscores->add_panel("Top 10");

	/* first column should have player names */
	/* second column: name of course */
	/* third column: score */
/*
	aboutDialog = GLUI_Master.create_glui("Splash Screen");
	aboutDialog->add_statictext("Minigolf");
	aboutDialog->add_statictext("by Richard Carter");
	aboutDialog->add_statictext("Assignment 4 - External Objects");
	aboutDialog->add_statictext("April 8, 2012");
	aboutDialog->add_button("OK", 0, &closeAbout); */
}
