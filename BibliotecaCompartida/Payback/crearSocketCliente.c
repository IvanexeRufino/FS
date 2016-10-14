#include "Configuracion.h"

int crearSocketCliente(char ip[], int puerto) {
	int socketCliente;
	struct sockaddr_in servaddr;

	if ((socketCliente = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Problem creando el Socket.");
		exit(2);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(puerto);

	if (connect(socketCliente, (struct sockaddr *) &servaddr, sizeof(servaddr))
			< 0) {
		perror("Problema al intentar la conexión con el Servidor");
		exit(3);
	}
	return socketCliente;
}
