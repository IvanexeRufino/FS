#include "Configuracion.h"

int IniciarSocketServidor(int puertoServer)
{
	struct sockaddr_in socketInfo;
	int socketEscucha;
	int optval = 1;

	// Crear un socket
	socketEscucha = socket (AF_INET, SOCK_STREAM, 0);
	if (socketEscucha == -1) return -1;

	setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval,sizeof(optval));
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_port = htons(puertoServer);
	socketInfo.sin_addr.s_addr = INADDR_ANY;
	if (bind (socketEscucha,(struct sockaddr *)&socketInfo,sizeof (socketInfo)) != 0)
		{
		close (socketEscucha);
		return -1;
		}
	//Se avisa al sistema que comience a atender llamadas de clientes
	if (listen (socketEscucha, 10) == -1)
		{
		close (socketEscucha);
		return -1;
		}
			//Se devuelve el descriptor del socket servidor
		return socketEscucha;
}
