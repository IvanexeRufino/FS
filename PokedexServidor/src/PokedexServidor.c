/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"
#include <errno.h>

#define PORT "9999"
#define size_header  sizeof(uint16_t) * 2
#define MAX_BUFFERSIZE 1024

int socketServidor;

typedef struct {
	uint16_t codigo;
	uint16_t tamanio;
	char* datos;
  }__attribute__((__packed__)) t_paquete;

  void* memoria(int cantidad)
  {
  	void* puntero = NULL;
  	if (cantidad !=0)
  	{
  		while (puntero == NULL)
  		{
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
  }
  void desacoplador(char* buffer,int sizeBuffer, t_list* paquetes)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
  {
  	int desacoplado = 0;
  	t_paquete* paquete;
  	while (sizeBuffer!= desacoplado)
  	{
  		paquete= memoria(sizeof(t_paquete));
  		paquete->codigo= (uint16_t)*(buffer + desacoplado);
  		paquete->tamanio= (uint16_t)* (buffer + sizeof(uint16_t) + desacoplado);
  		paquete->datos= memoria(paquete->tamanio);
  		memcpy(paquete->datos, buffer + desacoplado + size_header, paquete->tamanio);
  		list_add(paquetes, paquete);
  		desacoplado += (size_header + paquete->tamanio);

  	}
  	return;
  }
int crearSocketServidor(char *puerto) {
	int BACKLOOG = 5;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(NULL, puerto, &hints, &serverInfo);
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	int yes =1;
	setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOOG);
	return listenningSocket;
}

int IniciarSocketServidor(int puertoServer)
{
	struct sockaddr_in socketInfo;
		int socketEscucha;
		int optval = 1;

		// Crear un socket
		socketEscucha = socket (AF_INET, SOCK_STREAM, 0);
		if (socketEscucha == -1)
		 	return -1;

		setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval,
				sizeof(optval));
		socketInfo.sin_family = AF_INET;
		socketInfo.sin_port = htons(puertoServer);
		socketInfo.sin_addr.s_addr = INADDR_ANY;
		if (bind (socketEscucha,(struct sockaddr *)&socketInfo,sizeof (socketInfo)) != 0)
		{
			close (socketEscucha);
			return -1;
		}

		/*
		* Se avisa al sistema que comience a atender llamadas de clientes
		*/
		if (listen (socketEscucha, 10) == -1)
		{
			close (socketEscucha);
			return -1;
		}
		/*
		* Se devuelve el descriptor del socket servidor
		*/
		return socketEscucha;
}

int AceptarConexionCliente(int socketServer) {
	socklen_t longitudCliente;//esta variable tiene inicialmente el tamaño de la estructura cliente que se le pase
	struct sockaddr cliente;
	int socketNuevaConexion;//esta variable va a tener la descripcion del nuevo socket que estaria creando
	longitudCliente = sizeof(cliente);
	socketNuevaConexion = accept (socketServer, &cliente, &longitudCliente);//acepto la conexion del cliente
	if (socketNuevaConexion < 0){
		return -1;
	}
	return socketNuevaConexion;

}

//int getattr(char* bufpath){
//	osada_file* archivo = obtenerArchivo (bufpath);
//	if(archivo == NULL || archivo->state == 0) {
//		return -ENOENT;
//	}
//
//	send(socketServidor,archivo,sizeof(archivo),0);
//	return 0;
//}
//
//int readdir(char* bufpath){
//	int i;
//	int indice = obtenerIndice(bufpath);
//	if(indice == -1) {
//		return -ENOENT;
//	}
//	t_list* listaDeHijos = listaDeHijosDelArchivo(indice);
//	send(socketServidor,listaDeHijos,sizeof(listaDeHijos),0);
//
//	return 0;
//}
//
//int open_callback(char* path) {
//	osada_file* archivo = obtenerArchivo (path);
//	if(archivo == NULL || archivo->state == 0) {
//		return -ENOENT;
//	}
//
//	send(socketServidor,archivo,sizeof(archivo),0);
//	return 0;
//}
//
//int mkdir_callback(char* nombreNuevo) {
//	crear_archivo(nombreNuevo, 2);
//	return 0;
//}

void recibirQueSos(int newfd){
	char buffer[MAX_BUFFERSIZE];
	t_paquete* paquete;
	int sizebytes;
	t_list* paquetes= malloc(sizeof(t_list*));
	if((sizebytes = recv(newfd, &buffer, MAX_BUFFERSIZE - 1,0)) <= 0)
	{
		puts("ERROR RECIBIR");
		//log_error(logDelPersonaje, "Error al recibir paquete del cliente \n");
		exit(1);
	}
	printf("El cliente me envía un paquete \n");
	desacoplador(buffer,sizebytes,paquetes);
	paquete = list_remove(paquetes, 0);

	printf("el codigo es %d \n", paquete->codigo);
	printf("los datos son %s \n",paquete->datos);
	printf("el tamaño es %d \n", paquete->tamanio);

//	switch(paquete->codigo){
//		case 1:
//			getattr(paquete->datos);
//			break;
//		case 2:
//			readdir(paquete->datos);
//			break;
//		case 3:
//			mkdir_callback(paquete->datos);
//			break;
//		case 4:
//			create_callback(paquete->datos);
//			break;
//		case 5:
//			open_callback(paquete->datos);
//			break;
//		case 6:
//			read_callback(paquete->datos);
//			break;
//		case 7:
//			write_callback(paquete->datos);
//			break;
//		case 8:
//			remove_callback(paquete->datos);
//			break;
//		case 9:
//			utimens_callback(paquete->datos);
//			break;
//		case 10:
//			truncate_callback(paquete->datos);
//			break;
//		case 11:
//			rename_callback(paquete->datos);
//			break;
//		case 12:
//			link_callback(paquete->datos);
//			break;
//		}

//	free(buffer);
}

int main(void) {

	int newfd;
	socketServidor = crearSocketServidor("9999");
	IniciarSocketServidor(atoi("9999"));
	puts("conecok");
	reconocerOSADA("/home/utnso/disco.bin");
	while (1) {
		newfd = AceptarConexionCliente(socketServidor);
		recibirQueSos(newfd);
	}
	return 0;
}




