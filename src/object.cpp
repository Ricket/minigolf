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

void apply_static_transformation(struct object *obj) {
	int i;
	float x,y,z,w;
	struct listnode *node;
	struct polygon *poly;
	float *T;

	/* apply the transformation matrix to the obj's
	   polys and bounding box, then set it to
	   identity */
	T = obj->transformation;

	node = obj->polys->first;
	while(node != NULL) {
		poly = (struct polygon *)node->ptr;
		for(i = 0; i < poly->num_edges; i++) {
			x = T[0] * poly->x[i] + T[4] * poly->y[i] + T[8] * poly->z[i] + T[12];
			y = T[1] * poly->x[i] + T[5] * poly->y[i] + T[9] * poly->z[i] + T[13];
			z = T[2] * poly->x[i] + T[6] * poly->y[i] + T[10] * poly->z[i] + T[14];
			w = T[3] * poly->x[i] + T[7] * poly->y[i] + T[11] * poly->z[i] + T[15];

			x /= w;
			y /= w;
			z /= w;

			poly->x[i] = x;
			poly->y[i] = y;
			poly->z[i] = z;
		}

		node = node->next;
	}

	if(obj->bbox != NULL) {
		for(i = 0; i < obj->bbox->num_points; i++) {
			x = T[0] * obj->bbox->x[i] + T[4] * obj->bbox->y[i] + T[8] * obj->bbox->z[i] + T[12];
			y = T[1] * obj->bbox->x[i] + T[5] * obj->bbox->y[i] + T[9] * obj->bbox->z[i] + T[13];
			z = T[2] * obj->bbox->x[i] + T[6] * obj->bbox->y[i] + T[10] * obj->bbox->z[i] + T[14];
			w = T[3] * obj->bbox->x[i] + T[7] * obj->bbox->y[i] + T[11] * obj->bbox->z[i] + T[15];

			x /= w;
			y /= w;
			z /= w;

			obj->bbox->x[i] = x;
			obj->bbox->y[i] = y;
			obj->bbox->z[i] = z;
		}
	}

	for(i = 0; i < 16; i++) {
		T[i] = 0;
	}
	T[0] = T[5] = T[10] = T[15] = 1;
}

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
