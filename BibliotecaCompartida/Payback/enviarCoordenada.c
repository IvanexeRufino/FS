#include "Configuracion.h"

const char *my_itoa_buf(char *buf, size_t len, int num)
{
  static char loc_buf[sizeof(int)]; /* not thread safe */

  if (!buf)
  {
    buf = loc_buf;
    len = sizeof(loc_buf);
  }

  if (snprintf(buf, len, "%d", num) == -1)
    return ""; /* or whatever */

  return buf;
}

const char *my_itoa(int num)
{ return my_itoa_buf(NULL, 0, num); }

int enviarCoordenada(int coordenada,int socket){
	char* buffer = malloc(sizeof(char)*4);	// Seria 2 del int de la coordenada- 1 del id y 1 del /0
	char* identificador="0";
	const char* posicion=my_itoa(coordenada);
	strcpy(buffer,identificador);
	strcat(buffer,posicion);
	send(socket, buffer, sizeof(buffer), 0);
	free(buffer);
	return coordenada;
}
