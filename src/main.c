/**
 * main.c
 * Main entry point, setup, rendering, user input.
 *
 * Richard Carter
 * 2012/01/27
 * CSC 462 Assignment 1: Minigolf Rendering
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#define PI 3.14159265f

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "data.h"
#include "cuptee.h"


int main(int argc, char** argv);
static int init();
static void update_logic();
static void reshape(int, int);
static void render();
static void mousemove(int, int);
static void mousedownmove(int, int);
static void keypress(unsigned char, int, int);
static void keypress_special(int, int, int);

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

#define MOUSE_SPEED_X (1.0f / 2.0f)
#define MOUSE_SPEED_Y (-1.0f / 3.0f)


static struct hole *hole;

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	
	if(argc < 2) {
		printf("Syntax: %s [GLUT_args] input_filename\n", argv[0]);
		return 0;
	}
	
	hole = load_hole(argv[1]);
	/* print_hole(hole); */
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/* glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); */
	glutCreateWindow("Minigolf by Richard Carter");
	glutIdleFunc(&update_logic);
	glutDisplayFunc(&render);
	glutReshapeFunc(&reshape);
	glutMotionFunc(&mousedownmove);
	glutPassiveMotionFunc(&mousemove);
	glutKeyboardFunc(&keypress);
	glutSpecialFunc(&keypress_special);
	if (!init()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }
	initialize_cuptee(hole);

    glutMainLoop();
	
	free(hole);
    return 0;
}

static int init() {
	GLfloat lightpos[] = {1.0f, 1.0f, 0.0f, 0.0f};
	GLfloat lightamb[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat lightdif[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat lightspc[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat materialspc[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat materialems[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 5, -7.0f, 0, 0, 0, 0, 1, 0);
	
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	
	/* I found this page to be helpful for lighting instructions and tips:
	   http://www.sjbaker.org/steve/omniv/opengl_lighting.html
	*/
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightspc);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialspc);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialems);
	
    return 1;
}

static void update_logic() {
	
}

static void reshape(int w, int h) {
	glViewport(0,0,w,h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, ((double)w) / ((double)h), 0.1, 1000.0);
	
	glMatrixMode(GL_MODELVIEW);
}

static float cameraRotX = 0.0f, cameraRotY = 0.0f, cameraDist = 7.0f;
static float cameraPosX = 0.0f, cameraPosY = 0.0f, cameraPosZ = 0.0f;
static void setup_camera();
static void render_tile(struct tile *t);

static void render() {
	struct listnode *node;
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setup_camera();

	/* draw each tile */
	node = hole->tiles->first;
	while(node != NULL) {
		render_tile((struct tile *)node->ptr);
		node = node->next;
	}
	
	draw_cup(hole->cup);
	draw_tee(hole->tee);

    glutSwapBuffers();
}

static void setup_camera() {
	float y,z;
	
	/* assume already in GL_MODELVIEW */
	glLoadIdentity();
	z = sin(cameraRotY*PI/180.0f) * cameraDist;
	if(z == 0.0f) z = 0.0001f;
	y = cos(cameraRotY*PI/180.0f) * cameraDist;
	gluLookAt(0, y, -z, 0, 0, 0, 0, 1, 0);
	glRotatef(cameraRotX, 0, 1, 0);
	glTranslatef(cameraPosX, cameraPosY, cameraPosZ);
}

static void render_tile(struct tile *t) {
	int i,j,k;
	float j_minus_i[3];
	float j_minus_i_mag;
	float side_norm[3];
	float side_norm_mag;
	
	/* top */
	glColor3f(0.529f, 0.969f, 0.090f);
	if(t->num_edges == 3) {
		glBegin(GL_TRIANGLES);
	} else if(t->num_edges == 4) {
		glBegin(GL_QUADS);
	} else {
		glBegin(GL_POLYGON);
	}
	glNormal3f(t->norm_x, t->norm_y, t->norm_z);
	for(i = 0; i < t->num_edges; i++) {
		glVertex3f(t->vertices[i].x, t->vertices[i].y, t->vertices[i].z);
	}
	
	/* bottom */
	glColor3f(0.396f, 0.451f, 0.514f);
	if(t->num_edges > 4) {
		glEnd();
		glBegin(GL_POLYGON);
	}
	glNormal3f(0.0f, -1.0f, 0.0f);
	for(i = t->num_edges - 1; i >= 0; i--) {
		glVertex3f(t->vertices[i].x, 0.0f, t->vertices[i].z);
	}
	glEnd();
	
	/* draw the sides and borders */
	glBegin(GL_QUADS);
	for(i = 0; i < t->num_edges; i++) {
		if(t->neighbors[i].id == 0) {
			/* no neighbor that way */
			/* draw the side */
			j = (i+1) % (t->num_edges);
			j_minus_i[0] = t->vertices[j].x - t->vertices[i].x;
			j_minus_i[1] = t->vertices[j].y - t->vertices[i].y;
			j_minus_i[2] = t->vertices[j].z - t->vertices[i].z;
			j_minus_i_mag = sqrt(j_minus_i[0]*j_minus_i[0] + j_minus_i[1]*j_minus_i[1] + j_minus_i[2]*j_minus_i[2]);
			for(k = 0; k < 3; k++) { /* normalize j_minus_i */
				j_minus_i[k] = j_minus_i[k] / j_minus_i_mag;
			}
			
			side_norm[0] = j_minus_i[1] * t->norm_z - j_minus_i[2] * t->norm_y;
			side_norm[1] = j_minus_i[2] * t->norm_x - j_minus_i[0] * t->norm_z;
			side_norm[2] = j_minus_i[0] * t->norm_y - j_minus_i[1] * t->norm_x;
			side_norm_mag = sqrt(side_norm[0]*side_norm[0] + side_norm[1]*side_norm[1] + side_norm[2]*side_norm[2]);
			for(k = 0; k < 3; k++) { /* normalize side_norm */
				side_norm[k] = side_norm[k] / side_norm_mag;
			}
			glNormal3f(side_norm[0], side_norm[1], side_norm[2]); /* normal of both side and border */
			
			glColor3f(0.396f, 0.451f, 0.514f);
			glVertex3f(t->vertices[j].x, t->vertices[j].y, t->vertices[j].z);
			glVertex3f(t->vertices[i].x, t->vertices[i].y, t->vertices[i].z);
			glVertex3f(t->vertices[i].x, 0.0f, t->vertices[i].z);
			glVertex3f(t->vertices[j].x, 0.0f, t->vertices[j].z);
			
			/* draw the border */
			glColor3f(0.647f, 0.164f, 0.164f);
			glVertex3f(t->vertices[i].x, t->vertices[i].y, t->vertices[i].z);
			glVertex3f(t->vertices[j].x, t->vertices[j].y, t->vertices[j].z);
			glVertex3f(t->vertices[j].x, t->vertices[j].y + 0.1f, t->vertices[j].z);
			glVertex3f(t->vertices[i].x, t->vertices[i].y + 0.1f, t->vertices[i].z);
		}
	}
	glEnd();
}

static int lastx = INT_MIN, lasty = INT_MIN;

static void mousemove(int x, int y) {
	lastx = x; lasty = y;
}

static void mousedownmove(int x, int y) {
	if(lastx == INT_MIN && lasty == INT_MIN) {
		lastx = x; lasty = y;
	}
	
	if((x-lastx) != 0) {
		cameraRotX += (x-lastx) * MOUSE_SPEED_X;
		/* glRotatef((float)(x-lastx), 0,1,0); */
	}
	if((y-lasty) != 0) {
		cameraRotY += (y-lasty) * MOUSE_SPEED_Y;
		if(cameraRotY > 180.0f) cameraRotY = 179.99f;
		else if(cameraRotY < 0.0f) cameraRotY = 0.01f;
		/* glRotatef((float)(y-lasty), 1,0,0); */
	}

	lastx = x; lasty = y;
	
	glutPostRedisplay();
}

static void keypress(unsigned char key, int x, int y) {
	/* printf("key: %d\n", (int)key); */
	if(key == 27) { /* escape */
		exit(0);
		return;
	}
	
	if(key == 39) { /* quotes */
		cameraPosY += 1.0f;
	} else if(key == 47) { /* slash questionmark */
		cameraPosY -= 1.0f;
	}
	
	glutPostRedisplay();
}

static void keypress_special(int key, int x, int y) {
	/* printf("skey: %d\n", key); */
	if(key == GLUT_KEY_LEFT) {
		cameraPosX += 1.0f;
	} else if(key == GLUT_KEY_RIGHT) {
		cameraPosX -= 1.0f;
	} else if(key == GLUT_KEY_UP) {
		cameraPosZ += 1.0f;
	} else if(key == GLUT_KEY_DOWN) {
		cameraPosZ -= 1.0f;
	}
	
	glutPostRedisplay();
}

