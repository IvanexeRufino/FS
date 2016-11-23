#include "Configuracion.h"

int recibirCoordenada(int socketEntrenador)
{
	char* mensajeRecibido = string_new();
	recv(socketEntrenador, (void*) mensajeRecibido,4, 0);
	int coordenada=atoi(mensajeRecibido);

	return coordenada;
}
