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
#include <semaphore.h>
#include <dirent.h>
#include <time.h>

#include <Payback/aceptarConexionCliente.c>
#include <Payback/str_cut.h>
#include <Payback/enviarCoordenada.c>
#include <Payback/iniciarSocketServidor.c>
#include <Payback/crearSocketServidor.c>
#include <Payback/charToString.c>
#include <Payback/string.c>
/*----------------------- Declaraciones de Estructuras ---------------------------------*/

typedef struct
{
	char identificador;
	int ultimoRecurso;  // 0 = ultimo movimiento fue en X ---- 1 = ultimo movimiento fue en Y
	int socket;
	int x;
	int y;
	int distanciaARecurso;
	bool posicionPedida;
}t_registroPersonaje;

typedef struct
{
	char identificador;
	char tipo[15];
	int x;
	int y;
	int cantidadDisp;
}t_registroPokenest;

typedef struct
{
	char nombre[20];
	char *ipEscucha;
	char *puertoEscucha;
	int quantum;
	int retardo;
	char *algoritmo;
	int batalla;
	int tiempoChequeoDeadlock;
} mapa_datos;

/*----------------------- Declaraciones de Variables Globales ---------------------------*/


/*----------------------- Declaraciones de Constantes ----------------------------------*/

		/* Configuración de LOG */
		#define LOG_FILE "proceso_Mapa.log"
		#define PROGRAM_NAME "MAPA"
		#define PROGRAM_DESCRIPTION "Proceso MAPA"
		#define IS_ACTIVE_CONSOLE true
		#define T_LOG_LEVEL LOG_LEVEL_INFO

		/* Configuración de SOCKETS */

		#define PORT "10000"  // puerto por el que estamos escuchando
		#define BACKLOG 10

		/* Configuración de ARCHIVOS DE CONFIGURACION */

		#define PATH_CONFIG "../Mapas/Ciudad Paleta/metadata"

/*----------------------- Declaraciones de Prototipos ---------------------------------*/

		void *get_in_addr(struct sockaddr *sa);
		/*
		Objetivo : utilizar estructura IPv4 or IPv6 segun corresponda la familia del protocolo
		Recibe   : puntero a una estructura sockaddr
		Devuelve : Devuelve una estructura sockaddr_in o sockaddr_in6
		*/
		void leerConfiguracionPokenest(char* mapa, char* path);

#endif /* MAPA_H_ */
