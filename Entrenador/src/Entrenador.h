/*
 * Entrenador.h
 *
 *  Created on: 5/9/2016
 *      Author: utnso
 */

#ifndef ENTRENADOR_H_
#define ENTRENADOR_H_

/*----------------------- Declaraciones de Bibliotecas ---------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdbool.h>

/*----------------------- Declaraciones de Estructuras ---------------------------------*/

typedef struct
{
	char *nombre;
	char *simbolo;
	char** hojaDeViaje;
	t_list ObjMapa;
	int vidas;
	} entrenador_datos;

/*----------------------- Declaraciones de Constantes ----------------------------------*/
	/* Configuración de SOCKETS */

	#define IP "127.0.0.1"
	#define PUERTO "10000"
	#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

	/* Configuración de LOG */

	#define LOG_FILE "proceso_Entrenador.log"
	#define PROGRAM_NAME "ENTRENADOR"
	#define PROGRAM_DESCRIPTION "Proceso ENTRENADOR"
	#define IS_ACTIVE_CONSOLE true
	#define T_LOG_LEVEL LOG_LEVEL_INFO

#endif /* ENTRENADOR_H_ */
