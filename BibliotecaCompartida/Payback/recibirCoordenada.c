#include "Configuracion.h"

void str_cut2(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return;
}

int recibirCoordenada(int socketEntrenador)
{
	int coordenada;
	char* buffer = string_new();
	recv(socketEntrenador, buffer,sizeof(buffer), 0);
	char* payload = string_new();
	payload =string_duplicate(buffer);
	str_cut2(payload,0,1);

	coordenada=atoi(payload);
	return coordenada;
}
