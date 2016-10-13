/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"

#define PORT "9999"


int main(void) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);
	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000];

	//crear socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	   if (socket_desc == -1) {
	       printf("No se pudo crear el socket");
	   }
	   puts("Socket creado");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//bindear
	if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
	        //print the error message
	        perror("Fallo el bind");
	        return 1;
	}
	    puts("Bind okay");

	//escuchar
	listen(socket_desc , 3);

	//aceptar conexiones
	puts("Esperando conexiones...");
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0){
	    perror("Fallo la aceptacion");
	    return 1;
	}
	puts("Conexion aceptada");

	//recibir mensaje de cliente
	while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ){
		//responderle al cliente
	    write(client_sock ,client_message, strlen(client_message));
	    }

	    if(read_size == 0)
	    {
	        puts("Cliente desconectado");
	        fflush(stdout);
	    }
	    else if(read_size == -1)
	    {
	        perror("Fallo la recepcion");
	    }

	    return 0;
}

