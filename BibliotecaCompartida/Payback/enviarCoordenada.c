#include "Configuracion.h"

int enviarCoordenada(int coordenada,int socket){

	char* coordEnviar = string_new();
	coordEnviar = string_itoa(coordenada);
	send(socket, coordEnviar,4, 0);

	return coordenada;
}
