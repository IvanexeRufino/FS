#include "Configuracion.h"

char* convertirStreamAString(char* stream)
{
	int tamanio = strlen(stream);
	char* string= malloc(tamanio);
	int i;

	for(i=0;i<tamanio;i++)
	{
		string[i]='\0';
	}
	strcpy(string,stream);
	return string;
}

