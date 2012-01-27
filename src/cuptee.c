/**
 * cuptee.c
 * Cup and tee functions; create the OpenGL texture(s) and draw the quads.
 *
 * Richard Carter
 * 2012/01/27
 * CSC 462 Assignment 1: Minigolf Rendering
 */

#include "cuptee.h"
#include "data.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#ifndef GL_BGRA
#  define GL_BGRA GL_BGRA_EXT
#endif

static GLuint textures[1];
#define TEXTURE_CIRCLE 0
#define NUM_TEXTURES 1

#define CIRCLE_TEXTURE_SIZE 64
#define CUP_3D_SIZE 0.1f
#define TEE_3D_SIZE 0.04f

struct quad {
	float pt[4][3];
};
static struct quad cupquad, teequad;

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
		cupquad.pt[0][0] = hole->cup->x + vx[0];
		cupquad.pt[0][1] = hole->cup->y + vx[1];
		cupquad.pt[0][2] = hole->cup->z + vx[2];
		cupquad.pt[1][0] = hole->cup->x - vy[0];
		cupquad.pt[1][1] = hole->cup->y - vy[1];
		cupquad.pt[1][2] = hole->cup->z - vy[2];
		cupquad.pt[2][0] = hole->cup->x - vx[0];
		cupquad.pt[2][1] = hole->cup->y - vx[1];
		cupquad.pt[2][2] = hole->cup->z - vx[2];
		cupquad.pt[3][0] = hole->cup->x + vy[0];
		cupquad.pt[3][1] = hole->cup->y + vy[1];
		cupquad.pt[3][2] = hole->cup->z + vy[2];
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
		teequad.pt[0][0] = hole->tee->x + vx[0];
		teequad.pt[0][1] = hole->tee->y + vx[1];
		teequad.pt[0][2] = hole->tee->z + vx[2];
		teequad.pt[1][0] = hole->tee->x - vy[0];
		teequad.pt[1][1] = hole->tee->y - vy[1];
		teequad.pt[1][2] = hole->tee->z - vy[2];
		teequad.pt[2][0] = hole->tee->x - vx[0];
		teequad.pt[2][1] = hole->tee->y - vx[1];
		teequad.pt[2][2] = hole->tee->z - vx[2];
		teequad.pt[3][0] = hole->tee->x + vy[0];
		teequad.pt[3][1] = hole->tee->y + vy[1];
		teequad.pt[3][2] = hole->tee->z + vy[2];
	} else {
		printf("Warning: invalid tee tile id");
	}
}

void initialize_textures() {
	int i,j;
	int cupsize_over_2 = (CIRCLE_TEXTURE_SIZE/2);
	int cupsize_over_2_squared = cupsize_over_2*cupsize_over_2;
	unsigned int *cupImg;

	glGenTextures(NUM_TEXTURES, textures);
	
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CIRCLE]);
	cupImg = (unsigned int *)malloc(CIRCLE_TEXTURE_SIZE*CIRCLE_TEXTURE_SIZE*sizeof(unsigned int));
	for(i = 0; i < CIRCLE_TEXTURE_SIZE; i++) {
		for(j = 0; j < CIRCLE_TEXTURE_SIZE; j++) {
			if(cupsize_over_2_squared < (i-cupsize_over_2)*(i-cupsize_over_2) + (j-cupsize_over_2)*(j-cupsize_over_2)) {
				/* alpha */
				cupImg[j*CIRCLE_TEXTURE_SIZE+i] = 0x00FFFFFF;
			} else {
				/* color */
				cupImg[j*CIRCLE_TEXTURE_SIZE+i] = 0xFFFFFFFF;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CIRCLE_TEXTURE_SIZE, CIRCLE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)cupImg);
	
	/* TODO does cupImg need to be freed? (can it be?) */
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initialize_cuptee(struct hole *hole) {
	/* this method must be called after creating the OpenGL window */
	initialize_quads(hole);
	initialize_textures();
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