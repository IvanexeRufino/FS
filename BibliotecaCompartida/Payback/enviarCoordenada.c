#include "Configuracion.h"

int enviarCoordenada(int coordenada,int socket)
{
	void *buffer = malloc(sizeof(int));
	memcpy(buffer,&coordenada,sizeof(int));
	send(socket,buffer,sizeof(int),0);
	free(buffer);
	return coordenada;
}
