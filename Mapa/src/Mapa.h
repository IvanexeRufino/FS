/*
 * Mapa.h
 *
 *  Created on: 5/9/2016
 *      Author: utnso
 */

#ifndef MAPA_H_
#define MAPA_H_

/*----------------------- Declaraciones de Bibliotecas ---------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <tad_items.h>
#include <nivel.h>
#include <curses.h>
#include <pthread.h>

/*----------------------- Declaraciones de Estructuras ---------------------------------*/

typedef struct
{
	char *identificador;
	char *ultimoRecurso;
	int socket;
	int distanciaARecurso;
	bool posicionPedida;
}t_registroPersonaje;

typedef struct
{
	char *identificador;
	char *tipo;
	int x;
	int y;
	int cantidadDisp;
}t_registroPokenest;

typedef struct
{
	char *nombre;
	char *ipEscucha;
	int puertoEscucha;
	int quantum;
	int retardo;
	char *algoritmo;
	int batalla;
	int tiempoChequeoDeadlock;
	} mapa_datos;

/*----------------------- Declaraciones de Constantes ----------------------------------*/

		/* Configuración de LOG */
		#define LOG_FILE "proceso_Mapa.log"
		#define PROGRAM_NAME "MAPA"
		#define PROGRAM_DESCRIPTION "Proceso MAPA"
		#define IS_ACTIVE_CONSOLE true
		#define T_LOG_LEVEL LOG_LEVEL_INFO

		/* Configuración de SOCKETS */

		#define PORT "10000"  // puerto por el que estamos escuchando

		/* Configuración de ARCHIVOS DE CONFIGURACION */

		#define PATH_CONFIG "../Mapas/Ciudad Paleta/metadata"

/*----------------------- Declaraciones de Prototipos ---------------------------------*/

		void *get_in_addr(struct sockaddr *sa);
		/*
		Objetivo : utilizar estructura IPv4 or IPv6 segun corresponda la familia del protocolo
		Recibe   : puntero a una estructura sockaddr
		Devuelve : Devuelve una estructura sockaddr_in o sockaddr_in6
		*/
		int leerConfiguracionCpu(mapa_datos *datos);
		/*
		Objetivo : llenar la estructura de los datos del mapa
		Recibe   : una estructura mapa_datos
		Devuelve : la estructura completa con el archivo de configuracion, si se validan todos los campos
		*/


	void enviarMensajePaquetizado(char *mensaje)
		{
/*
			switch (mensaje)
			{
				//ResponderUbicacionPokenest
				case mensaje[0]==0:

					break;
				//ResponderUbicacionActualEntrenador
				case mensaje[0]==1:

					break;
				//ResponderCapturaExitosaPokemon
				case mensaje[0]==2:

					break;
			}*/
	}

#endif /* MAPA_H_ */
