/*
 * pokedexservidor.h
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#ifndef POKEDEXSERVIDOR_SRC_POKEDEXSERVIDOR_H_
#define POKEDEXSERVIDOR_SRC_POKEDEXSERVIDOR_H_

/* -- declaraciones de bibliotecas -- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdbool.h>

/* -- declaraciones de estructuras -- */

/* -- declaraciones de constantes -- */

/* -- configuracion de sockets -- */

#define PORT "9999"

/* -- configuracion de log -- */

#define LOG_FILE "pokedexservidor.log"
#define PROGRAM_NAME "POKEDEX SERVIDOR"
#define PROGRAM_DESCRIPTION "Proceso POKEDEX SERVIDOR"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO

#endif /* POKEDEXSERVIDOR_SRC_POKEDEXSERVIDOR_H_ */
