#include "Configuracion.h"

int enviarCoordenada(int coordenada,int socketEntrenador){
	char* buffer = malloc(sizeof(char)*3);
	char* identificador="0";
	char* posicion;
	sprintf(posicion,"%d",coordenada);			//No tocar, warning al dope
	strcpy(buffer,identificador);
	strcat(buffer,posicion);
	send(socketEntrenador, buffer, sizeof(buffer), 0);
	free(buffer);
	return coordenada;
}
