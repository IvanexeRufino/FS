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

typedef struct{
	t_registroPersonaje* entrenador;
	t_pokemon* pok;
}t_pokEn;


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

	char rutaArgv[100];
	pid_t pid;
	int filas, columnas;
	t_log* logger;
	int threadAEjecutar;
	int systemInDeadlock = false;	//detección de deadlock

	t_registroPersonaje *hiloEscucha;
	mapa_datos* infoMapa;

	t_list* listaPokenest;
	t_list* items;
	t_list* entrenadores_listos;
	t_list* entrenadores_bloqueados;
	t_list* listapokEn;
	t_pkmn_factory* fabricaPokemon;

	//manejo de pokenests
	t_dictionary *available;
	t_dictionary *request;
	t_dictionary *alloc;

/*----------------------- Declaraciones de Semaforos Globales ---------------------------*/

	sem_t colaDeListos;
	sem_t pasoDeEntrenador;
	sem_t turnoMain;

	pthread_mutex_t mutex_EntrenadoresActivos = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_threadAEjecutar = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_bloqPlanificador = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_entrenadorEnEjecucion = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_Ejecucion = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_siguienteQuantum = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //PARA EL DEADLOCK

/*----------------------- Declaraciones de Constantes ----------------------------------*/

		/* Configuración de LOG */
		#define LOG_FILE "proceso_Mapa.log"
		#define PROGRAM_NAME "MAPA"
		#define PROGRAM_DESCRIPTION "Proceso MAPA"
		#define IS_ACTIVE_CONSOLE false
		#define T_LOG_LEVEL LOG_LEVEL_INFO

		/* Configuración de SOCKETS */

		#define PORT "10000"  // puerto por el que estamos escuchando
		#define BACKLOG 10

		/* Configuración de ARCHIVOS DE CONFIGURACION */

		#define PATH_CONFIG "../Mapas/Ciudad Paleta/metadata"

/*----------------------- Declaraciones de Prototipos ---------------------------------*/

void pokemonMasFuerteDe(t_registroPersonaje* );
int leerDatosBill(char* , char*);
void recuperarPokemonDeEntrenador(t_registroPersonaje* );
void copiarPokemonAEntrenador(t_registroPersonaje *, t_registroPokenest* );
int leerConfiguracionMapa();
int reLeerConfiguracionMapa();
void leerConfiguracionPokenest(char*, char*);
char recibirBienvenidaEntrenador(int ,t_registroPersonaje* );
void cargoDatosPokemonActual(char ,t_registroPokenest* );
int distanciaAProxObjetivo(t_registroPersonaje* , char );
int calcularMasCercanoASuObjetivo ();
void mover (t_registroPersonaje* , t_registroPokenest* );
void envioQueSeAtrapoPokemon (t_registroPersonaje*, t_registroPokenest* );
void recibirQueHacer(t_registroPersonaje* );
void ejecutar_Entrenador(parametros_entrenador* );
void planificarNuevo();
void releerconfig(int);
char *liberar_recursos(char*);
void liberar_recurso(char*, char*, int);
int asignar_recurso(char*, char*, int);
void batallaPokemon();
void *detectar_interbloqueo(void*);

#endif /* MAPA_H_ */
