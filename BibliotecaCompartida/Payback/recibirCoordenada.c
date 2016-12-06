#include "Configuracion.h"

int recibirCoordenada(int socketEntrenador)
{
	int coordenada;
	void *buffer = malloc(sizeof(int));
	recv(socketEntrenador,buffer,sizeof(int),0);
	memcpy(&coordenada,buffer,sizeof(int));
	free(buffer);
	return coordenada;
}
