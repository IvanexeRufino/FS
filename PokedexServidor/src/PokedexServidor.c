/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"
#include "FileSysOSADA/osada.h"
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

void getattr(char bufpath){
	puts("getattr");
}

int readdir(char* bufpath){
	int i;
	int indice = obtenerIndice(bufpath);
	if(indice == -1) {
		return -ENOENT;
	}
	t_list* listaDeHijos = listaDeHijosDelArchivo(indice);
	char* nombresDeHijos = malloc(list_size(listaDeHijos)*sizeof(listaDeHijos));
	for(i = 0; i < list_size(listaDeHijos); i++) {
		nombresDeHijos[i] = list_get(listaDeHijos,i);
	}
	char* buff = malloc(sizeof(listaDeHijos));
	memcpy(buff,nombresDeHijos,sizeof(nombresDeHijos));
	enviarPaquete("2",buff);
}

int enviarPaquete (char* head, char* buff){
	t_package *package= malloc(sizeof(package));
	package->header=head;
	package->path=buff;

	char* buf= malloc(sizeof(package));
	strcpy(buf,package->header);
	strcat(buf,package->path);

	send(socketServidor,buff,sizeof(buff),0);
	puts("enviado");
	return 0;
}

int recibirPaquete(int socket){

	char* buf=malloc(sizeof(char)*2);
	recv(socket,buf,sizeof(buf),0);
	puts("recibi");

	char bufheader;
	bufheader=buf[0];
	printf("%c",bufheader);
	puts("oka");

	char bufpath;
	bufpath=buf[1];
	printf("%c",bufpath);
	puts("oka2");

	enviarPaquete(buf,socket);
	puts("enviado");

	switch(bufheader){
	case '1':
		getattr(bufpath);
		break;
	case '2':
		readdir(bufpath);
		break;
	}


	return 0;
}


int main(void) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);
	int newfd;
//	system("truncate -s 100k disco.bin");
//	system("./osada-format /home/ivan/tp-2016-2c-so-II-the-payback/PokedexServidor/disco.bin");
	socketServidor = crearSocketServidor("9999");
	IniciarSocketServidor(atoi("9999"));
	puts("conecok");
	newfd = AceptarConexionCliente(socketServidor);
	printf("El cliente nuevo se ha conectado por el socket %d\n", newfd);
	recibirPaquete(newfd);
	return 0;
}

