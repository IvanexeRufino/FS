/*
 * PokedexServidor.c
 *
 *  Created on: 30/9/2016
 *      Author: utnso
 */

#include "pokedexservidor.h"
#include <pthread.h>

void ctrl_c(int nro){
	exit(1);
}

void* memoria(int cantidad) {
	void* puntero = NULL;
	if (cantidad !=0) {
		while (puntero == NULL) {
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
}

typedef struct {
	uint32_t codigo;
	uint32_t tamanio;
	uint32_t offset;
	uint32_t size;
}__attribute__((__packed__)) t_paquetePro ;

t_paquetePro* empaquetarPro(int codigo, int tamanio, int offset, int size){
	t_paquetePro* paquete= malloc(sizeof(t_paquetePro));
	paquete->codigo= codigo;
	paquete->tamanio= tamanio;
	paquete->offset = offset;
	paquete->size = size;
	return paquete;
}

void* acopladorPro(t_paquetePro* paquete) {
	void* paqueteSalida = memoria(size_header);
	memcpy(paqueteSalida, paquete, size_header);
	return paqueteSalida;
}

void* recibirNormal(int newfd, int size) {
	char buffer[size];
	if(recv(newfd, &buffer, size, MSG_WAITALL) <= 0) {
		//puts("ERROR RECIBIR");

		exit(1);
	}
	void* path = malloc(size);
	memcpy(path,buffer,size);
	return path;
}

t_paquetePro* desacopladorPro(char* buffer)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
  {
  	t_paquetePro* paquete;
  	paquete= memoria(sizeof(t_paquetePro));
  	memcpy(paquete,buffer,size_header);
  	return paquete;
  }

void enviarQueSos(int newfd, t_paquetePro* paqueteSend, void* buffer){
	void* enviar = acopladorPro(paqueteSend);
	if(send(newfd,enviar, size_header ,0)<0) {
				//puts("ERROR ENVIO");
				exit(1);
	}
	if(send(newfd,buffer, paqueteSend->tamanio ,0)<0) {
				//puts("ERROR ENVIO");
				exit(1);
	}
	free(paqueteSend);
	free(enviar);
}

void getattr(int newfd, t_paquetePro* paqueterecv) {
	printf("Se ejecuta GETATTR. \n");
	void* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(buffer);
	if(archivo == NULL || archivo->state == 0) {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
	else {
		enviarQueSos(newfd, empaquetarPro(1, sizeof(osada_file),0,0), archivo);
	}
}

void readdir(int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta READDIR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	int indice = obtenerIndice(buffer);
	t_list* listaDeHijos= listaDeHijosDelArchivo(indice);
	char* bufo= malloc(17*list_size(listaDeHijos));
	unsigned char* nombre = malloc(17);
	int i;
	for(i=0;i<list_size(listaDeHijos);i++){
		osada_file* archivoHijo= list_get(listaDeHijos,i);
		memcpy(nombre, archivoHijo->fname,17);
		strcat(nombre,"\0");
		strcat(bufo,nombre);
		strcat(bufo, "/");
	}

	if(list_size(listaDeHijos) != 0){
		enviarQueSos(newfd, empaquetarPro(2, strlen(bufo)+1,0,0), bufo);
	} else {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
}

void hacerdir (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta HACERDIR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	if(strlen(adquirirNombre(buffer)) <= 17) {
		int error = crear_archivo(buffer,2);
		if (error == -1) {
			enviarQueSos(newfd, empaquetarPro(101, 18,0,0), "Tabla de Archivos");
		} else {
			enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
		}
	}
	else {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
}

void abrir (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta ABRIR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(buffer);
	if(archivo == NULL || archivo->state == 0) {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
	else {
		enviarQueSos(newfd, empaquetarPro(1, sizeof(osada_file),0,0), archivo);
	}
}

void crear (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta CREAR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	printf("%s \n", buffer);
	if(strlen(adquirirNombre(buffer)) <= 17) {
		int  error = crear_archivo(buffer,1);
		if (error == -1) {
			enviarQueSos(newfd, empaquetarPro(101, 18,0,0), "Tabla de Archivos");
		} else if (error == -2){
			enviarQueSos(newfd, empaquetarPro(105, 8,0,0), "espacio");
		} else {
			enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
		}
	}
	else {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
}

void leer (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta LEER. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(buffer);
	char* buf = malloc(minimoEntre(archivo->file_size, paqueterecv->size));
	int size = leer_archivo(buffer, paqueterecv->offset, minimoEntre(archivo->file_size,paqueterecv->size),buf);
	if(size != 0) {
		enviarQueSos(newfd, empaquetarPro(6, size,0,0), buf);
	} else {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
}

void escribir (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta ESCRIBIR. \n");
	char* path = recibirNormal(newfd, paqueterecv->tamanio);
	char* buffer = recibirNormal(newfd, paqueterecv->size);
	int tam = escribir_archivo(path,paqueterecv->offset, paqueterecv->size, buffer);
	if(tam == -1) {
		enviarQueSos(newfd,empaquetarPro(105,8,0,0),"espacio");
	} else if(tam != paqueterecv->size) {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	} else {
		osada_file* archivo = obtenerArchivo(path);
		archivo->lastmod = time(0);
		enviarQueSos(newfd, empaquetarPro(7, tam,0,0), buffer);
	}
}

void remover (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta REMOVER. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(buffer);
	if(archivo == NULL || archivo->state == DELETED) {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	}
	if(archivo->state == REGULAR) {
		borrar_archivo(buffer);
		enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
	}
	if(archivo->state == DIRECTORY) {
		borrar_directorio_vacio(buffer);
		enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
	}
}

void truncar(int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta TRUNCAR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(buffer);
	if(archivo == NULL) {
		enviarQueSos(newfd, empaquetarPro(100,6,0,0), "error");
	} else {
		truncar_archivo(archivo,(uint32_t)paqueterecv->size);
		enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
	}
}

void renombrar (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta RENOMBRAR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	char** bufonuevox= string_split(buffer,"%");
	renombrar_archivo(bufonuevox[0],bufonuevox[1]);
	enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
}

void linkear (int newfd, t_paquetePro* paqueterecv){
	printf("Se ejecuta LINKEAR. \n");
	char* buffer = recibirNormal(newfd, paqueterecv->tamanio);
	char** bufonuevox= string_split(buffer,"%");
	copiar_archivo(bufonuevox[0],bufonuevox[1]);
	enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
}

void utimens (int newfd, t_paquetePro* paqueterecv) {
	printf("Se ejecuta UTIMENS. \n");
	char* path = recibirNormal(newfd, paqueterecv->tamanio);
	osada_file* archivo = obtenerArchivo(path);
	archivo->lastmod = time(0);
	enviarQueSos(newfd, empaquetarPro(99, 3,0,0), "ok");
}

void wrapperRecibir(int newfd) {
	while(1) {
		recibirQueSos(newfd);
	}
}

void recibirQueSos(int newfd){

	char bufferHead[size_header];
	int sizebytes;
	if((sizebytes = recv(newfd, &bufferHead, size_header, MSG_WAITALL)) <= 0) {
		//puts("ERROR RECIBIR");
		exit(1);
	}

	t_paquetePro* paqueterecv = desacopladorPro(bufferHead);

	printf("Recibo un paquete de POKEDEX CLIENTE. \n");
	printf("Codigo: %d \n", paqueterecv->codigo);
	printf("Tamaño: %d \n", paqueterecv->tamanio);
	printf("Offset: %d \n", paqueterecv->offset);
	printf("Size: %d \n", paqueterecv->size);

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
				break;
			case 7:
				escribir(newfd,paqueterecv);
				break;
			case 8:
				remover(newfd, paqueterecv);
				break;
			case 9:
				utimens(newfd, paqueterecv);
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
//limpio 11505 used blocks - 152335 free - 0 padding bits
//2049 MAL 13595 used blocks - 150245 free - 0 padding bits
//13608 used blocks - 150232 free - 0 padding bits
int main(int argc, char *argv[]) {
//	system("truncate -s 200k disco.bin");
//	system("./osada-format disco.bin");
	reconocerOSADA(argv[1]);
//	reconocerOSADA("/home/utnso/disco.bin");

	int sockfd, new_fd;  // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del cliente
	int sin_size;
	struct sigaction sa;
	int yes=1;
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		//perror("socket");
		exit(1);
	    }
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		//perror("setsockopt");
		exit(1);
	}
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(PORT);     // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		//perror("bind");
		exit(1);
	}
		if (listen(sockfd, BACKLOG) == -1) {
		//perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		//perror("sigaction");
		exit(1);
	}
	pthread_t client_threadid;
	while(1) {

		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
			//perror("accept");
			continue;
		}
		printf("POKEDEX CLIENTE conectado desde la IP: %s",  inet_ntoa(their_addr.sin_addr));
		pthread_create(&client_threadid,NULL,(void*)wrapperRecibir,new_fd);

	}


	return 0;
}
