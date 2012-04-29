/**
 * networkgame.h
 * Network game functions and variables.
 *
 * Richard Carter
 * 2012/04/29
 * CSC 462 Course Project (Minigolf)
 */

#ifndef NETWORKGAME_H
#define NETWORKGAME_H

extern int network_mode;
#define NM_LOCAL 0
#define NM_SERVER 1
#define NM_CLIENT 2

void show_hostgame_dialog(void);

#endif
