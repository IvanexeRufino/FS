<<<<<<< HEAD
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
#define PORT "10000"  // port we're listening on

	/* Configuración de LOG */
	#define LOG_FILE "proceso_Mapa.log"


	/* Configuración de LOG */
	#define PROGRAM_NAME "MAPA"
	#define PROGRAM_DESCRIPTION "Proceso MAPA"
	#define IS_ACTIVE_CONSOLE true
	#define T_LOG_LEVEL LOG_LEVEL_INFO

// get sockaddr, IPv4 or IPv6:
=======
#include "Mapa.h"

>>>>>>> 1480d91e16cd430ca24d4ec99380db791097109d
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

<<<<<<< HEAD
typedef struct
{
	char identificador;
	char ultimoRecurso;
	int socket;
	int distanciaARecurso;
	bool posicionPedida;
}t_registroPersonaje;

typedef struct
{
	char identificador;
	char tipo;
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
int leerConfiguracionMapa(mapa_datos *datos)
=======
int leerConfiguracionCpu(mapa_datos *datos )
>>>>>>> 1480d91e16cd430ca24d4ec99380db791097109d
{

	char nombre[10];
	//printf("%s", "Nombre del Mapa?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[40] = "Mapas/Ciudad ";
	strcat(pathconfigMetadata, nombre);
	strcat(pathconfigMetadata,  "/metadata");
	t_config* config = config_create(pathconfigMetadata);
	if ( config_has_property(config, "IP") && config_has_property(config, "Puerto") && config_has_property(config, "algoritmo") && config_has_property(config, "quantum") && config_has_property(config, "retardo") && config_has_property(config, "Batalla") && config_has_property(config, "TiempoChequeoDeadlock"))
	{
	/*	int a = config_get_int_value(config,"TiempoChequeoDeadlock");
		printf("%i", a);			// Esto es para probar si lee el archivo metadata
		puts(pathconfigMetadata);   */
		datos->nombre = nombre;
		datos->tiempoChequeoDeadlock = config_get_int_value(config, "TiempoChequeoDeadlock");
		datos->batalla = config_get_int_value(config, "batalla");
		datos->algoritmo  = config_get_string_value(config, "algoritmo");
		datos->quantum = config_get_int_value(config, "quantum");
		datos->retardo = config_get_int_value(config, "retardo");
		datos->ipEscucha = config_get_string_value(config, "IP");
		datos->puertoEscucha  = config_get_int_value(config, "Puerto");
		return 1;
	}
	else
    {
		return -1;
    }

}
<<<<<<< HEAD

t_list* listaPersonajes;
t_list* listaPokenest;

int main(void)
{
	mapa_datos infoMapa;
	listaPokenest = list_create();
	listaPersonajes = list_create();
	/* Inicializacion y registro inicial de ejecucion */
=======
int main(void)
{
		/* Inicializacion y registro inicial de ejecucion */
>>>>>>> 1480d91e16cd430ca24d4ec99380db791097109d
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
		log_info(logger, PROGRAM_DESCRIPTION);

	//--------

		  if ( leerConfiguracionMapa ( &infoMapa ) == 1 )
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");


		//Inicializo la gui --------------------------------
	t_list* items = list_create();
	int rows, cols;
	int c,r;
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&rows, &cols);
	c = 1;
	r = 1;

	// --------------------------------
	//Inicializo la config del mapa

    fd_set master;    // conjunto maestro de descriptores de fichero
    fd_set read_fds;  // conjunto temporal de descriptores de fichero para select()
    int fdmax;        // número máximo de descriptores de fichero

    int listener;     // descriptor de socket a la escucha
    int newfd;        // descriptor de socket de nueva conexión aceptada
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer para datos del cliente
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // para setsockopt() SO_REUSEADDR, más abajo
    int i, rv;
    //int j;
    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // borra los conjuntos maestro
    FD_ZERO(&read_fds);  // borra los conjuntos temporal

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
 //       fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        log_info(logger, "Fallo la lectura de datos locales para el socket");
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // obviar el mensaje "address already in use" (la dirección ya se está usando)
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
//        fprintf(stderr, "selectserver: failed to bind\n");
        log_info(logger, "fallo el bind con el socket listener");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // Aca ponemos a escuchar y validar si hay un error
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // añadir listener al conjunto maestro
    FD_SET(listener, &master);

    // seguir la pista del descriptor de fichero mayor
    fdmax = listener; // por ahora es éste

	nivel_gui_dibujar(items,  infoMapa.nombre );



    // bucle principal
    for(;;) {
    	read_fds = master; // cópialo
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // explorar conexiones existentes en busca de datos que leer
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // ¡tenemos datos!
                if (i == listener) {
                    // gestionar nuevas conexiones
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        //perror("accept");
                        log_info(logger, "Error en el accept");
                    } else {
                        FD_SET(newfd, &master); // añadir al conjunto maestro
                        if (newfd > fdmax) {    // actualizar el máximo
                            fdmax = newfd;
                            //agrego un personaje nuevo
  /*                          t_registroPersonaje* nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
                            list_add(listaPersonajes,nuevoPersonaje);
                            nuevoPersonaje->identificador = '@';// *(unPaquete->datos);
                            nuevoPersonaje->socket= socket;
                            nuevoPersonaje->ultimoRecurso= '\n';*/
               //             CrearPersonaje(items, nuevoPersonaje->identificador, 0, 0);
                        }
                       printf("selectserver: new connection from %s on "
                           "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                    }
                } else {
                    // gestionar datos de un cliente
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // error o conexión cerrada por el cliente
                        if (nbytes == 0) {
                            // conexión cerrada
//                           printf("selectserver: socket %d hung up\n", i);
                        } else {
                            log_info(logger, "Error al recibir datos");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // eliminar del conjunto maestro
                    } else {
                    	log_info(logger, "Recibiendo datos de un cliente");
                    	switch (buf[0]){
                    				case 'J':
                    				case 'j':
                    					if (r > 1) {
                    							r--;
                    					}
                    					break;
                    				case'L':
                    				case 'l':
           								if (r < rows) {
            									r++;
              								}
          							break;
                    				case 'I':
                    				case 'i':
										if (c > 1) {
												c--;
											}
           							break;
                    				case 'K':
          							case 'k':
          								if (c < cols) {
       									c++;
      								}
          								break;
                    			}

             //       	printf("\n %s", buf);
                    	/*char *message = "recibido\0";
                    	if ((send(i, message,sizeof(message)+1, 0))<=0)
							{
								printf("error mandando");
							}
                    	else{
                    		printf("ok mandado");
                    	}




                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {


                               // except the listener and ourselves
                                if (j != listener (&& j != i)) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                        printf(buf);
                                    }
                                        else
                                        {
                                        	printf(buf);
                                        }

                                }
                            }
                        }*/
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors

    	MoverPersonaje(items, '@', r, c);
    	nivel_gui_dibujar(items, "Prueba");

    } // END for(;;)--and you thought it would never end!

    return 0;


    }

