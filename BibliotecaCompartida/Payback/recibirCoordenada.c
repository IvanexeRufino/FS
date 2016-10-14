#include "Configuracion.h"
#include "str_cut.h"

void recibirCoordenada(int *coordenada, int socket)  // Le envio como puntero porque hago por referencia
{
	char* buffer = malloc(sizeof(char)*3);
	recv(socket, buffer,sizeof(buffer), 0);

	char* payload = malloc(sizeof(char)*3);
	strcpy(payload, buffer);
	str_cut(payload,0,1);

	(*coordenada)=atoi(payload);
	free(buffer);
	free(payload);
}
