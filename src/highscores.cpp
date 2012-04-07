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
		printf("Error deleting highscores file (it may already not exist)\n");
	}
}

static void clear_highscores_glui(int code) {
	clear_highscores();
}

static void close_highscores(int code) {
	gluiHighscores->close();
	gluiHighscores = NULL;
}

static struct highscoretable *new_highscoretable() {
	struct highscoretable *ret;

	ret = (struct highscoretable *)calloc(1, sizeof(struct highscoretable));
	/* num_entries will be 0, entries will be 0/NULL */

	return ret;
}

#define HIGHSCORES_HEADER "MINIGOLF-RWCARTER-V1"

static struct highscoretable *load_highscores() {
	struct highscoretable *ret;
	FILE *file;
	void *buf;
	int i,len;

	file = fopen(HIGHSCORES_FILE, "r");
	if(file == NULL) {
		printf("No existing highscores file found, creating new one\n");
		return new_highscoretable();
	}

	buf = malloc(500);

	fread(buf, strlen(HIGHSCORES_HEADER)+1, 1, file);
	if(strcmp((char*)buf, HIGHSCORES_HEADER) != 0) {
		printf("Invalid highscores file.\n");
		free(buf);
		return NULL;
	}

	ret = (struct highscoretable *)calloc(1, sizeof(struct highscoretable));

	fread(buf, sizeof(int), 1, file);
	ret->num_entries = *((int*)buf);
	ret->entries = (struct highscoreentry *)calloc(1, sizeof(struct highscoreentry));
	for(i=0; i<ret->num_entries; i++) {
		fread(buf, sizeof(int), 1, file);
		len = *((int*)buf);
		if(len > 0) {
			ret->entries[i].player = (char*)malloc(len+1);
			fread(buf, len, 1, file);
			((char*)buf)[len] = '\0';
			strcpy(ret->entries[i].player, (char*)buf);
		}

		fread(buf, sizeof(int), 1, file);
		len = *((int*)buf);
		if(len > 0) {
			ret->entries[i].course = (char*)malloc(len+1);
			fread(buf, len, 1, file);
			((char*)buf)[len] = '\0';
			strcpy(ret->entries[i].course, (char*)buf);
		}

		fread(buf, sizeof(int), 1, file);
		ret->entries[i].score = *((int*)buf);
	}

	fclose(file);

	free(buf);
	return ret;
}

static void save_highscores(struct highscoretable *highscores) {
	FILE *file;
	int i, len;

	file = fopen(HIGHSCORES_FILE, "w");

	if(file == NULL) {
		printf("Error: could not open highscores file for writing\n");
		return;
	}

	fwrite(HIGHSCORES_HEADER, strlen(HIGHSCORES_HEADER)+1, 1, file);

	fwrite(&(highscores->num_entries), sizeof(int), 1, file);
	for(i=0; i<highscores->num_entries; i++) {
		len = strlen(highscores->entries[i].player);
		fwrite(&len, sizeof(int), 1, file);
		if(len > 0) {
			fwrite(highscores->entries[i].player, len, 1, file);
		}

		len = strlen(highscores->entries[i].course);
		fwrite(&len, sizeof(int), 1, file);
		if(len > 0) {
			fwrite(highscores->entries[i].course, len, 1, file);
		}

		fwrite(&(highscores->entries[i].score), sizeof(int), 1, file);
	}

	fclose(file);

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
