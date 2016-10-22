#include "Configuracion.h"

int recibirCoordenada(int socketEntrenador)
{
	int coordenada;
	char* buffer = string_new();
	recv(socketEntrenador, buffer,sizeof(buffer), 0);
	char* payload = string_new();
	payload =string_duplicate(buffer);
	str_cut(payload,0,1);

	coordenada=atoi(payload);
	return coordenada;
}
