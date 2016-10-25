/*
 * pokedexcliente.h
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#ifndef POKEDEXCLIENTE_SRC_POKEDEXCLIENTE_H_
#define POKEDEXCLIENTE_SRC_POKEDEXCLIENTE_H_

/* -- declaraciones de bibliotecas -- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fuse.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdbool.h>

typedef unsigned char osada_block[64];
typedef uint32_t osada_block_pointer;

/* -- declaraciones de estructuras -- */

/* -- declaraciones de constantes -- */

/* -- configuracion de sockets -- */

#define IP "127.0.0.1"
#define PUERTO "9999"
#define PACKAGESIZE 1024

/* -- configuracion de log -- */

#define LOG_FILE "pokedexcliente.log"
#define PROGRAM_NAME "CLIENTE"
#define PROGRAM_DESCRIPTION "Proceso POKEDEX CLIENTE"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO


#endif /* POKEDEXCLIENTE_SRC_POKEDEXCLIENTE_H_ */
