#include "Mapa.h"

t_list* listaPokenest;
mapa_datos* infoMapa;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int leerConfiguracionMapa(mapa_datos *datos, t_list* listaPokenest)
{
	char nombre[10];
	printf("%s", "Nombre del Mapa?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[90] = "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/Mapas/Ciudad ";
	strcat(pathconfigMetadata, nombre);
	strcat(pathconfigMetadata,  "/metadata");
	t_config* config = config_create(pathconfigMetadata);

	// Verifico que los parametros tengan sus valores OK
	if ( config_has_property(config, "IP") && config_has_property(config, "Puerto")
	&& config_has_property(config, "algoritmo") && config_has_property(config, "quantum")
	&& config_has_property(config, "retardo") && config_has_property(config, "Batalla")
	&& config_has_property(config, "TiempoChequeoDeadlock"))
	{
		datos->nombre = nombre;
		datos->tiempoChequeoDeadlock = config_get_int_value(config, "TiempoChequeoDeadlock");
		datos->batalla = config_get_int_value(config, "Batalla");
		datos->algoritmo  = config_get_string_value(config, "algoritmo");
		datos->quantum = config_get_int_value(config, "quantum");
		datos->retardo = config_get_int_value(config, "retardo");
		datos->ipEscucha = config_get_string_value(config, "IP");
		datos->puertoEscucha  = config_get_int_value(config, "Puerto");

		printf(" El nombre del mapa es: %s\n su tiempoChequeoDeadlock es: %d\n su batalla es: %d\n "
				"su algoritmo es: %s\n su quantum es de: %d\n su retardo es: %d\n su ipEscucha es: %s\n "
				"su puertoEscucha es: %d\n"
								,datos->nombre,
								datos->tiempoChequeoDeadlock,
								datos->batalla,
								datos->algoritmo,
								datos->quantum,
								datos->retardo,
								datos->ipEscucha,
								datos->puertoEscucha);
		//Leo todas la pokenest ******************************************************* POR AHORA UNA
	//	if(leerConfiguracionPokenest (listaPokenest, nombre)== 1)
			return 1;
	}
	else
    {
		return -1;
    }

}

int leerConfiguracionPokenest(t_list* pokeNests, char* mapa){
	char pathpokenestMetadata[40] = "Mapas/Ciudad ";
	strcat(pathpokenestMetadata, mapa);
	strcat(pathpokenestMetadata,"/PokeNests/Pikachu/metadata");
	t_config* configNest = config_create(pathpokenestMetadata);
	if (config_has_property(configNest, "Tipo") /*&& config_has_property(configNest, "Posicion")*/ && config_has_property(configNest, "Identificador")){
		t_registroPokenest* pokenest = malloc(sizeof(t_registroPokenest));
		pokenest->tipo = config_get_string_value(configNest, "Tipo");
		pokenest->identificador= config_get_string_value(configNest, "Identificador");
		pokenest->x =config_get_int_value(configNest,"X");
		pokenest->y = config_get_int_value(configNest,"Y");

		printf("\n El nombre del Nest es: Pikachu\n su posicion es X: %d\n Y es: %d\n "
						"su identificador: %s\n"
										,pokenest->x, pokenest->y, pokenest->identificador);

		pokeNests = malloc(sizeof(t_list));
		list_add(pokeNests,(void*) pokenest);
		return 1;
	}

	return -1;
}

int main(void)
{
	listaPokenest = malloc(sizeof(t_list));
	listaPokenest = list_create();

		/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
		log_info(logger, PROGRAM_DESCRIPTION);

	//--------
	  infoMapa = malloc(sizeof(mapa_datos));
	  if (leerConfiguracionMapa (infoMapa, listaPokenest) == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");


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
    if ((rv = getaddrinfo(NULL, PORT , &hints, &ai)) != 0) {
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

	//Inicializo la gui --------------------------------

    t_list* items = list_create();
    list_add_all(items,listaPokenest);
    int rows, cols;
	int c,r;
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&rows, &cols);
	c = 1;
	r = 1;
	nivel_gui_dibujar(items,  infoMapa->nombre );



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
                            //agrego un entrenador nuevo

                            char* buffer = malloc(1);
                            recv(newfd, buffer, 1, 0);
                            t_registroPersonaje* nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
                            memcpy(&(nuevoPersonaje->identificador), buffer, 1);
                            nuevoPersonaje->socket=newfd;
                          //  printf("reciving char: %c\n", nuevoPersonaje->identificador);
                            CrearPersonaje(items, nuevoPersonaje-> identificador, 0, 0);
                        }
               /*        printf("selectserver: new connection from %s on "
                           "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                  */  }
                } else {
                    // gestionar datos de un cliente
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // error o conexión cerrada por el cliente
                        if (nbytes == 0) {
                            // conexión cerrada
                           printf("selectserver: socket %d hung up\n", i);
                        } else {
                            log_info(logger, "Error al recibir datos");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // eliminar del conjunto maestro
                    } else {
                    //	log_info(logger, "Recibiendo datos de un cliente");
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

                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors

    	MoverPersonaje(items, '@', r, c);
    	nivel_gui_dibujar(items, "Mapa");

    } // END for(;;)--and you thought it would never end!

    return 0;


    }

