/**
 * physics.c
 * Physics functions
 *
 * Richard Carter
 * 2012/02/19
 * CSC 462 Assignment 2: Ball Physics
 */

#include "physics.h"

#include <stdbool.h>

/* taken from:
   http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
*/
bool pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
	int i, j;
	bool c = false;
	for (i = 0, j = nvert-1; i < nvert; j = i++) {
		if ( ((verty[i]>testy) != (verty[j]>testy)) &&
				(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) ) {
			c = !c;
		}
	}
	return c;
}