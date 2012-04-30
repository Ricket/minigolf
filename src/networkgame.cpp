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
#  include <WS2tcpip.h>
#  define socketwrite(socket, buf, len) ( send((socket), (buf), (len), 0) )
#  define socketread(socket, buf, len) ( recv((socket), (buf), (len), 0) )
#  define socketclose(socket) ( closesocket(socket) )
#else
#  include <sys/socket.h>
#  include <netinet/in.h> /* struct sockaddr_in */
#  include <netinet/tcp.h> /* TCP_NODELAY */
#  include <netdb.h> /* gethostbyname() */
#  define socketwrite(socket, buf, len) ( write((socket), (buf), (len)) )
#  define socketread(socket, buf, len) ( read((socket), (buf), (len)) )
#  define socketclose(socket) ( close(socket) )
#endif

int network_mode = NM_LOCAL;
int my_player_num = 0;
int sockfd_server = -1;
struct sockaddr_in serv_addr;
int sockfd_clients[3];
char *sock_client_buf[3] = {NULL, NULL, NULL};
int sock_client_buf_pending[3];

int sockfd_client = -1;
char *sock_server_buf = NULL;
int sock_server_buf_pending = 0;

/* stuff from main */
extern std::string newFilename;
extern int newPlayerEnabled[4];
extern std::string newPlayerNames[4];
void start_game_from_new_values();

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

static void text_hostgame(int);
static void button_hostgame(int);

/* join */
static GLUI *joingameDialog = NULL;
static GLUI_EditText *gluiServerHostName = NULL, *gluiServerPort = NULL, *gluiRemoteName = NULL;
static std::string serverHostName, serverPort;
std::string remoteName;

#define GH_SERVER_ADDR 120
#define GH_SERVER_PORT 121
#define GH_REMOTE_NAME 122

static void text_joingame(int);
static void button_joingame(int);

void show_hostgame_dialog() {
	GLUI_Panel *panel;

	if(hostgameDialog != NULL) {
		return;
	}

	hostgameDialog = GLUI_Master.create_glui("Host Game");

	new GLUI_EditText(hostgameDialog, "Input file:", newFilename);

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

	my_player_num = 0;
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
	int i, portnum;
#ifdef _WIN32
	WSADATA wsaData = {0};
	int rsaResult = 0;
#endif

	if(code == GH_CANCEL) {
		hostgameDialog->close();
		hostgameDialog = NULL;

		if(sockfd_server >= 0) {
			socketclose(sockfd_server);
			sockfd_server = -1;
			for(i=0; i<3; i++) {
				sockfd_clients[i] = -1;
				if(sock_client_buf[i] != NULL) {
					free(sock_client_buf[i]);
					sock_client_buf[i] = NULL;
				}
				sock_client_buf_pending[i] = 0;
			}
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

		sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd_server < 0) {
			printf("Error opening socket\n");
#ifdef _WIN32
			WSACleanup();
#endif
			return;
		}
		memset(&serv_addr, '\0', sizeof(struct sockaddr_in));
		portnum = atoi(port.c_str());
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portnum);
		if(bind(sockfd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
			printf("Error binding socket\n");
			/* TODO handle error */
			socketclose(sockfd_server);
			sockfd_server = -1;
			for(i=0; i<3; i++) {
				sockfd_clients[i] = -1;
				if(sock_client_buf[i] != NULL) {
					free(sock_client_buf[i]);
					sock_client_buf[i] = NULL;
				}
				sock_client_buf_pending[i] = 0;
			}
#ifdef _WIN32
			WSACleanup();
#endif
			return;
		}
		listen(sockfd_server, 4);

		for(i=0; i<3; i++) {
			sockfd_clients[i] = -1;
			sock_client_buf[i] = NULL;
			sock_client_buf_pending[i] = 0;

			newPlayerEnabled[i+1] = 0;
		}

		newPlayerEnabled[0] = 1;
		newPlayerNames[0] = std::string(hostName);

		network_mode = NM_SERVER;

	} else if(code == GH_STARTGAME) {
		/* start game */

		/* send start message */

		socket_send_str(sockfd_client, MSG_START, newFilename);
		
		/* trigger actual game start */

		start_game_from_new_values();
	}
}

void show_joingame_dialog() {
	if(joingameDialog != NULL) {
		return;
	}

	joingameDialog = GLUI_Master.create_glui("Join Game");

	serverHostName = std::string("127.0.0.1");
	gluiServerHostName = new GLUI_EditText(joingameDialog, "Host addr:", serverHostName, GH_SERVER_ADDR, &text_joingame);
	
	serverPort = std::string("");
	gluiServerPort = new GLUI_EditText(joingameDialog, "Port:", serverPort, GH_SERVER_PORT, &text_joingame);

	remoteName = std::string("");
	gluiRemoteName = new GLUI_EditText(joingameDialog, "Your name:", remoteName, GH_REMOTE_NAME, &text_joingame);
	
	joingameDialog->add_button("Join", 0, &button_joingame);
}

static void text_joingame(int code) {
}

static void button_joingame(int code) {
	struct addrinfo *serverAddrInfo;
#ifdef _WIN32
	WSADATA wsaData = {0};
	int rsaResult = 0;
#endif

	gluiServerHostName->disable();
	gluiServerPort->disable();
	gluiRemoteName->disable();

#ifdef _WIN32
		if((rsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
			printf("WSAStartup failed: %d\n", rsaResult);
			return;
		}
#endif

	sockfd_client = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd_client < 0) {
		printf("Error opening socket\n");
#ifdef _WIN32
		WSACleanup();
#endif
		return;
	}

	/* get host from name */
	if(getaddrinfo(serverHostName.c_str(), serverPort.c_str(), NULL, &serverAddrInfo) != 0) {
		printf("Error in getaddrinfo\n");
		sockfd_client = -1;
#ifdef _WIN32
		WSACleanup();
#endif
		return;
	}

	memcpy(&serv_addr, serverAddrInfo->ai_addr, sizeof(serv_addr));

	if(connect(sockfd_client, serverAddrInfo->ai_addr, sizeof(serv_addr)) < 0) {
		printf("Error connecting to server\n");
		sockfd_client = -1;
#ifdef _WIN32
		WSACleanup();
#endif
		return;
	}

	/* send name message */
	socket_send_str(sockfd_client, MSG_NAME, remoteName);

	/* allocate buffer, etc. */

	if(sock_server_buf == NULL) {
		sock_server_buf = (char*)malloc(SOCK_CLIENT_BUF_SIZE);
	}
	sock_server_buf_pending = 0;

	network_mode = NM_CLIENT;

	joingameDialog->add_statictext("Waiting for host...");
}

void close_joingame_dialog() {
	if(joingameDialog != NULL) {
		joingameDialog->close();
		joingameDialog = NULL;
	}
}

void socket_send_void(int sock, char type) {
	int i;
	char buffer[512];

	i = 0;
	((unsigned short *)buffer)[0] = htons((unsigned short)sizeof(char));
	i += sizeof(unsigned short);
	buffer[i] = type;
	i += sizeof(char);

	socketwrite(sock, buffer, i);
}

void socket_send_str(int sock, char type, std::string str) {
	int i;
	char buffer[512];

	i = sizeof(unsigned short); /* account for length, added at end */
	buffer[i] = type;
	i += sizeof(char);
	*((unsigned int*)(&(buffer[i]))) = htonl((unsigned int)str.length());
	i += sizeof(unsigned int);
	if(str.length() > 0) {
		memcpy(&buffer[i], str.c_str(), str.length());
		i += str.length();
	}

	((unsigned short *)buffer)[0] = htons(i - sizeof(unsigned short));

	socketwrite(sock, buffer, i);
}

void socket_send_char(int sock, char type, char c) {
	int i;
	char buffer[512];

	i = sizeof(unsigned short); /* account for length, added at end */
	buffer[i] = type;
	i += sizeof(char);
	buffer[i] = c;
	i += sizeof(char);

	((unsigned short *)buffer)[0] = htons(i - sizeof(unsigned short));

	socketwrite(sock, buffer, i);
}

void socket_send_char_str(int sock, char type, char c, std::string str) {
	int i;
	char buffer[512];

	i = sizeof(unsigned short); /* account for length, added at end */
	buffer[i] = type;
	i += sizeof(char);
	buffer[i] = c;
	i += sizeof(char);
	*((unsigned int*)(&(buffer[i]))) = htonl((unsigned int)str.length());
	i += sizeof(unsigned int);
	if(str.length() > 0) {
		memcpy(&buffer[i], str.c_str(), str.length());
		i += str.length();
	}

	((unsigned short *)buffer)[0] = htons(i - sizeof(unsigned short));

	socketwrite(sock, buffer, i);
}

void broadcast_except_char_str(int exceptId, char type, char c, std::string str) {
	int i;
	for(i=0; i<3; i++) {
		if(i == exceptId || sockfd_clients[i] < 0) continue;
		socket_send_char_str(sockfd_clients[i], type, c, str);
	}
}
