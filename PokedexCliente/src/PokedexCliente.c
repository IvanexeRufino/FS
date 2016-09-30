/*
 ============================================================================
 Name        : PokedexCliente.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
//#include "Configuracion.c"
#include "pokedexcliente.h"

#define LOG_FILE "pokedexcliente.log"
#define PROGRAM_NAME "CLIENTE"
#define PROGRAM_DESCRIPTION "Proceso POKEDEX CLIENTE"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO

int main(void) {
	t_log* logger;
	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	log_info(logger, PROGRAM_DESCRIPTION);

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	int buf;
	int i;
	ssize_t nbytes;

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	int enviar = 1;
	char message[PACKAGESIZE];
	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");
	log_info(logger, "Conectado al servidor");

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);			// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
		if (!strcmp(message,"exit\n"))
			{
				enviar = 0;
				log_info(logger, "El usuario ha decidido salir"); // Chequeo que el usuario no quiera salir
			}
		if (enviar)
			{
				send(serverSocket, message, strlen(message) + 1, 0); 	// Solo envio si el usuario no quiere salir.
				log_info(logger, "El mensaje fue enviado");
			}
	}
		close(serverSocket);

	return 0;
}
