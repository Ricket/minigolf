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
extern int sockfd_server;
extern struct sockaddr_in serv_addr;
extern int sockfd_clients[3];
extern void *sock_client_buf[3];
extern int sock_client_buf_pending[3];
#define SOCK_CLIENT_BUF_SIZE 1024

extern int sockfd_client;

void show_hostgame_dialog(void);

#endif
