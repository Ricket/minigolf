#include "scorecard.h"

#include <stdio.h>

void clear_scorecard(struct scorecard *scorecard, int num_players, int num_holes) {
	int i;

	if(scorecard->scores != NULL) {
		free(scorecard->scores);
	}
	if(scorecard->pars != NULL) {
		free(scorecard->pars);
	}
	if(scorecard->playerNames != NULL) {
		for(i=0; i<4; i++) {
			free(scorecard->playerNames[i]);
		}
		free(scorecard->playerNames);
	}

	scorecard->scores = (int *)calloc(num_players * num_holes, sizeof(int));
	scorecard->pars = (int *)calloc(num_holes, sizeof(int));
	scorecard->playerNames = (char**)calloc(num_players, sizeof(char*));
	for(i=0; i<4; i++) {
		scorecard->playerNames[i] = (char*)calloc(1, sizeof(GLUI_String));
	}
	scorecard->num_holes = num_holes;
	scorecard->num_players = num_players;
}

void set_par(struct scorecard *scorecard, int hole, int par) {
	scorecard->pars[hole] = par;
}

void set_playername(struct scorecard *scorecard, int player, char *name) {
	strcpy(scorecard->playerNames[player], name);
	printf("set_playername %d, %s\n", player, name);
}

void add_score(struct scorecard *scorecard, int hole, int player, int score) {
	scorecard->scores[hole * scorecard->num_players + player] = score;
	printf("add_score %d, %d, %d\n", hole, player, score);
}

GLUI *create_scorecard(struct scorecard *scorecard, GLUI_CB callback) {
	GLUI *ret;

	ret = GLUI_Master.create_glui("Scorecard");
	ret->add_button("OK", SCORECARD_OK, callback);

	return ret;
}
