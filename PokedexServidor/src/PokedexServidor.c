/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"
//#include "FileSysOSADA/osada.h"
#include <errno.h>

#define PORT "9999"

int socketServidor;

typedef struct {
	char* header;
	const char *path;
	int size;
}t_package;

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
////		return -ENOENT;
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
////		return -ENOENT;
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
//
//int recibirPaquete(int socket){
//
//	char* buf=malloc(sizeof(t_package*));
//	recv(socket,buf,sizeof(buf),MSG_WAITALL);
//	puts("recibi");
//
//	char bufheader;
//	bufheader=buf[0];
//
//	char* bufpath = malloc(sizeof(char*));
//	bufpath = (char*) buf[1];
//
//	printf("%c \n",bufheader);
//	printf("%s \n",&bufpath);
//
//	switch(bufheader){
//	case '1':
//		getattr(bufpath);
//		break;
//	case '2':
//		readdir(bufpath);
//		break;
//	case '3':
//		open_callback(bufpath);
//		break;
//	case '4':
//		mkdir_callback(bufpath);
//		break;
//	}
//
//	return 0;
//}

void recibirQueSos(int newfd){
	char buforecibido[4096];
	recv(newfd,buforecibido,sizeof(buforecibido),0);
	char headerrecv= buforecibido[0];
	printf("%c",headerrecv);
	puts("recibiok");

}



int main(void) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);
	int newfd;
	socketServidor = crearSocketServidor("9999");
	IniciarSocketServidor(atoi("9999"));
	puts("conecok");
	while (1) {
		newfd = AceptarConexionCliente(socketServidor);
		printf("El cliente nuevo se ha conectado por el socket %d\n", newfd);
		recibirQueSos(newfd);
		//recibirPaquete(newfd);
	}
	return 0;
}


