/**
 * objects.c
 * Cup and tee functions; create the OpenGL texture(s) and draw the quads.
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#ifdef _WIN32
/* disable warning 'unreferenced formal parameter' */
#  pragma warning( disable : 4100 )
#endif

#include "ballcuptee.h"
#include "data.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#ifndef GL_BGRA
#  define GL_BGRA GL_BGRA_EXT
#endif

#define TEXTURE_CIRCLE 0
#define TEXTURE_ARROW 1
#define NUM_TEXTURES 2
static GLuint textures[NUM_TEXTURES];

#define CIRCLE_TEXTURE_SIZE 64
#define ARROW_TEXTURE_SIZE 64

#define CUP_3D_SIZE 0.1f
#define TEE_3D_SIZE 0.04f
#define ARROW_3D_SIZE 0.3f

#define BALL_INITIAL_SPEED 1.0f

struct quad {
	float pt[4][3];
};
static struct quad cupquad, teequad, ballquad, arrowquad;

void initialize_quads(struct hole *hole) {
	float vx[3], vy[3], mag;
	int i;
	struct listnode *node;
	struct tile *tile;
	
	/* cup quad */
	node = hole->tiles->first;
	while(node != NULL) {
		tile = (struct tile *)node->ptr;
		if(tile->id == hole->cup->tile_id) {
			break;
		}
		node = node->next;
	}
	
	if(node != NULL) { /* tile was found */
		vx[0] = tile->vertices[1].x - tile->vertices[0].x;
		vx[1] = tile->vertices[1].y - tile->vertices[0].y;
		vx[2] = tile->vertices[1].z - tile->vertices[0].z;
		mag = sqrt(vx[0]*vx[0] + vx[1]*vx[1] + vx[2]*vx[2]);
		for(i = 0; i < 3; i++) {
			vx[i] = vx[i] / mag * CUP_3D_SIZE;
		}
		/* vy = vx cross tilenorm */
		vy[0] = vx[1] * tile->norm_z - vx[2] * tile->norm_y;
		vy[1] = vx[2] * tile->norm_x - vx[0] * tile->norm_z;
		vy[2] = vx[0] * tile->norm_y - vx[1] * tile->norm_x;
		mag = sqrt(vy[0]*vy[0] + vy[1]*vy[1] + vy[2]*vy[2]);
		for(i = 0; i < 3; i++) {
			vy[i] = vy[i] / mag * CUP_3D_SIZE;
		}
		
		/* surely there's a better way to do this, but oh well */
		cupquad.pt[0][0] = hole->cup->x + vx[0] + tile->norm_x * 0.001f;
		cupquad.pt[0][1] = hole->cup->y + vx[1] + tile->norm_y * 0.001f;
		cupquad.pt[0][2] = hole->cup->z + vx[2] + tile->norm_z * 0.001f;
		cupquad.pt[1][0] = hole->cup->x - vy[0] + tile->norm_x * 0.001f;
		cupquad.pt[1][1] = hole->cup->y - vy[1] + tile->norm_y * 0.001f;
		cupquad.pt[1][2] = hole->cup->z - vy[2] + tile->norm_z * 0.001f;
		cupquad.pt[2][0] = hole->cup->x - vx[0] + tile->norm_x * 0.001f;
		cupquad.pt[2][1] = hole->cup->y - vx[1] + tile->norm_y * 0.001f;
		cupquad.pt[2][2] = hole->cup->z - vx[2] + tile->norm_z * 0.001f;
		cupquad.pt[3][0] = hole->cup->x + vy[0] + tile->norm_x * 0.001f;
		cupquad.pt[3][1] = hole->cup->y + vy[1] + tile->norm_y * 0.001f;
		cupquad.pt[3][2] = hole->cup->z + vy[2] + tile->norm_z * 0.001f;
	} else {
		printf("Warning: invalid cup tile id");
	}
	
	/* tee quad */
	node = hole->tiles->first;
	while(node != NULL) {
		tile = (struct tile *)node->ptr;
		if(tile->id == hole->tee->tile_id) {
			break;
		}
		node = node->next;
	}
	
	if(node != NULL) { /* tile was found */
		vx[0] = tile->vertices[1].x - tile->vertices[0].x;
		vx[1] = tile->vertices[1].y - tile->vertices[0].y;
		vx[2] = tile->vertices[1].z - tile->vertices[0].z;
		mag = sqrt(vx[0]*vx[0] + vx[1]*vx[1] + vx[2]*vx[2]);
		for(i = 0; i < 3; i++) {
			vx[i] = vx[i] / mag * TEE_3D_SIZE;
		}
		/* vy = vx cross tilenorm */
		vy[0] = vx[1] * tile->norm_z - vx[2] * tile->norm_y;
		vy[1] = vx[2] * tile->norm_x - vx[0] * tile->norm_z;
		vy[2] = vx[0] * tile->norm_y - vx[1] * tile->norm_x;
		mag = sqrt(vy[0]*vy[0] + vy[1]*vy[1] + vy[2]*vy[2]);
		for(i = 0; i < 3; i++) {
			vy[i] = vy[i] / mag * TEE_3D_SIZE;
		}
		
		/* surely there's a better way to do this, but oh well */
		teequad.pt[0][0] = hole->tee->x + vx[0] + tile->norm_x * 0.001f;
		teequad.pt[0][1] = hole->tee->y + vx[1] + tile->norm_y * 0.001f;
		teequad.pt[0][2] = hole->tee->z + vx[2] + tile->norm_z * 0.001f;
		teequad.pt[1][0] = hole->tee->x - vy[0] + tile->norm_x * 0.001f;
		teequad.pt[1][1] = hole->tee->y - vy[1] + tile->norm_y * 0.001f;
		teequad.pt[1][2] = hole->tee->z - vy[2] + tile->norm_z * 0.001f;
		teequad.pt[2][0] = hole->tee->x - vx[0] + tile->norm_x * 0.001f;
		teequad.pt[2][1] = hole->tee->y - vx[1] + tile->norm_y * 0.001f;
		teequad.pt[2][2] = hole->tee->z - vx[2] + tile->norm_z * 0.001f;
		teequad.pt[3][0] = hole->tee->x + vy[0] + tile->norm_x * 0.001f;
		teequad.pt[3][1] = hole->tee->y + vy[1] + tile->norm_y * 0.001f;
		teequad.pt[3][2] = hole->tee->z + vy[2] + tile->norm_z * 0.001f;
	} else {
		printf("Warning: invalid tee tile id");
	}
}

void initialize_object_textures() {
	int i,j;
	int cupsize_over_2 = (CIRCLE_TEXTURE_SIZE/2);
	int cupsize_over_2_squared = cupsize_over_2*cupsize_over_2;
	unsigned int *imgData;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(NUM_TEXTURES, textures);
	
	/* TEXTURE_CIRCLE */
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CIRCLE]);
	imgData = (unsigned int *)malloc(CIRCLE_TEXTURE_SIZE*CIRCLE_TEXTURE_SIZE*sizeof(unsigned int));
	for(i = 0; i < CIRCLE_TEXTURE_SIZE; i++) {
		for(j = 0; j < CIRCLE_TEXTURE_SIZE; j++) {
			if(cupsize_over_2_squared < (i-cupsize_over_2)*(i-cupsize_over_2) + (j-cupsize_over_2)*(j-cupsize_over_2)) {
				/* alpha */
				imgData[j*CIRCLE_TEXTURE_SIZE+i] = 0x00FFFFFF;
			} else {
				/* color */
				imgData[j*CIRCLE_TEXTURE_SIZE+i] = 0xFFFFFFFF;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CIRCLE_TEXTURE_SIZE, CIRCLE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)imgData);
	free(imgData);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	/* TEXTURE_ARROW */
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ARROW]);
	imgData = (unsigned int *)malloc(ARROW_TEXTURE_SIZE*ARROW_TEXTURE_SIZE*sizeof(unsigned int));
	for(i = 0; i < ARROW_TEXTURE_SIZE; i++) {
		for(j = 0; j < ARROW_TEXTURE_SIZE; j++) {
			if(i < ARROW_TEXTURE_SIZE/4 ||
			   i >= 3*ARROW_TEXTURE_SIZE/4 ||
			   (i >= ARROW_TEXTURE_SIZE/4 && i < 3*ARROW_TEXTURE_SIZE/8 && j >= ARROW_TEXTURE_SIZE/4) ||
			   (i >= 5*ARROW_TEXTURE_SIZE/8 && i < 3*ARROW_TEXTURE_SIZE/4 && j >= ARROW_TEXTURE_SIZE/4) ||
			   (i >= ARROW_TEXTURE_SIZE/4 && i-ARROW_TEXTURE_SIZE/4+j < ARROW_TEXTURE_SIZE/4) ||
			   (i < 3*ARROW_TEXTURE_SIZE/4 && i-ARROW_TEXTURE_SIZE/2-j >= 0)
			) {
				/* alpha */
				imgData[j*ARROW_TEXTURE_SIZE+i] = 0x00FFFFFF;
			} else {
				/* color */
				imgData[j*ARROW_TEXTURE_SIZE+i] = 0xFFFFFFFF;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ARROW_TEXTURE_SIZE, ARROW_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)imgData);
	free(imgData);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initialize_cuptee(struct hole *hole) {
	/* this method must be called after creating the OpenGL window */
	initialize_quads(hole);
}

void draw_cup(struct cup *c) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glColor3f(0.0f, 0.0f, 0.0f); /* color the cup black */
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CIRCLE]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(cupquad.pt[0][0], cupquad.pt[0][1], cupquad.pt[0][2]);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(cupquad.pt[1][0], cupquad.pt[1][1], cupquad.pt[1][2]);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(cupquad.pt[2][0], cupquad.pt[2][1], cupquad.pt[2][2]);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(cupquad.pt[3][0], cupquad.pt[3][1], cupquad.pt[3][2]);
	glEnd();
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void draw_tee(struct tee *t) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glColor3f(1.0f, 1.0f, 1.0f); /* color the tee white */
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CIRCLE]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(teequad.pt[0][0], teequad.pt[0][1], teequad.pt[0][2]);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(teequad.pt[1][0], teequad.pt[1][1], teequad.pt[1][2]);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(teequad.pt[2][0], teequad.pt[2][1], teequad.pt[2][2]);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(teequad.pt[3][0], teequad.pt[3][1], teequad.pt[3][2]);
	glEnd();
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

float ball_y(struct ball *ball) {
	struct tile *t;
	struct tile_vertex *p0;

	/* (ball.pos - tile.p0) dot tile.norm = 0 */
	t = ball->tile;
	p0 = &(t->vertices[0]);

	return ( p0->y * t->norm_y
		- (ball->x - p0->x) * t->norm_x
		- (ball->z - p0->z) * t->norm_z ) / t->norm_y;
}

float ball_dy(struct ball *ball) {
	struct tile *t;
	struct tile_vertex *p0;

	/* ball.dir dot tile.norm = 0 */
	t = ball->tile;
	p0 = &(t->vertices[0]);

	return ( ball->dx * t->norm_x + ball->dz * t->norm_z ) / (-t->norm_y);
}

void ball_debug(struct ball *ball) {
	float balldy, mag;

	balldy = ball_dy(ball);
	mag = ball->dx * ball->dx + balldy*balldy + ball->dz * ball->dz;
	printf("ball dmag: %f\n", mag);
}

struct ball *make_ball(struct tee *tee) {
	struct ball *ret = (struct ball *) calloc(1, sizeof(struct ball));
	reset_ball(ret, tee);
	ret->dx = 1.0f;
	ret->dz = 0.0f;
	return ret;
}

void reset_ball(struct ball *ball, struct tee *tee) {
	if(tee != NULL) {
		ball->tile_id = tee->tile_id;
		ball->tile = tee->tile;
		ball->x = tee->x;
		ball->z = tee->z;
	}
	
	ball->speed = BALL_INITIAL_SPEED;
}

void update_ball(struct ball *ball, struct tile *tile) {
	float bally, balldy;
	float vx[3], vy[3], mag;
	int i;
	
	vx[0] = tile->vertices[1].x - tile->vertices[0].x;
	vx[1] = tile->vertices[1].y - tile->vertices[0].y;
	vx[2] = tile->vertices[1].z - tile->vertices[0].z;
	mag = sqrt(vx[0]*vx[0] + vx[1]*vx[1] + vx[2]*vx[2]);
	for(i = 0; i < 3; i++) {
		vx[i] = vx[i] / mag * TEE_3D_SIZE;
	}
	/* vy = vx cross tilenorm */
	vy[0] = vx[1] * tile->norm_z - vx[2] * tile->norm_y;
	vy[1] = vx[2] * tile->norm_x - vx[0] * tile->norm_z;
	vy[2] = vx[0] * tile->norm_y - vx[1] * tile->norm_x;
	mag = sqrt(vy[0]*vy[0] + vy[1]*vy[1] + vy[2]*vy[2]);
	for(i = 0; i < 3; i++) {
		vy[i] = vy[i] / mag * TEE_3D_SIZE;
	}
	
	bally = ball_y(ball);
	balldy = ball_dy(ball);

	/* surely there's a better way to do this, but oh well */
	ballquad.pt[0][0] = ball->x - vx[0] - vy[0] + tile->norm_x * 0.001f;
	ballquad.pt[0][1] = bally   - vx[1] - vy[1] + tile->norm_y * 0.001f;
	ballquad.pt[0][2] = ball->z - vx[2] - vy[2] + tile->norm_z * 0.001f;
	ballquad.pt[1][0] = ball->x - vx[0] + vy[0] + tile->norm_x * 0.001f;
	ballquad.pt[1][1] = bally   - vx[1] + vy[1] + tile->norm_y * 0.001f;
	ballquad.pt[1][2] = ball->z - vx[2] + vy[2] + tile->norm_z * 0.001f;
	ballquad.pt[2][0] = ball->x + vx[0] + vy[0] + tile->norm_x * 0.001f;
	ballquad.pt[2][1] = bally   + vx[1] + vy[1] + tile->norm_y * 0.001f;
	ballquad.pt[2][2] = ball->z + vx[2] + vy[2] + tile->norm_z * 0.001f;
	ballquad.pt[3][0] = ball->x + vx[0] - vy[0] + tile->norm_x * 0.001f;
	ballquad.pt[3][1] = bally   + vx[1] - vy[1] + tile->norm_y * 0.001f;
	ballquad.pt[3][2] = ball->z + vx[2] - vy[2] + tile->norm_z * 0.001f;
	
	/* TODO create arrowquad based on the arrow's rotation and the vectors */
	
	vy[0] = ball->dx * ball->speed;
	vy[1] = balldy   * ball->speed;
	vy[2] = ball->dz * ball->speed;
	
	for(i = 0; i < 3; i++) {
		vy[i] = vy[i] * ARROW_3D_SIZE;
	}
	
	
	/* vx = vy cross tilenorm */
	vx[0] = - vy[1] * tile->norm_z + vy[2] * tile->norm_y;
	vx[1] = - vy[2] * tile->norm_x + vy[0] * tile->norm_z;
	vx[2] = - vy[0] * tile->norm_y + vy[1] * tile->norm_x;
	mag = sqrt(vx[0]*vx[0] + vx[1]*vx[1] + vx[2]*vx[2]);
	for(i = 0; i < 3; i++) {
		vx[i] = vx[i] / mag * ARROW_3D_SIZE;
	}
	
	/* surely there's a better way to do this, but oh well */
	arrowquad.pt[0][0] = ball->x - vx[0] + 0.1f * vy[0] + tile->norm_x * 0.001f;
	arrowquad.pt[0][1] = bally   - vx[1] + 0.1f * vy[1] + tile->norm_y * 0.001f;
	arrowquad.pt[0][2] = ball->z - vx[2] + 0.1f * vy[2] + tile->norm_z * 0.001f;
	arrowquad.pt[1][0] = ball->x - vx[0] + 1.1f * vy[0] + tile->norm_x * 0.001f;
	arrowquad.pt[1][1] = bally   - vx[1] + 1.1f * vy[1] + tile->norm_y * 0.001f;
	arrowquad.pt[1][2] = ball->z - vx[2] + 1.1f * vy[2] + tile->norm_z * 0.001f;
	arrowquad.pt[2][0] = ball->x + vx[0] + 1.1f * vy[0] + tile->norm_x * 0.001f;
	arrowquad.pt[2][1] = bally   + vx[1] + 1.1f * vy[1] + tile->norm_y * 0.001f;
	arrowquad.pt[2][2] = ball->z + vx[2] + 1.1f * vy[2] + tile->norm_z * 0.001f;
	arrowquad.pt[3][0] = ball->x + vx[0] + 0.1f * vy[0] + tile->norm_x * 0.001f;
	arrowquad.pt[3][1] = bally   + vx[1] + 0.1f * vy[1] + tile->norm_y * 0.001f;
	arrowquad.pt[3][2] = ball->z + vx[2] + 0.1f * vy[2] + tile->norm_z * 0.001f;
}

void draw_ball(struct ball *ball) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glColor3f(0.5f, 0.5f, 0.5f); /* color the ball gray */
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CIRCLE]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(ballquad.pt[0][0], ballquad.pt[0][1], ballquad.pt[0][2]);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(ballquad.pt[1][0], ballquad.pt[1][1], ballquad.pt[1][2]);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(ballquad.pt[2][0], ballquad.pt[2][1], ballquad.pt[2][2]);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(ballquad.pt[3][0], ballquad.pt[3][1], ballquad.pt[3][2]);
	glEnd();
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void draw_arrow() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glColor3f(0.8f, 0.0f, 0.0f); /* color the arrow red */
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ARROW]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(arrowquad.pt[0][0], arrowquad.pt[0][1], arrowquad.pt[0][2]);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(arrowquad.pt[1][0], arrowquad.pt[1][1], arrowquad.pt[1][2]);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(arrowquad.pt[2][0], arrowquad.pt[2][1], arrowquad.pt[2][2]);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(arrowquad.pt[3][0], arrowquad.pt[3][1], arrowquad.pt[3][2]);
	glEnd();
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
