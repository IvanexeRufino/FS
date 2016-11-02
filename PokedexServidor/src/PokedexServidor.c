/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"
#include <errno.h>
#include "FileSysOSADA/osada.h"

#define PORT "9999"
#define size_header  sizeof(uint16_t) * 2
#define MAX_BUFFERSIZE 1024

typedef struct {
	uint16_t codigo;
	uint16_t tamanio;
	char* datos;
}__attribute__((__packed__)) t_paquete;

void* memoria(int cantidad) {
	void* puntero = NULL;
  	if (cantidad !=0) {
  		while (puntero == NULL) {
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
}

t_paquete* empaquetar(uint16_t codigo, void* datos, uint16_t size){
  	t_paquete* paquete= malloc(sizeof(t_paquete));
  	paquete->codigo= codigo;
  	paquete->datos= datos;
  	paquete->tamanio= size;
  	return paquete;
}

char* acoplador(t_paquete* paquete) /*transforma una estructura de tipo t_paquete en un stream*/
{
  	void* paqueteSalida = memoria(size_header + paquete->tamanio);
  	memcpy(paqueteSalida, paquete, size_header);
  	memcpy(paqueteSalida + size_header, paquete->datos, paquete->tamanio);
  	return paqueteSalida;
}

t_paquete* desacoplador(char* buffer,int sizeBuffer)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
  {
  	t_paquete* paquete;
  	paquete= memoria(sizeof(t_paquete));
  	paquete->codigo= (uint16_t)*(buffer);
  	paquete->tamanio= (uint16_t)* (buffer + sizeof(uint16_t));
  	paquete->datos= memoria(paquete->tamanio);
  	memcpy(paquete->datos, buffer + size_header, paquete->tamanio);
  	return paquete;
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
	int sizebytes;
	if((sizebytes = recv(newfd, &buffer, MAX_BUFFERSIZE - 1,0)) <= 0)
	{
		puts("ERROR RECIBIR");
		//log_error(logDelPersonaje, "Error al recibir paquete del cliente \n");
		exit(1);
	}
	printf("El cliente me envía un paquete \n");
	t_paquete* paqueterecv = desacoplador(buffer,sizebytes);

	printf("el codigo es %d \n", paqueterecv->codigo);
	printf("los datos son %s \n",paqueterecv->datos);
	printf("el tamaño es %d \n", paqueterecv->tamanio);
	osada_file* archivo;
	void* enviar;
	t_paquete* paqueteSend = malloc(sizeof(t_paquete));
		switch(paqueterecv->codigo){
		case 1:
			archivo = obtenerArchivo(paqueterecv->datos);
			if(archivo == NULL || archivo->state == 0) {
				paqueteSend = empaquetar(100,"error",6);
				printf("los datos a enviar son %s \n",paqueteSend->datos);
				enviar = acoplador(paqueteSend);
			}
			else {
				paqueteSend = empaquetar(1,archivo,sizeof(osada_file));
				enviar = acoplador(paqueteSend);
			}
			break;
	//		case 2:
	//			readdir(paquete->datos);
	//			break;
			case 3:
				crear_archivo(paqueterecv->datos,2);
				break;
			case 4:
				crear_archivo(paqueterecv->datos,1);
				break;
			case 5:
				archivo = obtenerArchivo(paqueterecv->datos);
				if(archivo == NULL || archivo->state == 0) {
					paqueteSend = empaquetar(100,"error",6);
					printf("los datos a enviar son %s \n",paqueteSend->datos);
					enviar = acoplador(paqueteSend);
				}
				else {
					paqueteSend = empaquetar(1,archivo,sizeof(osada_file));
					enviar = acoplador(paqueteSend);
				}
				break;
	//		case 6:
	//			read_callback(paquete->datos);
	//			break;
	//		case 7:
	//			write_callback(paquete->datos);
	//			break;
			case 8:
				archivo = obtenerArchivo(paqueterecv->datos);
				if(archivo->state == 1) {
					borrar_archivo(paqueterecv->datos);
				}
				else if(archivo->state == 2) {
					borrar_directorio_vacio(paqueterecv->datos);
				}
				if(archivo == NULL || archivo->state == 0) {
					paqueteSend = empaquetar(100,"error",6);
					enviar = acoplador(paqueteSend);
				}
				break;
			case 9:
				break;
	//		case 10:
	//			truncate_callback(paquete->datos);
	//			break;
	//		case 11:
	//			rename_callback(paquete->datos);
	//			break;
	//		case 12:
	//			link_callback(paquete->datos);
	//			break;
			}

		if(send(newfd,enviar,paqueteSend->tamanio + size_header ,0)<0) {
					puts("ERROR ENVIO");
					exit(1);
		}
}

int main(void) {

	int newfd;
	int socketServidor = crearSocketServidor("9999");
	IniciarSocketServidor(atoi("9999"));
	reconocerOSADA("/home/utnso/disco.bin");
	puts("conecok");
	while (1) {
		newfd = AceptarConexionCliente(socketServidor);
		recibirQueSos(newfd);
	}
	return 0;
}




