#include "about.h"

#include <GL/glui.h>

static GLUI *aboutDialog = NULL;

static void closeAbout(int);

void show_about_dialog() {
	if(aboutDialog != NULL) {
		return;
	}

	aboutDialog = GLUI_Master.create_glui("Splash Screen");
	aboutDialog->add_statictext("Minigolf");
	aboutDialog->add_statictext("by Richard Carter");
	aboutDialog->add_statictext("Assignment 4 - External Objects");
	aboutDialog->add_statictext("April 8, 2012");
	aboutDialog->add_button("OK", 0, &closeAbout);
}

static void closeAbout(int code) {
	aboutDialog->close();
	aboutDialog = NULL;
}
