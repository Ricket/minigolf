#include "highscores.h"

#include <stdio.h>
#include <GL/glui.h>

static GLUI *gluiHighscores = NULL;

struct highscoreentry {
	char *player;
	char *course;
	int score;
};

struct highscoretable {
	struct highscoreentry *entries;
	int num_entries;
};

void clear_highscores() {
	if(remove(HIGHSCORES_FILE) != 0) {
		printf("Error deleting highscores file\n");
	}
}

static void clear_highscores_glui(int code) {
	clear_highscores();
}

static void close_highscores(int code) {
	gluiHighscores->close();
	gluiHighscores = NULL;
}

void show_highscores() {
	GLUI_Panel *panel;

	if(gluiHighscores != NULL) {
		return;
	}

	gluiHighscores = GLUI_Master.create_glui("Highscores");
	panel = gluiHighscores->add_panel("Top 10");

	gluiHighscores->add_button("Clear", 0, &clear_highscores_glui);
	gluiHighscores->add_button("OK", 0, &close_highscores);

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
