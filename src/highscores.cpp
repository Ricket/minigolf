#include "highscores.h"

#include <stdio.h>
#include <GL/glui.h>

#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

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

static void close_highscores(int code) {
	gluiHighscores->close();
	gluiHighscores = NULL;
}

static void clear_highscores_glui(int code) {
	clear_highscores();
	close_highscores(code);
}

void add_highscore(struct highscoretable *table, char *player, char *course, int score) {
	struct highscoreentry *entry;
	int i, j, insertionIdx, len;

	/* TODO eliminate the code duplication in this function */

	if(table->entries == NULL) {
		table->entries = (struct highscoreentry *)calloc(1, sizeof(struct highscoreentry));
		len = strlen(player);
		if(len > 0) {
			table->entries[0].player = (char*)calloc(1, len+1);
			strcpy(table->entries[0].player, player);
		}

		len = strlen(course);
		if(len > 0) {
			table->entries[0].course = (char*)calloc(1, len+1);
			strcpy(table->entries[0].course, course);
		}

		table->entries[0].score = score;

		table->num_entries = 1;
	} else {
		insertionIdx = -1;
		/* entry holds the new array of entries */
		entry = (struct highscoreentry *)calloc(min(10, table->num_entries+1), sizeof(struct highscoreentry));

		/* loop through the existing entries to find the spot for the new one */
		for(i=0,j=0; i<table->num_entries && j < 10; i++,j++) {
			if(insertionIdx == -1) { /* insertion point not yet found */
				if(table->entries[i].score > score) {
					/* We found a score greater than our inserting score! Now set the
					   insertionIdx to this one. Then decrement i; next loop round,
					   j will be one ahead of i, which is what we want. i is the index
					   into the old array, and j is the index into the new array; now that
					   j is one ahead, it will leave space for the insertionIdx. */
					insertionIdx = i;
					i--;
					continue;
				}
			}

			memcpy(&(entry[j]), &(table->entries[i]), sizeof(struct highscoreentry));
		}


		if(insertionIdx == -1) {
			insertionIdx = table->num_entries;
			if(insertionIdx >= 10) {
				free(entry);
				return; /* score is too low; nothing to add */
			}
		}

		free(table->entries);
		table->entries = entry;
		entry = &(table->entries[insertionIdx]);

		len = strlen(player);
		if(len > 0) {
			entry->player = (char*)calloc(1, len+1);
			strcpy(entry->player, player);
		}

		len = strlen(course);
		if(len > 0) {
			entry->course = (char*)calloc(1, len+1);
			strcpy(entry->course, course);
		}

		entry->score = score;

		if(table->num_entries < 10) {
			table->num_entries++;
		}
	}
}

static struct highscoretable *new_highscoretable() {
	struct highscoretable *ret;

	ret = (struct highscoretable *)calloc(1, sizeof(struct highscoretable));
	/* num_entries will be 0, entries will be 0/NULL */

	return ret;
}

#define HIGHSCORES_HEADER "MINIGOLF-RWCARTER-V1"

struct highscoretable *load_highscores() {
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
	ret->entries = (struct highscoreentry *)calloc(ret->num_entries, sizeof(struct highscoreentry));
	for(i=0; i<ret->num_entries; i++) {
		fread(buf, sizeof(int), 1, file);
		len = *((int*)buf);
		if(len > 0) {
			ret->entries[i].player = (char*)calloc(len+1, sizeof(char));
			fread(buf, len, 1, file);
			((char*)buf)[len] = '\0';
			strcpy(ret->entries[i].player, (char*)buf);
		}

		fread(buf, sizeof(int), 1, file);
		len = *((int*)buf);
		if(len > 0) {
			ret->entries[i].course = (char*)calloc(len+1, sizeof(char));
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

void save_highscores(struct highscoretable *highscores) {
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

void free_highscores(struct highscoretable *highscores) {
	int i;

	for(i=0; i<highscores->num_entries; i++) {
		if(highscores->entries[i].player != NULL) {
			free(highscores->entries[i].player);
		}
		if(highscores->entries[i].course != NULL) {
			free(highscores->entries[i].course);
		}
	}
	if(highscores->entries != NULL) {
		free(highscores->entries);
	}
	free(highscores);
}

void show_highscores() {
	GLUI_Panel *panel;
	struct highscoretable *highscores;
	int i;
	char score[20];

	if(gluiHighscores != NULL) {
		gluiHighscores->close();
		gluiHighscores = NULL;
	}

	gluiHighscores = GLUI_Master.create_glui("Highscores");
	panel = gluiHighscores->add_panel("Top 10");

	highscores = load_highscores();
	
	if(highscores->num_entries > 0) {
		for(i=0; i<highscores->num_entries; i++) {
			gluiHighscores->add_statictext_to_panel(panel, highscores->entries[i].player);
		}
		gluiHighscores->add_column_to_panel(panel, true);
		for(i=0; i<highscores->num_entries; i++) {
			gluiHighscores->add_statictext_to_panel(panel, highscores->entries[i].course);
		}
		gluiHighscores->add_column_to_panel(panel, true);
		for(i=0; i<highscores->num_entries; i++) {
			if(highscores->entries[i].score == 0) {
				gluiHighscores->add_statictext_to_panel(panel, "E");
			} else {
				sprintf(score, "%+d", highscores->entries[i].score);
				gluiHighscores->add_statictext_to_panel(panel, score);
			}
		}
	} else {
		gluiHighscores->add_statictext_to_panel(panel, "No highscores");
	}

	free_highscores(highscores);

	gluiHighscores->add_button("Clear", 0, &clear_highscores_glui);
	gluiHighscores->add_button("OK", 1, &close_highscores);

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
