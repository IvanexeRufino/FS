#include "Configuracion.h"

int enviarCoordenada(int coordenada,int socket){

	char* buffer = string_new();
	string_append(&buffer,string_itoa(0));
	string_append(&buffer,string_itoa(coordenada));
	send(socket, buffer, sizeof(buffer), 0);

	return coordenada;
}
