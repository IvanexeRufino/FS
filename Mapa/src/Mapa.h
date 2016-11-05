#ifndef MAPA_H_
#define MAPA_H_

/*----------------------- Declaraciones de Bibliotecas ---------------------------------*/
#include <pkmn/battle.h>
#include <pkmn/factory.h>
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
#include <signal.h>

#include <Payback/aceptarConexionCliente.c>
#include <Payback/str_cut.h>
#include <Payback/enviarCoordenada.c>
#include <Payback/iniciarSocketServidor.c>
#include <Payback/crearSocketServidor.c>
#include <Payback/charToString.c>
#include <Payback/string.c>
#include <Payback/recibirCoordenada.c>

/*----------------------- Declaraciones de Estructuras ---------------------------------*/

typedef struct
{
	char nombre[50];
	char identificador;
	char tipo[15];
	int x;
	int y;
	int cantidadDisp;
}t_registroPokenest;



typedef struct
{
	char nombre[40];
	char identificador;
	char proximoObjetivo;
	int ultimoRecurso;  // 0 = ultimo movimiento fue en X ---- 1 = ultimo movimiento fue en Y
	int socket;
	int x;
	int y;
	bool marcado;
	int distanciaARecurso;
	bool posicionPedida;
	int threadId;
	char estado; // E= en ejecucion, B= bloqueado, L = listo 	T = terminado    M = es el hilo Main
	sem_t comienzoTurno;
	sem_t finTurno;
	char accion;
	int quantumFaltante;
	t_registroPokenest* pokemonActual;

}t_registroPersonaje;



typedef struct
{
	char nombre[30];
	char *ipEscucha;
	char *puertoEscucha;
	int quantum;
	int retardo;
	char *algoritmo;
	int batalla;
	int tiempoChequeoDeadlock;
} mapa_datos;

typedef struct
	{
	int newfd;
	pthread_t idHilo;

	} parametros_entrenador;

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

void leerConfiguracionPokenest(char*, char*);
void liberar_recurso(char*, char*, int);
char *liberar_recursos(char*);
int asignar_recurso(char*, char*, int);
int leerDatosBill(char* , char*);

#endif /* MAPA_H_ */
