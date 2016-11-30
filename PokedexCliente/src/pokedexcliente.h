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
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <commons/string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "FileSysOSADA/osada.h"

/* -- declaraciones de estructuras -- */

typedef unsigned char osada_block[64];
typedef uint32_t osada_block_pointer;

/* -- declaraciones de constantes -- */

#define MAX_BUFFERSIZE 4096
#define size_header  sizeof(uint16_t) * 2

/* -- configuracion de sockets -- */

#define IP "127.0.0.1"
#define PUERTO "9999"
#define PACKAGESIZE 1024

/* -- configuracion de log -- */

#define LOG_FILE "pokedexcliente.log"
#define PROGRAM_NAME "Char* Mander"
#define PROGRAM_DESCRIPTION "Proceso POKEDEX CLIENTE"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO


#endif /* POKEDEXCLIENTE_SRC_POKEDEXCLIENTE_H_ */
