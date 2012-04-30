/**
 * hostgame.cpp
 * Network game functions and variables.
 *
 * Richard Carter
 * 2012/04/29
 * CSC 462 Course Project (Minigolf)
 */

#include "networkgame.h"

#include <GL/glui.h>

/* socket stuff */
#ifdef _WIN32
#  pragma comment(lib, "Ws2_32.lib")
#  include <winsock2.h>
#  define socketwrite(socket, buf, len) ( send((socket), (buf), (len), 0) )
#  define socketread(socket, buf, len) ( recv((socket), (buf), (len), 0) )
#  define socketclose(socket) ( closesocket(socket) )
#else
#  include <sys/socket.h>
#  include <netinet/in.h> /* struct sockaddr_in */
#  include <netdb.h> /* gethostbyname() */
#  define socketwrite(socket, buf, len) ( write((socket), (buf), (len)) )
#  define socketread(socket, buf, len) ( read((socket), (buf), (len)) )
#  define socketclose(socket) ( close(socket) )
#endif

int network_mode = NM_LOCAL;
int sockfd = -1;

/* host */
static GLUI *hostgameDialog = NULL;
static GLUI_EditText *gluiHostName = NULL, *gluiPort = NULL;
static std::string hostName, port;
static GLUI_Checkbox *hostPlayerCheckboxes[4];
static GLUI_Button *hostBtnStartListening = NULL, *hostBtnStartGame = NULL, *hostBtnCancel = NULL;

#define GH_PORT 20
#define GH_HOSTNAME 21
#define GH_STARTLISTENING 10
#define GH_STARTGAME 11
#define GH_CANCEL 12

/* join */
static GLUI *joingameDialog = NULL;

static void text_hostgame(int);
static void button_hostgame(int);
static void close_joingame(int);

void show_hostgame_dialog() {
	GLUI_Panel *panel;

	if(hostgameDialog != NULL) {
		return;
	}

	hostgameDialog = GLUI_Master.create_glui("Host Game");

	port = std::string("");
	gluiPort = new GLUI_EditText(hostgameDialog, "Port:", port, GH_PORT, &text_hostgame);

	panel = hostgameDialog->add_panel("Players");
	hostPlayerCheckboxes[0] = hostgameDialog->add_checkbox_to_panel(panel, "");
	hostPlayerCheckboxes[0]->set_int_val(1);
	hostPlayerCheckboxes[0]->disable();

	hostPlayerCheckboxes[1] = hostgameDialog->add_checkbox_to_panel(panel, "");
	hostPlayerCheckboxes[1]->set_int_val(0);
	hostPlayerCheckboxes[1]->disable();

	hostPlayerCheckboxes[2] = hostgameDialog->add_checkbox_to_panel(panel, "");
	hostPlayerCheckboxes[2]->set_int_val(0);
	hostPlayerCheckboxes[2]->disable();

	hostPlayerCheckboxes[3] = hostgameDialog->add_checkbox_to_panel(panel, "");
	hostPlayerCheckboxes[3]->set_int_val(0);
	hostPlayerCheckboxes[3]->disable();

	hostgameDialog->add_column_to_panel(panel, false);
	/* hostgameDialog->add_edittext_to_panel(panel, "Your name:", GLUI_EDITTEXT_TEXT, NULL, 20, &text_hostgame); */
	hostName = std::string("");
	gluiHostName = new GLUI_EditText(panel, "Your name:", hostName, GH_HOSTNAME, &text_hostgame);
	/* add statictext for each of the 3 remote players */
	

	panel = hostgameDialog->add_panel("Controls");
	hostBtnStartListening = hostgameDialog->add_button_to_panel(panel, "Start listening", GH_STARTLISTENING, &button_hostgame);
	hostBtnStartListening->disable();
	hostgameDialog->add_column_to_panel(panel, false);
	hostBtnStartGame = hostgameDialog->add_button_to_panel(panel, "Start game", GH_STARTGAME, &button_hostgame);
	hostBtnStartGame->disable();
	hostgameDialog->add_column_to_panel(panel, false);
	hostBtnCancel = hostgameDialog->add_button_to_panel(panel, "Cancel/Stop", GH_CANCEL, &button_hostgame);
}

static void text_hostgame(int code) {
	if(code == GH_HOSTNAME) { /* name */
		printf("name change\n");
		if(hostName.length() == 0 || port.length() == 0) {
			hostBtnStartListening->disable();
		} else {
			hostBtnStartListening->enable();
		}
	}
}

static void button_hostgame(int code) {
	struct sockaddr_in serv_addr;
	int portnum;
#ifdef _WIN32
	WSADATA wsaData = {0};
	int rsaResult = 0;
#endif

	if(code == GH_CANCEL) {
		hostgameDialog->close();
		hostgameDialog = NULL;

		if(sockfd >= 0) {
			socketclose(sockfd);
			sockfd = -1;
#ifdef _WIN32
			WSACleanup();
#endif
		}
	} else if(code == GH_STARTLISTENING) {
		/* start listening */
		gluiPort->disable(); /* disallow changing port */
		gluiHostName->disable(); /* disallow changing host name */

#ifdef _WIN32
		if((rsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
			printf("WSAStartup failed: %d\n", rsaResult);
			return;
		}
#endif

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd < 0) {
			printf("Error opening socket\n");
			/* TODO handle error */
			return;
		}
		memset(&serv_addr, '\0', sizeof(struct sockaddr_in));
		portnum = atoi(port.c_str());
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portnum);
		if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
			printf("Error binding socket\n");
			/* TODO handle error */
			socketclose(sockfd);
			sockfd = -1;
#ifdef _WIN32
			WSACleanup();
#endif
			return;
		}
		listen(sockfd, 5);

		network_mode = NM_SERVER;

	} else if(code == GH_STARTGAME) {
		/* start game */
	}
}

void show_joingame_dialog() {
	if(joingameDialog != NULL) {
		return;
	}

	joingameDialog = GLUI_Master.create_glui("Join Game");
	/* add stuff here */
	joingameDialog->add_button("OK", 0, &close_joingame);
}

static void close_joingame(int code) {
	joingameDialog->close();
	joingameDialog = NULL;
}
