/**
 * main.c
 * Main entry point, setup, rendering, user input.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#ifdef _WIN32
/* disable warning 'unreferenced formal parameter' */
#  pragma warning( disable : 4100 )
#endif

#include <GL/glui.h>

#include <stdio.h>
#include <stdlib.h>
#define bool int
#define true 1
#define false 0
#include <limits.h>
#include <math.h>
#define PI 3.14159265f

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "data.h"
#include "objects.h"
#include "physics.h"
#include "player.h"
#include "scorecard.h"
#include "about.h"

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


int main(int argc, char** argv);
static int init(void);
static void reload_course(void);
static void update_logic(void);
static void reshape(int, int);
static void render(void);
static void myGlutBitmapString(void*, const char*);
static void push2D();
static void pop2D();
static void mouseclick(int, int, int, int);
static void mousemove(int, int);
static void rightmousedownmove(int, int);
static void keypress(unsigned char, int, int);
static void keypress_special(int, int, int);
static void gluiQuick(int);

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

#define MOUSE_SPEED_X (1.0f / 2.0f)
#define MOUSE_SPEED_Y (-1.0f / 3.0f)

#define ARROW_LENGTH_SPEED 0.03f
#define MIN_ARROW_LENGTH 0.1f
#define MAX_ARROW_LENGTH 2.0f

#define FRICTION 0.01f
#define GRAVITY_MAGNITUDE 0.05f

#define CUP_VICINITY 0.10f
#define CUP_FALLIN 100.0f

enum gamestate {
	GAMESTATE_BALLDIRECTION,
	GAMESTATE_BALLVELOCITY,
	GAMESTATE_BALLMOVING
};

static int windowId;
static GLUI *glui, *gluiNewGame;
static char *newFilename;
static int newPlayerEnabled[4];
static char *newPlayerNames[4];

static char *filename;
static struct player *players[4];
static int currentPlayer;
static GLUI_StaticText *gluiCurrentPlayer;
static GLUI_StaticText *gluiPar;
static struct scorecard *scorecard;
static GLUI *gluiScorecard;
static char parText[9];

#define GLUI_NEW_GAME 12345
#define GLUI_NEW_GAME_OK 3332
#define GLUI_NEW_GAME_CANCEL 3333
#define GLUI_QUIT 17734
#define GLUI_ABOUT 17735
#define GLUI_INPUTFILE 234

static struct course *course;
static struct hole *hole;
static int hole_num;
static struct listnode *hole_node;
static float timeOnHole;
static enum gamestate gameState = GAMESTATE_BALLDIRECTION;

static float cameraDist = 7.0f;
static float cameraRotMatrix[16];
static float cameraPosX = 0.0f, cameraPosY = 0.0f, cameraPosZ = 0.0f;
static void setup_camera(void);
static void render_tile(struct tile *t);

int main(int argc, char** argv) {
	int i,j;

	glutInit(&argc, argv);
	
	if(argc < 2) {
		printf("Optional arguments: %s [GLUT_args] [input_filename]\n", argv[0]);
	}

	gluiNewGame = NULL;
	gluiScorecard = NULL;

    scorecard = (struct scorecard *)calloc(1, sizeof(struct scorecard));

	for(i=0; i<4; i++) {
		players[i] = (struct player *)calloc(1, sizeof(struct player));
		players[i]->name = (char*)calloc(1, sizeof(GLUI_String));

		newPlayerNames[i] = NULL;
	}
	players[0]->enabled = 1;
	strcpy(players[0]->name, "Player 1");

	filename = (char*)calloc(1, sizeof(GLUI_String));
	if(argc >= 2) {
		strncpy(filename, argv[1], max(sizeof(GLUI_String), strlen(argv[1])));
	}
	newFilename = NULL;
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/* glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); */
	windowId = glutCreateWindow("Minigolf by Richard Carter");
	GLUI_Master.set_glutIdleFunc(&update_logic);
	glui = GLUI_Master.create_glui_subwindow(windowId, GLUI_SUBWINDOW_RIGHT);

	/* new game and quit buttons */
	glui->add_button("New game...", GLUI_NEW_GAME, &gluiQuick);
	glui->add_button("Quit", GLUI_QUIT, &gluiQuick);
	glui->add_button("About...", GLUI_ABOUT, &gluiQuick);
	glui->add_separator();

	/* camera rotation control */
	for(i=0; i<4; i++) {
    	for(j=0; j<4; j++) {
    		if(i==j) {
    			cameraRotMatrix[i*4+j] = 1.0f;
    		} else {
    			cameraRotMatrix[i*4+j] = 0.0f;
    		}
    	}
    }
    glui->add_rotation("Rotation", cameraRotMatrix);
    glui->add_separator();

    glui->add_statictext("Current player:");
    gluiCurrentPlayer = glui->add_statictext(players[0]->name);
    glui->add_separator();

    parText[0] = '\0';
    gluiPar = glui->add_statictext(parText);
	
	glui->set_main_gfx_window(windowId);
	glutDisplayFunc(&render);
	GLUI_Master.set_glutReshapeFunc(&reshape);
	GLUI_Master.set_glutMouseFunc(&mouseclick);
	glutPassiveMotionFunc(&mousemove);
	GLUI_Master.set_glutKeyboardFunc(&keypress);
	GLUI_Master.set_glutSpecialFunc(&keypress_special);
	if (!init()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }

    initialize_object_textures();
    reload_course();

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

	glEnable(GL_CULL_FACE);
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

static void reset_hole() {
	int i;

	timeOnHole = 0;
	for(i=0; i<4; i++) {
		players[i]->ball = make_ball(hole->tee);
	}
	initialize_cuptee(hole);
	gameState = GAMESTATE_BALLDIRECTION;
	currentPlayer = 0;
	for(i=0; i<4; i++) {
		players[i]->done = 0;
		players[i]->score = 0;
	}
	gluiCurrentPlayer->set_text(players[0]->name);

	if(hole->par > 0 && hole->par <= 999) {
		sprintf(parText, "Par %d", hole->par);
	} else if(hole->par <= 0) {
		strcpy(parText, "Par ???");
	} else {
		strcpy(parText, "Par lots");
	}
	parText[8] = '\0';
	gluiPar->set_text(parText);

}

static void reload_course() {
	int i;
	struct listnode *node;

	if(course != NULL) {
		free_course(course);
		course = NULL;
	}

	if(strlen(filename) > 0) {
		course = load_course(filename);
		/* print_hole(hole); */
		if(course != NULL) {
			clear_scorecard(scorecard, 4, course->num_holes);
			for(i=0; i<4; i++) {
				set_playername(scorecard, i, players[i]->name);
				set_playerenabled(scorecard, i, players[i]->enabled);
			}
			
			i=0;
			node = course->holes->first;
			while(node != NULL) {
				set_par(scorecard, i, ((struct hole*)node->ptr)->par);
				i++;
				node = node->next;
			}

			hole_node = (*(course->holes)).first;
			hole = (struct hole *) hole_node->ptr;
			hole_num = 0;

			reset_hole();
		}
	}

	if(course == NULL) {
		gluiCurrentPlayer->set_text("none");
		parText[0] = '\0';
		gluiPar->set_text(parText);
	}
}

static int lastUpdate = 0;
static int pendingDelta = 0;
static float arrowLengthV = ARROW_LENGTH_SPEED;

static int all_players_done() {
	int i;
	for(i=0; i<4; i++) {
		if(players[i]->enabled && !players[i]->done) {
			return 0;
		}
	}
	return 1;
}

static void next_player() {
	do {
		currentPlayer = (currentPlayer+1) % 4;
	} while(!players[currentPlayer]->enabled || players[currentPlayer]->done);
	gluiCurrentPlayer->set_text(players[currentPlayer]->name);
}

static void show_scorecard() {
	gluiScorecard = create_scorecard(scorecard, &gluiQuick);
}

static void next_hole() {
	if(hole_node->next != NULL) {
		hole_node = hole_node->next;
		hole = (struct hole *)hole_node->ptr;
		hole_num++;
		reset_hole();
	} else {
		hole = NULL;
		/*
		ball->speed = 0.0f;
		ball->x = hole->cup->x;
		ball->y = hole->cup->y;
		ball->z = hole->cup->z;
		*/
	}
}

static void update_logic() {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	bool shouldRender = false;
	float newv[3];
	int closestEdge;
	float dist, dotprod, mag;
	float dx, dy, dz;

	struct ball *ball;

	pendingDelta += currentTime - lastUpdate;
	
	lastUpdate = currentTime;
	if(pendingDelta > 16) {
		shouldRender = true;
	}
	
	while(pendingDelta > 16) {
		/* TICK */
		if(course != NULL && hole != NULL) {
			ball = players[currentPlayer]->ball;
			if(gameState == GAMESTATE_BALLDIRECTION) {
				/* rotate the ball's velocity slightly around the tile norm */
				newv[0] = ball->tile->rotMat[0][0]*ball->dx + ball->tile->rotMat[1][0]*ball->dy + ball->tile->rotMat[2][0]*ball->dz;
				newv[1] = ball->tile->rotMat[0][1]*ball->dx + ball->tile->rotMat[1][1]*ball->dy + ball->tile->rotMat[2][1]*ball->dz;
				newv[2] = ball->tile->rotMat[0][2]*ball->dx + ball->tile->rotMat[1][2]*ball->dy + ball->tile->rotMat[2][2]*ball->dz;
				ball->dx = newv[0];
				ball->dy = newv[1];
				ball->dz = newv[2];
			}
			else if(gameState == GAMESTATE_BALLVELOCITY) {
				/* increase/loop the ball's velocity slightly */
				ball->speed += arrowLengthV;
				if(ball->speed > MAX_ARROW_LENGTH) {
					ball->speed = MAX_ARROW_LENGTH - (ball->speed - MAX_ARROW_LENGTH);
					arrowLengthV = -arrowLengthV;
				} else if(ball->speed < MIN_ARROW_LENGTH) {
					ball->speed = MIN_ARROW_LENGTH - (ball->speed - MIN_ARROW_LENGTH);
					arrowLengthV = -arrowLengthV;
				}
			}
			else if(gameState == GAMESTATE_BALLMOVING) {
				/* apply gravity to ball->speed */
				/*
				ball->speed -= GRAVITY_MAGNITUDE * sqrt(ball->tile->norm_z*ball->tile->norm_z + ball->tile->norm_x*ball->tile->norm_x);
				if(ball->speed < 0.0f) {
					ball->speed = -ball->speed;
					ball->dx = -ball->dx;
					ball->dy = -ball->dy;
					ball->dz = -ball->dz;
				}
				*/
				
				/* move the ball, account for friction */
				ball->x += ball->dx * ball->speed * 0.02f;
				ball->y += ball->dy * ball->speed * 0.02f;
				ball->z += ball->dz * ball->speed * 0.02f;
				
				ball->speed = max(ball->speed - FRICTION, 0.0f);

				/* if ball exits current tile, bounce or switch tiles */
				while(!ball_in_tile(ball)) {
					/* reflect or switch tiles */
					closestEdge = get_closest_edge(ball);
					
					if(ball->tile->neighbors[closestEdge].id == 0) {
						/* printf("*bounce*\n"); */
						bounce_ball(ball, closestEdge);
					} else {
						/* printf("*cross into new tile*\n"); */
						transfer_ball(ball, closestEdge);
					}
				}
				
				/* check if ball is sinking into cup */
				if(ball->tile == hole->cup->tile) {
					/* distance squared */
					dist = (ball->x - hole->cup->x)*(ball->x - hole->cup->x) + (ball->y - hole->cup->y)*(ball->y - hole->cup->y) + (ball->z - hole->cup->z)*(ball->z - hole->cup->z);
					if(dist <= CUP_VICINITY * CUP_VICINITY) {
						if(ball->speed - CUP_FALLIN * (CUP_VICINITY*CUP_VICINITY - dist) < 0) {
							/* Winner! */
							players[currentPlayer]->done = 1;
							add_score(scorecard, hole_num, currentPlayer, players[currentPlayer]->score);
							gameState = GAMESTATE_BALLDIRECTION;
							
							if(all_players_done()) {
								show_scorecard();
							} else {
								next_player();
							}
						} else {
							dotprod = (ball->dx * (hole->cup->x - ball->x)) + (ball->dy * (hole->cup->y - ball->y)) + (ball->dz * (hole->cup->z - ball->z));
							if(dotprod <= 0.0f) {
								/* ball is heading away from cup */
								/*printf("Heading away at speed %f\n", ball->speed);
								printf("CUP_FALLIN value = %f\n", CUP_FALLIN * (CUP_VICINITY*CUP_VICINITY - dist)); */
								/* set its direction directly away from cup */
								dx = -(hole->cup->x - ball->x);
								dy = -(hole->cup->y - ball->y);
								dz = -(hole->cup->z - ball->z);
								mag = sqrt(dx * dx + dy * dy + dz * dz);
								if(mag > 0.002f) {
									ball->dx = dx/mag;
									ball->dy = dy/mag;
									ball->dz = dz/mag;
								}
							}
						}
					}
				}
				
				/* when ball comes to a stop, go back to direction selection */
				if(ball->speed == 0.0f) {
					reset_ball(ball, NULL);
					gameState = GAMESTATE_BALLDIRECTION;
					next_player();

					/* printf("ball closest to edge %d\n", get_closest_edge(ball)); */
				}
			}
		}
		/* /TICK */
		
		timeOnHole += 16;
		pendingDelta -= 16;
	}
	
	if(shouldRender) {
		glutSetWindow(windowId);
		glutPostRedisplay();
	}
}

static void reshape(int w, int h) {
	int gluix, gluiy, gluiw, gluih;

	GLUI_Master.get_viewport_area(&gluix, &gluiy, &gluiw, &gluih);
	glViewport(gluix, gluiy, gluiw, gluih);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, ((double)gluiw) / ((double)gluih), 0.1, 1000.0);
	
	glMatrixMode(GL_MODELVIEW);
}

static void render() {
	struct listnode *node;
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(course == NULL) {
		// Before new game
		push2D();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glRasterPos2f(-1.0f, 0.9f);
		myGlutBitmapString(GLUT_BITMAP_HELVETICA_18, "Click new game ->");
		pop2D();
	} else if(hole == NULL) {
		// End of game
		push2D();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glRasterPos2f(-1.0f, 0.0f);
		myGlutBitmapString(GLUT_BITMAP_HELVETICA_18, "Game over!");
		pop2D();
	} else if(gluiScorecard != NULL) {
		/* blank screen; wait for user to close scorecard */
	} else {
		// During game
		
		setup_camera();

		/* draw each tile */
		node = hole->tiles->first;
		while(node != NULL) {
			render_tile((struct tile *)node->ptr);
			node = node->next;
		}
		
		draw_cup(hole->cup);
		draw_tee(hole->tee);
		
		/* find ball tile and draw ball */
		node = hole->tiles->first;
		while(node != NULL) {
			if(((struct tile *)node->ptr)->id == players[currentPlayer]->ball->tile_id) {
				break;
			}
			node = node->next;
		}
		if(node != NULL) {
			update_ball(players[currentPlayer]->ball, (struct tile *)node->ptr);
		} else {
			printf("Warning: ball tile not found\n");
		}
		
		draw_ball(players[currentPlayer]->ball);
		
		if(gameState == GAMESTATE_BALLDIRECTION || gameState == GAMESTATE_BALLVELOCITY) {
			draw_arrow();
		}

		push2D();
		if(timeOnHole < 3000) {
			if(timeOnHole < 2000) {
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			} else {
				/* fade out smoothly over 1 second */
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f - ((timeOnHole-2000.0f) / 1000.0f));
			}
			glRasterPos2i(0, 0);
			myGlutBitmapString(GLUT_BITMAP_HELVETICA_18, hole->name);
		}
		pop2D();
	}	

    glutSwapBuffers();
}

static void myGlutBitmapString(void *font, const char *str) {
	int i;
	for(i=0; str[i] != '\0'; i++) {
		glutBitmapCharacter(font, str[i]);
	}
}

static void push2D() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
}

static void pop2D() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
}

static void setup_camera() {
	/*float y,z;*/
	
	/* assume already in GL_MODELVIEW */
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -cameraDist);
	glMultMatrixf(cameraRotMatrix);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	/*
	z = sin(cameraRotY*PI/180.0f) * cameraDist;
	if(z == 0.0f) z = 0.0001f;
	y = cos(cameraRotY*PI/180.0f) * cameraDist;
	gluLookAt(0, y, -z, 0, 0, 0, 0, 1, 0);
	glRotatef(cameraRotX, 0, 1, 0);
	*/
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
			
			/* draw the backside of the border so that it is shown from all angles */
			glVertex3f(t->vertices[i].x, t->vertices[i].y, t->vertices[i].z);
			glVertex3f(t->vertices[i].x, t->vertices[i].y + 0.1f, t->vertices[i].z);
			glVertex3f(t->vertices[j].x, t->vertices[j].y + 0.1f, t->vertices[j].z);
			glVertex3f(t->vertices[j].x, t->vertices[j].y, t->vertices[j].z);
		}
	}
	glEnd();
}

static void mouseclick(int button, int state, int x, int y) {
	if(state == GLUT_DOWN) {
		if(button == GLUT_LEFT_BUTTON) {
			glutMotionFunc(NULL);
			if(gameState != GAMESTATE_BALLMOVING) {
				gameState = static_cast<gamestate>((gameState + 1) % 3);

				if(gameState == GAMESTATE_BALLMOVING) {
					players[currentPlayer]->score++;
				}
			}
		} else if(button == GLUT_RIGHT_BUTTON) {
			glutMotionFunc(&rightmousedownmove);
		}
	}
}

static int lastx = INT_MIN, lasty = INT_MIN;

static void mousemove(int x, int y) {
	lastx = x; lasty = y;
}

static void rightmousedownmove(int x, int y) {
	if(lastx == INT_MIN && lasty == INT_MIN) {
		lastx = x; lasty = y;
	}
	
	if((x-lastx) != 0) {
		/* cameraRotX += (x-lastx) * MOUSE_SPEED_X; */
		/* glRotatef((float)(x-lastx), 0,1,0); */
	}
	if((y-lasty) != 0) {
		/* cameraRotY += (y-lasty) * MOUSE_SPEED_Y;
		if(cameraRotY > 70.0f) cameraRotY = 69.99f;
		else if(cameraRotY < 10.0f) cameraRotY = 10.01f; */
		/* glRotatef((float)(y-lasty), 1,0,0); */
	}

	lastx = x; lasty = y;
	
	glutSetWindow(windowId);
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
	
	glutSetWindow(windowId);
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
	
	glutSetWindow(windowId);
	glutPostRedisplay();
}

static void gluiQuick(int code) {
	int i;

	if(gluiScorecard != NULL && code == GLUI_NEW_GAME) {
		/* do not allow the user to click the new game button if
		   they are viewing the scorecard */
		return;
	}

	if(code == GLUI_NEW_GAME && gluiNewGame == NULL) {
		/* create the window */
		gluiNewGame = GLUI_Master.create_glui("New Game");

		/* copy current values into the live variables */
		newFilename = (char*)calloc(1, sizeof(GLUI_String));
		strcpy(newFilename, filename);
		gluiNewGame->add_edittext("Input file:", GLUI_EDITTEXT_TEXT, newFilename);

		for(i=0; i<4; i++) {
			newPlayerEnabled[i] = players[i]->enabled;
			newPlayerNames[i] = (char*)calloc(1, sizeof(GLUI_String));
			strncpy(newPlayerNames[i], players[i]->name, sizeof(GLUI_String));
		}

		/* add the controls to the new game window */
		gluiNewGame->add_checkbox("Player 1", &newPlayerEnabled[0])->disable();
		gluiNewGame->add_edittext("Player 1 name", GLUI_EDITTEXT_TEXT, newPlayerNames[0]);
		gluiNewGame->add_checkbox("Player 2", &newPlayerEnabled[1]);
		gluiNewGame->add_edittext("Player 2 name", GLUI_EDITTEXT_TEXT, newPlayerNames[1]);
		gluiNewGame->add_checkbox("Player 3", &newPlayerEnabled[2]);
		gluiNewGame->add_edittext("Player 3 name", GLUI_EDITTEXT_TEXT, newPlayerNames[2]);
		gluiNewGame->add_checkbox("Player 4", &newPlayerEnabled[3]);
		gluiNewGame->add_edittext("Player 4 name", GLUI_EDITTEXT_TEXT, newPlayerNames[3]);

		/* OK and cancel buttons */
		gluiNewGame->add_button("OK", GLUI_NEW_GAME_OK, &gluiQuick);
		gluiNewGame->add_button("Cancel", GLUI_NEW_GAME_CANCEL, &gluiQuick);

	} else if(code == GLUI_NEW_GAME_OK) {
		/* user clicked OK; copy new values into vals */
		if(strlen(newFilename) > 0) {
			strcpy(filename, newFilename);
		}

		for(i=0; i<4; i++) {
			strcpy(players[i]->name, newPlayerNames[i]);
			players[i]->enabled = newPlayerEnabled[i];
			set_playername(scorecard, i, players[i]->name);
			set_playerenabled(scorecard, i, players[i]->enabled);
		}

		reload_course();

	} else if(code == SCORECARD_OK) {
		gluiScorecard->close();
		gluiScorecard = NULL;

		next_hole();
	} else if(code == GLUI_QUIT) {
		exit(0);
	} else if(code == GLUI_ABOUT) {
		show_about_dialog();
	}

	/* if user hit OK or Cancel, free memory */
	if(code == GLUI_NEW_GAME_CANCEL || code == GLUI_NEW_GAME_OK) {
		gluiNewGame->close();
		gluiNewGame = NULL;

		free(newFilename); newFilename = NULL;
		for(i=0; i<4; i++) {
			free(newPlayerNames[i]); newPlayerNames[i] = NULL;
		}
	}
}
