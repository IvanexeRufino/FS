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
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>

#include <Payback/str_cut.h>
#include <Payback/enviarCoordenada.c>
#include <Payback/charToString.c>
#include <Payback/crearSocketCliente.c>
#include <Payback/conectarConServer.c>
#include <Payback/convertirStreamAString.c>

/*----------------------- Declaraciones de Estructuras ---------------------------------*/

typedef struct
{
	char nombre[20];
	char simbolo;
	int vidas;
	int posicionEnX;
	int posicionEnY;
} entrenador_datos;

typedef struct {
	int puertoMapa;
	int socketMapa;
	int pokemonActualPosicionEnX;
	int pokemonActualPosicionEnY;
	char* ipMapa;
	char* nivel;
	bool estaTerminado;
	t_list* objetivos;
	pthread_t threadIdNivel;
} t_nivel;

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

/*----------------------- Declaraciones de Prototipos ---------------------------------*/

char* objetivosDelMapa(char* );
void muerteDefinitivaPorSenial(int );
void gameOver();
void muertePorSenial(int );
void sumarVida(int );
char* objetivosDelMapa(char* );
void imprimirClaveYValor(char* , void* );
void enviarMensajeInicial(int );
void jugarTurno(int );
void sendObjetivosMapa(int );
int conectarConServer(char *, int );
int crearSocketCliente(char [], int );


#endif /* ENTRENADOR_H_ */
