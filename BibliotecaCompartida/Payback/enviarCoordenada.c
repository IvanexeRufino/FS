#include "Configuracion.h"

int enviarCoordenada(int coordenada,int socket){
	char* buffer = malloc(sizeof(char)*3);
	char* identificador="0";
	char* posicion;
	sprintf(posicion,"%d",coordenada);			//No tocar, warning al dope
	strcpy(buffer,identificador);
	strcat(buffer,posicion);
	send(socket, buffer, sizeof(buffer), 0);
	free(buffer);
	return coordenada;
}
