#include "Configuracion.h"

int crearSocketServidor(char *puerto) {
	int BACKLOOG = 5;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(NULL, puerto, &hints, &serverInfo);
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	int yes =1;
	setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOOG);
	return listenningSocket;
}
