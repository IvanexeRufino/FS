/*
 * ejemplo.c
 *
 *  Created on: 26/08/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <fuse.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>


#define IP "127.0.0.1"
#define PUERTO "9999"
#define PACKAGESIZE 1024

#include "FileSysOSADA/osada.h"

static int ejemplo_getattr(const char *path, struct stat *stbuf) {
	enviarQueSos("1",path);
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));

	osada_file* archivo = obtenerArchivo(path);

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if(archivo == NULL || archivo->state == 0) {
		return -ENOENT;
	} else if (archivo->state == 2) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} 	else {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = archivo->file_size;
	}
	return res;
}

static int ejemplo_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {
	enviarQueSos("2",path);
	int res = 0;
	int i;
	int indice = obtenerIndice(path);
	t_list* listaDeHijos = listaDeHijosDelArchivo(indice);
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	for(i = 0; i <list_size(listaDeHijos); i++) {
		osada_file* archivoHijo = list_get(listaDeHijos,i);
		filler(buf, archivoHijo->fname, NULL, 0);
	}

	return res;
}

static int ejemplo_mkdir(const char* filename, mode_t modo){
//	enviarQueSos("3", NULL);
	crear_archivo(filename,2);
	return 0;
}

static int ejemplo_create (const char* path, mode_t modo, struct fuse_file_info * info) {
//	enviarQueSos("4", path);
	crear_archivo(path,1);
	return 0;
}

static int ejemplo_open(const char * path, int info) {
//	enviarQueSos("5",path);
	osada_file* archivo = obtenerArchivo (path);
	if(archivo == NULL || archivo->state == 0) {
		return -ENOENT;
	}
	return 0;
}

static int ejemplo_read(char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
//	enviarQueSos("6",path);
	return leer_archivo(path,offset,size,buf);
}


static int ejemplo_write ( char *path,  char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
//	enviarQueSos("7",path);
	return escribir_archivo(path,offset,size,buf);
}

static int ejemplo_remove (char* path) {
//	enviarQueSos("8", path);
	osada_file* archivo = obtenerArchivo(path);
	if (archivo == NULL) {
		return -ENOENT;
	}

	if(archivo->state == 1) {
		return borrar_archivo(path);
	}
	else if(archivo->state == 2) {
		return borrar_directorio_vacio(path);
	}
	else {
		return -ENOENT;
	}
}

static int ejemplo_utimens (const char * param1, const struct timespec tv[2] ){
//	enviarQueSos("9", NULL);
	return 0;
}

static int ejemplo_truncate(char* path, off_t size) {
//	enviarQueSos("10", path);
	osada_file* archivo = obtenerArchivo(path);
	if(archivo == NULL) {
		return -ENOENT;
	}
	else {
		truncar_archivo(archivo,(uint32_t)size);
		return 0;
	}

}

static int ejemplo_rename(const char *nombreViejo, const char *nombreNuevo){
//	enviarQueSos("11", NULL);
	renombrar_archivo(nombreViejo,nombreNuevo);
	return 0;
}

static int ejemplo_link (const char *archivoOrigen, const char *archivoDestino){
//	enviarQueSos("12", NULL);
	copiar_archivo(archivoOrigen, archivoDestino);
	return 0;
}

static struct fuse_operations ejemplo_oper = {
		.getattr = ejemplo_getattr,
		.readdir = ejemplo_readdir,
		.open = ejemplo_open,
		.read = ejemplo_read,
		.create = ejemplo_create,
		.unlink = ejemplo_remove,
		.mkdir = ejemplo_mkdir,
		.write = ejemplo_write,
		.utimens = ejemplo_utimens,
		.rmdir = ejemplo_remove,
		.truncate = ejemplo_truncate,
		.rename = ejemplo_rename,
		.link = ejemplo_link,
};

int conectarConServer()
{
	struct sockaddr_in socket_info;
	int nuevoSocket;
	// Se carga informacion del socket
	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	socket_info.sin_port = htons(9999);

	// Crear un socket:
	// AF_INET, SOCK_STREM, 0
	nuevoSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (nuevoSocket < 0)
		return -1;
	// Conectar el socket con la direccion 'socketInfo'.
	int conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
	int mostrarEsperaAconectar=0;
	while (conecto != 0){
		mostrarEsperaAconectar++;
		if (mostrarEsperaAconectar == 1){
			printf("Esperando...\n");
		}
		conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
		printf("Conectado");
	}

	return nuevoSocket;
 };

void enviarQueSos(char* nroop, const char* path){
	char* bufo= malloc(sizeof(char*)+sizeof(const char*));
	int socket= conectarConServer();
	strcpy(bufo,nroop);
	strcat(bufo,path);
	send(socket,bufo,sizeof(bufo),0);
}


int main(int argc, char *argv[]) {

	system("truncate -s 100k disco.bin");
	system("./osada-format /home/utnso/disco.bin");
	reconocerOSADA("/home/utnso/disco.bin");

	return fuse_main(argc, argv, &ejemplo_oper, NULL );

}
