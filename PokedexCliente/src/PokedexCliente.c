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
#include "FileSysOSADA/osada.h"

static int ejemplo_getattr(const char *path, struct stat *stbuf) {
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
	crear_archivo(filename,2);
	return 0;
}

static int ejemplo_create (const char* path, mode_t modo, struct fuse_file_info * info) {
	crear_archivo(path,1);
	return 0;
}

static int ejemplo_open(const char * path, int info) {
	osada_file* archivo = obtenerArchivo (path);
	if(archivo == NULL || archivo->state == 0) {
		return -ENOENT;
	}
	return 0;
}

static int ejemplo_read(char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	return leer_archivo(path,offset,size,buf);
}


static int ejemplo_write ( char *path,  char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	return escribir_archivo(path,offset,size,buf);
}

static int ejemplo_remove (char* path) {
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
	return 0;
}

static int ejemplo_truncate(char* path, off_t size) {

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
	renombrar_archivo(nombreViejo,nombreNuevo);
	return 0;
}

static int ejemplo_link (const char *archivoOrigen, const char *archivoDestino){
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


int main(int argc, char *argv[]) {

	system("truncate -s 100k disco.bin");
	system("./osada-format /home/utnso/disco.bin");
	reconocerOSADA("/home/utnso/disco.bin");

	return fuse_main(argc, argv, &ejemplo_oper, NULL );

}
