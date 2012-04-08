#include "object.h"
#include "linkedlist.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#ifndef NULL
#  define NULL 0
#endif

void render_object(struct object *obj) {
	int i;
	struct listnode *node;
	struct polygon *poly;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(obj->transformation);

	node = obj->polys->first;
	while(node != NULL) {
		poly = (struct polygon*)node->ptr;

		glColor3f(poly->r, poly->g, poly->b);
		if(poly->num_edges == 3) {
			glBegin(GL_TRIANGLES);
		} else if(poly->num_edges == 4) {
			glBegin(GL_QUADS);
		} else {
			glBegin(GL_POLYGON);
		}
		/* TODO NORMAL */
		for(i = 0; i < poly->num_edges; i++) {
			glVertex3f(poly->x[i], poly->y[i], poly->z[i]);
		}
		glEnd();

		node = node->next;
	}

	glPopMatrix();
}
