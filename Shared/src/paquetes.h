/*
 * paquetes.h
 *
 *  Created on: 26/9/2016
 *      Author: utnso
 */

#ifndef PAQUETES_H_
#define PAQUETES_H_

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>


#define size_header  sizeof(uint16_t) * 2

typedef struct
{
	uint16_t codOp;
	uint16_t tamanioDatos;
	char* datos;

} __attribute__((__packed__)) t_paquete ;


void* memoria(int cantidad);
char* acoplador(t_paquete* paquete);
void desacoplador(char* buffer,int sizeBuffer, t_list* paquetes);

#endif /* PAQUETES_H_ */
