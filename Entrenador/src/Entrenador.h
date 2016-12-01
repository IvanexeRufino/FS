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
#include <commons/string.h>
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>

#include <Payback/enviarCoordenada.c>
#include <Payback/recibirCoordenada.c>
#include <Payback/charToString.c>
#include <Payback/crearSocketCliente.c>
#include <Payback/conectarConServer.c>

/*----------------------- Declaraciones de Estructuras ---------------------------------*/

typedef struct
{
	char nombre[40];
	char simbolo;
	int vidas;
	int posicionEnX;
	int posicionEnY;
	int reintentos;
	int cantDeadlock;
} entrenador_datos;

typedef struct {
	int puertoMapa;
	int socketMapa;
	int pokemonActualPosicionEnX;
	int pokemonActualPosicionEnY;
	char* ipMapa;
	char* nombreMapa;
	t_list* objetivos;
	pthread_t threadIdNivel;
} t_mapa;

/*----------------------- Declaraciones de Constantes ----------------------------------*/

	/* Configuración de LOG */

	#define LOG_FILE " - Proceso_Entrenador.log"
	#define PROGRAM_NAME "ENTRENADOR"
	#define PROGRAM_DESCRIPTION "Proceso ENTRENADOR"
	#define IS_ACTIVE_CONSOLE true
	#define T_LOG_LEVEL LOG_LEVEL_INFO

	/* Configuración de Protocolo */

	#define BIENVENIDA 0
	#define SOLICITARPOSICION 1
	#define SOLICITARAVANZAR 2
	#define ATRAPARPOKEMON 3
	#define	FINALIZACION 4

/*----------------------- Declaraciones de Prototipos ---------------------------------*/

char* objetivosDelMapa(char*);
void muerteDefinitivaPorSenial(int);
void gameOver();
void muertePorSenial(int);
void sumarVida(int);
char* objetivosDelMapa(char*);
void enviarMensajeInicial(int);
int jugar();
void devolverMedallas();
int leerConfiguracionEntrenador();
int leerConfiguracionMapa(t_mapa*);
void solicitarPosicion(t_mapa*,char);
void informarFinalizacion();
void solicitarAvanzar(t_mapa*,char);
int atraparPokemon(t_mapa*,char);
void copiarMedalla(char*,char*);
void desconectar();

#endif /* ENTRENADOR_H_ */
