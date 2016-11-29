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
	free(enviar);
}

void getattr(int newfd, t_paquete* paqueterecv) {
	osada_file* archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo == NULL || archivo->state == 0) {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	}
	else {
		enviarQueSos(newfd, empaquetar(1,archivo,sizeof(osada_file)));
	}
}

void readdir(int newfd, t_paquete* paqueterecv){
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

	if(list_size(listaDeHijos) != 0){
		enviarQueSos(newfd, empaquetar(2,bufo,strlen(bufo)+1));
	} else {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	}
}

void hacerdir (int newfd, t_paquete* paqueterecv){
	if(strlen(adquirirNombre(paqueterecv->datos)) <= 17) {
		int error = crear_archivo(paqueterecv->datos,2);
		if (error == -1) {
			enviarQueSos(newfd, empaquetar(101,"tabla de archivos",18));
		} else {
			enviarQueSos(newfd, empaquetar(99,"ok",3));
		}
	}
	else {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	}
}

void abrir (int newfd, t_paquete* paqueterecv){
	osada_file* archivo;
	archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo == NULL || archivo->state == 0) {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	}
	else {
		enviarQueSos(newfd, empaquetar(1,archivo,sizeof(osada_file)));
	}
}

void crear (int newfd, t_paquete* paqueterecv){
	if(strlen(adquirirNombre(paqueterecv->datos)) <= 17) {
		int  error = crear_archivo(paqueterecv->datos,1);
		enviarQueSos(newfd, empaquetar(99,"ok",3));
		if (error == -1) {
			enviarQueSos(newfd, empaquetar(101,"tabla de archivos",18));
		}
	}
	else {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	}
}

void leer (int newfd, t_paquete* paqueterecv){
	t_paquete* paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(paqueteRead->datos);
	char* buf = malloc(archivo->file_size);
	int size = leer_archivo(paqueteRead->datos, paqueteRead->codigo, paqueteRead->tamanio,buf);
	enviarQueSos(newfd, empaquetar(6,buf,size));
}

void escribir (int newfd, t_paquete* paqueterecv){
	t_paquete* paqueteWrite = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
	char** bufonuevo= string_split(paqueteWrite->datos,"|");
//				char* bufpath = malloc(paqueterecv->tamanio - size_header - paqueteRead->tamanio);
//				char* bufbuf = malloc(paqueterecv->tamanio);
//				memcpy(bufpath, paqueteRead->datos, paqueterecv->tamanio -size_header - paqueteRead->tamanio);
//				memcpy(bufbuf,paqueteRead->datos + strlen(bufpath), paqueteRead->tamanio);
	int tam = escribir_archivo(bufonuevo[0],paqueteWrite->codigo,paqueteWrite->tamanio,bufonuevo[1]);
	enviarQueSos(newfd, empaquetar(7,bufonuevo[1],tam));
}

void remover (int newfd, t_paquete* paqueterecv){
	osada_file* archivo;
	archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo->state == 1) {
		borrar_archivo(paqueterecv->datos);
		enviarQueSos(newfd, empaquetar(99,"ok",3));
	}
	else if(archivo->state == 2) {
		borrar_directorio_vacio(paqueterecv->datos);
		enviarQueSos(newfd, empaquetar(99,"ok",3));
	}
	if(archivo == NULL || archivo->state == 0) {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	}
}

void truncar(int newfd, t_paquete* paqueterecv){
	osada_file* archivo;
	t_paquete* paqueteRead;
	paqueteRead = desacoplador1(paqueterecv->datos,paqueterecv->tamanio);
	archivo = obtenerArchivo(paqueterecv->datos);
	if(archivo == NULL) {
		enviarQueSos(newfd, empaquetar(100,"error",6));
	} else {
		truncar_archivo(archivo,(uint32_t)paqueteRead->tamanio);
		enviarQueSos(newfd, empaquetar(99,"ok",3));
	}
}

void renombrar (int newfd, t_paquete* paqueterecv){
	char* buforecibido;
	buforecibido= malloc(paqueterecv->tamanio);
	buforecibido= paqueterecv->datos;
	char** bufonuevox= string_split(buforecibido,"%");
	renombrar_archivo(bufonuevox[0],bufonuevox[1]);
	enviarQueSos(newfd, empaquetar(99,"ok",3));
}

void linkear (int newfd, t_paquete* paqueterecv){
	char* buforecibido;
	buforecibido= malloc(paqueterecv->tamanio);
	buforecibido= paqueterecv->datos;
	char** bufonuevox= string_split(buforecibido,"%");
	copiar_archivo(bufonuevox[0],bufonuevox[1]);
	enviarQueSos(newfd, empaquetar(99,"ok",3));
}

void utimens (int newfd) {
	enviarQueSos(newfd, empaquetar(99,"ok",3));
}

void recibirQueSos(int newfd){
	char buffer[MAX_BUFFERSIZE];
	int sizebytes;
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
				getattr(newfd, paqueterecv);
				break;
			case 2:
				readdir(newfd, paqueterecv);
				break;
			case 3:
				hacerdir(newfd, paqueterecv);
				break;
			case 4:
				crear(newfd, paqueterecv);
				break;
			case 5:
				abrir(newfd, paqueterecv);
				break;
			case 6:
				leer(newfd, paqueterecv);
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
				escribir(newfd, paqueterecv);
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
				remover(newfd, paqueterecv);
				break;
			case 9:
				utimens(newfd);
				break;
			case 10:
				truncar(newfd, paqueterecv);
				break;
			case 11:
				renombrar(newfd, paqueterecv);
				break;
			case 12:
				linkear(newfd, paqueterecv);
				break;
			}
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
