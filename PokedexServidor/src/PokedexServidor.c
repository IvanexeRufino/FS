/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"

#define PORT "9999"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);

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
	       fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
	        //log_info(logger, "fallo lectura datos locales");
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
	    fprintf(stderr, "selectserver: failed to bind\n");
	      //  log_info(logger, "fallo el bind con el socket listener");
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
	                   //     log_info(logger, "Error en el accept");
	                    } else {
	                        FD_SET(newfd, &master); // añadir al conjunto maestro
	                        if (newfd > fdmax) {    // actualizar el máximo
	                            fdmax = newfd;
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
	                            printf("selectserver: socket %d hung up\n", i);
	                        } else {
	                      //      log_info(logger, "Error al recibir datos");
	                        }
	                        close(i); // bye!
	                        FD_CLR(i, &master); // eliminar del conjunto maestro
	                    }
	                    	printf("\n %s", buf);
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

	                } // END handle data from client
	            } // END got new incoming connection
	        } // END loop file descriptors
	    } // END for(;;)--and you thought it would never end!

	    return 0;
	}

