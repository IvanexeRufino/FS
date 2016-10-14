#include "Configuracion.h"

int conectarConServer(char *ipServer, int puertoServer)
{
	struct sockaddr_in socket_info;
	int nuevoSocket;
	// Se carga informacion del socket
	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = inet_addr(ipServer);
	socket_info.sin_port = htons(puertoServer);

	// Crear un socket:
	// AF_INET, SOCK_STREM, 0
	nuevoSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (nuevoSocket < 0)
		return -1;
	// Conectar el socket con la direccion 'socketInfo'.
	int conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
	int mostrarEsperaAconectar=0;
	while (conecto != 0){
		mostrarEsperaAconectar++;
		if (mostrarEsperaAconectar == 1){
			printf("Esperando que el mapa se levante\n");
		}
		conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
	}

	return nuevoSocket;
}
