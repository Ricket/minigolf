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
	scorecard->playerEnabled = (int*)calloc(num_players, sizeof(int));
	scorecard->num_holes = num_holes;
	scorecard->num_players = num_players;
}

void set_par(struct scorecard *scorecard, int hole, int par) {
	scorecard->pars[hole] = par;
}

void set_playername(struct scorecard *scorecard, int player, char *name) {
	strcpy(scorecard->playerNames[player], name);
}

void set_playerenabled(struct scorecard *scorecard, int player, int enabled) {
	scorecard->playerEnabled[player] = enabled;
}

void add_score(struct scorecard *scorecard, int hole, int player, int score) {
	scorecard->scores[hole * scorecard->num_players + player] = score;
}

GLUI *create_scorecard(struct scorecard *scorecard, GLUI_CB callback) {
	GLUI *ret;
	GLUI_Panel *panel;
	int i,j,startHole,endHole;
	char holeNum[14];

	ret = GLUI_Master.create_glui("Scorecard");
	
	for(startHole=0; startHole < scorecard->num_holes;
			startHole += SCORECARD_ROW_WIDTH) {
		endHole = startHole + SCORECARD_ROW_WIDTH - 1;
		if(endHole >= scorecard->num_holes) {
			endHole = scorecard->num_holes-1;
		}

		sprintf(holeNum, "Holes %d-%d", (startHole+1), (endHole+1));
		panel = ret->add_panel(holeNum);
		panel->set_alignment(GLUI_ALIGN_LEFT);

		ret->add_statictext_to_panel(panel, "Hole");
		ret->add_separator_to_panel(panel);
		for(i=0; i<scorecard->num_players; i++) {
			if(scorecard->playerEnabled[i]) {
				ret->add_statictext_to_panel(panel, scorecard->playerNames[i]);
			}
		}

		for(i=startHole; i<=endHole; i++) {
			ret->add_column_to_panel(panel, true);
			sprintf(holeNum, "%d", (i+1));
			ret->add_statictext_to_panel(panel, holeNum);
			ret->add_separator_to_panel(panel);
			for(j=0; j<scorecard->num_players; j++) {
				if(scorecard->playerEnabled[j]) {
					sprintf(holeNum, "%d", scorecard->scores[i * scorecard->num_players + j]);
					ret->add_statictext_to_panel(panel, holeNum);
				}
			}
		}
	}

	ret->add_button("OK", SCORECARD_OK, callback);

	return ret;
}