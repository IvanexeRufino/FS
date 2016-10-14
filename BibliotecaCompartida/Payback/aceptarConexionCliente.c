#include "Configuracion.h"

int AceptarConexionCliente(int socketServer)
{
	socklen_t longitudCliente;//esta variable tiene inicialmente el tamaño de la estructura cliente que se le pase
	struct sockaddr cliente;
	int socketNuevaConexion;//esta variable va a tener la descripcion del nuevo socket que estaria creando
	longitudCliente = sizeof(cliente);
	socketNuevaConexion = accept (socketServer, &cliente, &longitudCliente);//acepto la conexion del cliente
	if (socketNuevaConexion < 0)
		return -1;

	return socketNuevaConexion;

}
