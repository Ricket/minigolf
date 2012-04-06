#include "scorecard.h"

GLUI *create_scorecard(int par, struct player* players[4], GLUI_CB callback) {
	GLUI *ret;

	ret = GLUI_Master.create_glui("Scorecard");
	ret->add_button("OK", SCORECARD_OK, callback);

	return ret;
}
