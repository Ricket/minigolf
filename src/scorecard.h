#ifndef SCORECARD_H
#define SCORECARD_H

#include <GL/glui.h>

struct player;

#define SCORECARD_OK 192939

GLUI *create_scorecard(int, struct player**, GLUI_CB);

#endif
