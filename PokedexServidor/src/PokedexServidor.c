/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"


void* memoria(int cantidad) {
	void* puntero = NULL;
	if (cantidad !=0) {
		while (puntero == NULL) {
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
}

t_paquete* empaquetar(int codigo, void* datos, int size){
	t_paquete* paquete= malloc(sizeof(t_paquete));
	paquete->codigo= codigo;
	paquete->datos= datos;
	paquete->tamanio= size;
	return paquete;
}


void* acoplador(t_paquete* paquete) /*transforma una estructura de tipo t_paquete en un stream*/
{
	void* paqueteSalida = memoria(size_header + paquete->tamanio);
	memcpy(paqueteSalida, paquete, size_header);
	memcpy(paqueteSalida + size_header, paquete->datos, paquete->tamanio);
	return paqueteSalida;
}

t_paquete* desacoplador1(char* buffer,int sizeBuffer) {
     	t_paquete* paquete;
     	paquete= memoria(sizeof(t_paquete));
     	memcpy(paquete,buffer,size_header);
    	paquete->datos = memoria(sizeBuffer);
    	memcpy(paquete->datos, buffer + size_header, sizeBuffer);
    	return paquete;
}

t_paquete* desacoplador(char* buffer)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
  {
  	t_paquete* paquete;
  	paquete= memoria(sizeof(t_paquete));
  	memcpy(paquete,buffer,size_header);
  	paquete->datos= memoria(paquete->tamanio);
  	memcpy(paquete->datos, buffer + size_header, paquete->tamanio);
  	return paquete;
  }

int crearSocketServidor(char *puerto) {
	int BACKLOOG = 5;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(NULL, puerto, &hints, &serverInfo);
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	int yes =1;
	setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOOG);
	return listenningSocket;
}

int AceptarConexionCliente(int socketServer) {
	socklen_t longitudCliente;//esta variable tiene inicialmente el tamaño de la estructura cliente que se le pase
	struct sockaddr cliente;
	int socketNuevaConexion;//esta variable va a tener la descripcion del nuevo socket que estaria creando
	longitudCliente = sizeof(cliente);
	socketNuevaConexion = accept (socketServer, &cliente, &longitudCliente);//acepto la conexion del cliente
	if (socketNuevaConexion < 0){
		return -1;
	}
	return socketNuevaConexion;

}

void enviarQueSos(int newfd,t_paquete* paqueteSend){
	void* enviar = acoplador(paqueteSend);
	if(send(newfd,enviar,paqueteSend->tamanio + size_header ,0)<0) {
				puts("ERROR ENVIO");
				exit(1);
	}
}

t_paquete* getattr(t_paquete* paqueterecv) {
	osada_file* archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo == NULL || archivo->state == 0) {
		return empaquetar(100,"error",6);
	}
	else {
		return empaquetar(1,archivo,sizeof(osada_file));
	}
}

t_paquete* readdir(t_paquete* paqueterecv){
	int indice;
	indice= obtenerIndice(paqueterecv->datos);
	t_list* listaDeHijos= listaDeHijosDelArchivo(indice);
	char* bufo= memoria(17*list_size(listaDeHijos));
	unsigned char* nombre = malloc(17);
	int i;
	for(i=0;i<list_size(listaDeHijos);i++){
		osada_file* archivoHijo= list_get(listaDeHijos,i);
		memcpy(nombre, archivoHijo->fname,17);
		strcat(bufo,nombre);
		strcat(bufo, "/");
	}

	if(list_size(listaDeHijos) != NULL){
		return empaquetar(2,bufo,strlen(bufo)+1);
	} else {
		return empaquetar(100,"error",6);
	}
}

t_paquete* hacerdir (t_paquete* paqueterecv){
	if(paqueterecv->tamanio - 2 <= 17) {
				int error = crear_archivo(paqueterecv->datos,2);
				if (error == -1) {
					return empaquetar(101,"tabla de archivos",18);
				}
				else {
					return empaquetar(99,"ok",3);
				}
	}
	else {
		return empaquetar(100,"error",6);
	}
}

t_paquete* abrir (t_paquete* paqueterecv){
	osada_file* archivo;
	archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo == NULL || archivo->state == 0) {
		return empaquetar(100,"error",6);
	}
	else {
		return empaquetar(1,archivo,sizeof(osada_file));
	}
}

t_paquete* crear (t_paquete* paqueterecv){
	if(paqueterecv->tamanio - 2 <= 17) {
		int  error = crear_archivo(paqueterecv->datos,1);
		return empaquetar(99,"ok",3);
		if (error == -1) {
			return empaquetar(101,"tabla de archivos",18);
		}
	}
	else {
		return empaquetar(100,"error",6);
	}
}

t_paquete* leer (t_paquete* paqueterecv){
	osada_file* archivo;
	archivo = obtenerArchivo(paqueterecv->datos);
	char* buf = malloc(archivo->file_size);
	int size = leer_archivo(paqueterecv->datos, 0, 4096,buf);
	return empaquetar(6,buf,size);
}

t_paquete* escribir (t_paquete* paqueterecv){
	t_paquete* paqueteRead;
	paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
	char** bufonuevo= string_split(paqueteRead->datos,"|");
//				char* bufpath = malloc(paqueterecv->tamanio - size_header - paqueteRead->tamanio);
//				char* bufbuf = malloc(paqueterecv->tamanio);
//				memcpy(bufpath, paqueteRead->datos, paqueterecv->tamanio -size_header - paqueteRead->tamanio);
//				memcpy(bufbuf,paqueteRead->datos + strlen(bufpath), paqueteRead->tamanio);
	int tam = escribir_archivo(bufonuevo[0],paqueteRead->codigo,paqueteRead->tamanio,bufonuevo[1]);
	return empaquetar(7,bufonuevo[1],tam);
}

t_paquete* remover (t_paquete* paqueterecv){
	osada_file* archivo;
	archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo->state == 1) {
		borrar_archivo(paqueterecv->datos);
	}
	else if(archivo->state == 2) {
		borrar_directorio_vacio(paqueterecv->datos);
	}
	if(archivo == NULL || archivo->state == 0) {
		return empaquetar(100,"error",6);
	}
	return 0;
}

t_paquete* truncar(t_paquete* paqueterecv){
	osada_file* archivo;
	t_paquete* paqueteRead;
	paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
	archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo == NULL) {
		return empaquetar(100,"error",6);
	} else {
		truncar_archivo(archivo,(uint32_t)paqueteRead->tamanio);
	}
	return 0;
}

void renombrar (t_paquete* paqueterecv){
	char* buforecibidox;
	char* buforecibido;
	buforecibido= malloc(paqueterecv->tamanio);
	buforecibido= paqueterecv->datos;
	char** bufonuevox= string_split(buforecibido,"%");
	renombrar_archivo(bufonuevox[0],bufonuevox[1]);
}

void linkear (t_paquete* paqueterecv){
	char* buforecibidox;
	char* buforecibido;
	buforecibidox= malloc(paqueterecv->tamanio);
	buforecibidox= paqueterecv->datos;
	char** bufonuevoxx= string_split(buforecibidox,"%");
	copiar_archivo(bufonuevoxx[0],bufonuevoxx[1]);
}

void recibirQueSos(int newfd){
	char* buforecibidox;
	char* buforecibido;
	osada_file* archivo;
	t_paquete* paqueteRead;
	t_paquete* paqueteSend;
	char buffer[MAX_BUFFERSIZE];
	int sizebytes;
	int indice;
	if((sizebytes = recv(newfd, &buffer, MAX_BUFFERSIZE - 1,0)) <= 0)
	{
		puts("ERROR RECIBIR");
		//log_error(logDelPersonaje, "Error al recibir paquete del cliente \n");
		exit(1);
	}
	printf("El cliente me envía un paquete \n");
	t_paquete* paqueterecv = desacoplador(buffer);


	printf("el codigo es %d \n", paqueterecv->codigo);
	printf("el tamaño es %d \n", paqueterecv->tamanio);

		switch(paqueterecv->codigo){
			case 1:
				printf("el path es %s \n", paqueterecv->datos);
				paqueteSend=getattr(paqueterecv);
				break;
			case 2:
				printf("el path es %s \n", paqueterecv->datos);
				paqueteSend= readdir(paqueterecv);
				break;
			case 3:
				printf("el path es %s \n", paqueterecv->datos);
				paqueteSend= hacerdir(paqueterecv);
				break;
			case 4:
				printf("el path es %s \n", paqueterecv->datos);
				paqueteSend= crear(paqueterecv);
				break;
			case 5:
				printf("el path es %s \n", paqueterecv->datos);
				paqueteSend= abrir(paqueterecv);
				break;
			case 6:
				paqueteSend= leer(paqueterecv);
//				paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
//				printf("el path es %s \n", paqueteRead->datos);
//				printf("el offset es %d \n", paqueteRead->codigo);
//				printf("el size es %d \n", paqueteRead->tamanio);
//				archivo = obtenerArchivo(paqueteRead->datos);
//				char* buf = malloc(minimoEntre(archivo->file_size, paqueteRead->tamanio));
//				int size = leer_archivo(paqueteRead->datos,paqueteRead->codigo,paqueteRead->tamanio,buf);
//				archivo = obtenerArchivo(paqueterecv->datos);
//				char* buf = malloc(archivo->file_size);
//				int size = leer_archivo(paqueterecv->datos, 0, 4096,buf);
//				paqueteSend = empaquetar(6,buf,size);
				break;
			case 7:
				paqueteSend= escribir(paqueterecv);
//				paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
//				char** bufonuevo= string_split(paqueteRead->datos,"|");
//				char* bufpath = malloc(paqueterecv->tamanio - size_header - paqueteRead->tamanio);
//				char* bufbuf = malloc(paqueterecv->tamanio);
//				memcpy(bufpath, paqueteRead->datos, paqueterecv->tamanio -size_header - paqueteRead->tamanio);
//				memcpy(bufbuf,paqueteRead->datos + strlen(bufpath), paqueteRead->tamanio);
//				int tam = escribir_archivo(bufonuevo[0],paqueteRead->codigo,paqueteRead->tamanio,bufonuevo[1]);
//				paqueteSend = empaquetar(7,bufonuevo[1],tam);
				break;
			case 8:
				paqueteSend= remover(paqueterecv);
				break;
			case 9:
				break;
			case 10:
				paqueteSend= truncar(paqueterecv);
				break;
			case 11:
				renombrar(paqueterecv);
				break;
			case 12:
				linkear(paqueterecv);
				break;
			}

		enviarQueSos(newfd,paqueteSend);
}

void sigchld_handler(int s){
	while(wait(NULL) > 0);
}

int main(void) {

//	system("truncate -s 200k disco.bin");
//	system("./osada-format disco.bin");
	reconocerOSADA("/home/utnso/base.bin");

	int sockfd, new_fd;  // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del cliente
	int sin_size;
	struct sigaction sa;
	int yes=1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	    }
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(PORT);     // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
	   exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	while(1) {
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
		if (!fork()) { // Este es el proceso hijo
			close(sockfd); // El hijo no necesita este descriptor
			while(1) {
				recibirQueSos(new_fd);
			}
		}
		close(new_fd);  // El proceso padre no lo necesita
		}
	return 0;
}
