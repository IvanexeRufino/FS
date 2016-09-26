#include "Mapa.h"


t_list* listaPokenest;
t_list* items;
mapa_datos* infoMapa;
t_list* entrenadoresActivos;
t_list* entrenadoresBloqueados;

void leerConfiguracionPokenest(char* mapa);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int leerConfiguracionMapa( t_list* listaPokenest)
{

	char nombre[10];
	printf("%s", "Nombre del Mapa?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[90] = "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/Mapas/Pueblo";
	strcat(pathconfigMetadata, nombre);
	strcat(pathconfigMetadata,  "/metadata");
	t_config* config = config_create(pathconfigMetadata);

	// Verifico que los parametros tengan sus valores OK
	if ( config_has_property(config, "IP") && config_has_property(config, "Puerto")
	&& config_has_property(config, "algoritmo") && config_has_property(config, "quantum")
	&& config_has_property(config, "retardo") && config_has_property(config, "Batalla")
	&& config_has_property(config, "TiempoChequeoDeadlock"))
	{
		infoMapa->nombre = nombre;
		infoMapa->tiempoChequeoDeadlock = config_get_int_value(config, "TiempoChequeoDeadlock");
		infoMapa->batalla = config_get_int_value(config, "Batalla");
		infoMapa->algoritmo  = config_get_string_value(config, "algoritmo");
		infoMapa->quantum = config_get_int_value(config, "quantum");
		infoMapa->retardo = config_get_int_value(config, "retardo");
		infoMapa->ipEscucha = config_get_string_value(config, "IP");
		infoMapa->puertoEscucha  = config_get_string_value(config, "Puerto");

		printf(" El nombre del mapa es: %s\n su tiempoChequeoDeadlock es: %d\n su batalla es: %d\n "
				"su algoritmo es: %s\n su quantum es de: %d\n su retardo es: %d\n su ipEscucha es: %s\n "
				"su puertoEscucha es: %s\n"
								,infoMapa->nombre,
								infoMapa->tiempoChequeoDeadlock,
								infoMapa->batalla,
								infoMapa->algoritmo,
								infoMapa->quantum,
								infoMapa->retardo,
								infoMapa->ipEscucha,
								infoMapa->puertoEscucha);
		//Leo todas la pokenest ******************************************************* POR AHORA UNA

//	leerConfiguracionPokenest(nombre);

	}
	else
    {
		return -1;
    }
	return 1 ;
}

void leerConfiguracionPokenest(char mapa[10]){
	char pathpokenestMetadata[100] = "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/Mapas/Pueblo";
	strcat(pathpokenestMetadata, mapa);
	strcat(pathpokenestMetadata,"/PokeNests/Pikachu/metadata");
	t_config* configNest = config_create(pathpokenestMetadata);
	if (config_has_property(configNest, "Tipo") && config_has_property(configNest, "Identificador")){
		t_registroPokenest* pokenest = malloc(sizeof(t_registroPokenest));
		pokenest->tipo = config_get_string_value(configNest, "Tipo");
		pokenest->identificador= config_get_string_value(configNest, "Identificador");
		pokenest->x =config_get_int_value(configNest,"X");
		pokenest->y = config_get_int_value(configNest,"Y");

		printf("\n El nombre del Nest es: Pikachu\n su posicion es X: %d\n Y es: %d\n "
							"su identificador: %s\n"
											,pokenest->x, pokenest->y, pokenest->identificador);

	list_add(listaPokenest,pokenest);
	}
}

void funcionDelThread (t_list* entrenadoresActivos){

	puts("hola");
}

t_registroPersonaje *get_personaje_en_socket(int socket) {
	int _with_socket(t_registroPersonaje *p) {
		return (p->socket == socket);
	}

	return list_find(entrenadoresActivos, (void*)_with_socket);
}

t_registroPokenest *get_pokenest_identificador(char identificador) {
	int _with_identificador(t_registroPokenest *p) {
		return (p->identificador == identificador);
	}

	return list_find(entrenadoresActivos, (void*)_with_identificador);
}

void recibirEntrenador(int newfd){
	char* buffer = malloc(2);
	recv(newfd, buffer, sizeof(char) * 2, 0);
	int a;
	t_registroPersonaje* nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	memcpy(&(a), buffer, sizeof(char));
	memcpy(&(nuevoPersonaje->identificador), buffer + sizeof(char)  ,  sizeof(char));

	                    //----RECIBO OBJETIVOS
	recv(newfd, nuevoPersonaje->objetivos,sizeof(char)*7,0);
	//puts(nuevoPersonaje->objetivos);
	nuevoPersonaje->socket=newfd;
	//printf("reciving char: %c\n", a);
	//printf("reciving char: %c\n", nuevoPersonaje->identificador);
	nuevoPersonaje->x = 1;
	nuevoPersonaje->y = 1;
	list_add(entrenadoresActivos, nuevoPersonaje);
    CrearPersonaje(items, nuevoPersonaje->identificador[0], 0, 0);
    free(buffer);
    free(nuevoPersonaje);
}


int main(int argc, char **argv)
{
	listaPokenest = malloc(sizeof(t_registroPokenest));
	listaPokenest = list_create();
	items = malloc(sizeof(t_list));
	items = list_create();
	entrenadoresActivos = malloc(sizeof(t_list));
	entrenadoresBloqueados = malloc (sizeof(t_list));

		/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
		log_info(logger, PROGRAM_DESCRIPTION);

	//--------
	  infoMapa = malloc((sizeof(char*)*4 + sizeof(int)*4));
	  if (leerConfiguracionMapa (listaPokenest) == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");

 pthread_t idHiloPlanificador;

 pthread_create (&idHiloPlanificador, NULL, (void*) funcionDelThread, NULL);

 pthread_join(idHiloPlanificador,0);
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

    int yes=1;        // para setsockopt() SO_REUSEADDR, más abajo
    int i, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // borra los conjuntos maestro
    FD_ZERO(&read_fds);  // borra los conjuntos temporal

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL,infoMapa->puertoEscucha , &hints, &ai)) != 0) {
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

    items = list_create();
    list_add_all(items,listaPokenest);
   int rows, cols;
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&rows, &cols);
	//int c = 1;
	//int r = 1;
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
                    //gestionar nuevas conexiones
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        log_info(logger, "Error en el accept");
                    } else {
                        FD_SET(newfd, &master); // añadir al conjunto maestro
                        if (newfd > fdmax) {    // actualizar el máximo
                            fdmax = newfd;
                            	///----------------------------------RECIBO ENTRENADOR
                            recibirEntrenador(newfd);

                        }
               //      printf("selectserver: new connection from %s on " "socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),remoteIP, INET6_ADDRSTRLEN),newfd);
                    }
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
                    	int turno = 0;

                    	while(turno <= fdmax){
                    		send(turno, "Tu turno", 10, 0);
                    		recv(turno, buf, sizeof buf, 0);
                    		char *header;
                    		char *payload;
                    		memcpy(&(header), buf, sizeof(char));
                    		memcpy(&(payload), buf + sizeof(char)  ,  sizeof(char));
           					char *identificadorPokenest = payload;
                    			switch(buf[0]){
                    				case '1': 	{
                    					//PedirPosicion Pokenest

                    					t_registroPokenest *pokenestSolicitada = get_pokenest_identificador(identificadorPokenest);
                                		char *msg = "x :";
                                		strcat(msg, pokenestSolicitada->x);
                                		strcat(msg," y: ");
                                		strcat(msg,pokenestSolicitada->y);
                    					send(turno, msg , 30, 0);
                    					break;
                    				}
                    				case'2':
                    				{//Mover
                    					t_registroPersonaje *personaje = get_personaje_en_socket(turno);
                    					t_registroPokenest *pokenestSolicitada = get_pokenest_identificador(identificadorPokenest);
                    						if(personaje->ultimoRecurso == 'x'){			//Si la ultima vez me movi en x ahora me tengo que mover en y;
                    							if(personaje->y <= pokenestSolicitada->y){
                    								personaje->y++;
                    								}
                    							else{
                    								personaje->y --;
                    							}
                    							personaje->ultimoRecurso = "y";
                    						}
                    						else{
                    							if(personaje->x <= pokenestSolicitada->x){
                    								personaje->x++;
                   								}
                    							else{
                    									personaje->x --;
                    								}
                    								personaje->ultimoRecurso = "x";
                    							}
                    						MoverPersonaje(items, personaje->identificador[0], personaje->y , personaje->x);
                    					break;
                    				}
                    				case '3':		//Atrapar Pokemon
                    					break;
                    				default:
                    					break;


                    		turno ++;
                    		}

                    	}
                    	//log_info(logger, "Recibiendo datos de un cliente");
 //                   	t_registroPersonaje *personaje = get_personaje_en_socket(i);

//                    	switch(buf[0]){
//                    				case 'J':
//                    				case 'j':
//                    					if (personaje->y > 1) {
//                    						personaje->y -- ;
//                         					}
//                    					break;
//                    				case'L':
//                    				case 'l':
//           								if (personaje-> y < rows) {
//            									personaje->y++;
//              								}
//          							break;
//                    				case 'I':
//                    				case 'i':
//										if (personaje->x > 1) {
//												personaje->x0-;
//												printf("%d",personaje->x);
//											}
//           							break;
//                    				case 'K':
//          							case 'k':
//          								if (personaje->x < cols) {
//       									personaje->x++;
//      								}
//          								break;
//
//          		                 MoverPersonaje(items, personaje->identificador[0], personaje->y , personaje->x);
//                    	}
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    	nivel_gui_dibujar(items, infoMapa->nombre);

    } // END for(;;)--and you thought it would never end!

    return 0;

}
