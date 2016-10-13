#include "Configuracion.h"
#include "str_cut.h"

void recibirCoordenada(int *coordenada, int socketEntrenador)
{
	char* buffer = malloc(sizeof(char)*3);
	recv(socketEntrenador, buffer,sizeof(buffer), 0);

	char* payload = malloc(sizeof(char)*3);
	strcpy(payload, buffer);
	str_cut(payload,0,1);

	(*coordenada)=atoi(payload);
	free(buffer);
	free(payload);
}
