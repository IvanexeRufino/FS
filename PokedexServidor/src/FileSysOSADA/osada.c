#include "osada.h"
#include "Global.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <commons/log.h>
#include <sys/mman.h>
#include <string.h>
#include <commons/collections/list.h>
#include <inttypes.h>


#define LOG_FILE "osada.log"
#define PROGRAM_NAME "Pokedex Servidor"
#define PROGRAM_DESCRIPTION "Proceso File System"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO

t_log* logger;
off_t inicioTablaDeArchivos;

void reconocerOSADA(void) {

	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	log_info(logger, PROGRAM_DESCRIPTION);

	int fd = open("/home/utnso/Descargas/basic.bin",O_RDWR);
	struct stat my_stat;

	if(fd == -1) {
		perror("open");
		exit(-1);
	}
	if(stat("/home/utnso/Descargas/basic.bin", &my_stat) < 0) {
		perror("fstat");
		close(fd);
		exit(-1);
	}
	disco =  mmap(NULL,my_stat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if(disco == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(-1);
	}
	close(fd);

	header = (osada_header*) &disco[0];
	tablaDeArchivos = (osada_file*)  (disco + (header->allocations_table_offset - 1024)*OSADA_BLOCK_SIZE);
	bitmap = bitarray_create(&disco[OSADA_BLOCK_SIZE],((my_stat.st_size / OSADA_BLOCK_SIZE) / 8));
	tablaDeAsignaciones = (int *) &disco[(header->allocations_table_offset)*OSADA_BLOCK_SIZE];

}


int buscarIndiceConPadre(char* nombreABuscar, int padre)
{
	int i;
	for(i = 0; i < 2048; i++)
	{
		if(strcmp(tablaDeArchivos[i].fname, nombreABuscar) == 0 && tablaDeArchivos[i].parent_directory == padre && tablaDeArchivos[i].state != DELETED)
		{
			return i;
		}
	}

	return -1;
}

int obtenerIndice(char* path) {

	char** arrayPath = string_split(path + 1,"/");
	int i = 0;
	int archivo = 65535;
	while(arrayPath[i] != NULL)
	{
		archivo = buscarIndiceConPadre(arrayPath[i], archivo);
		if (archivo == -1) {
			return -1;
		}
		i ++;
	}
	return archivo;
}

osada_file* obtenerArchivo(char* path) {

	int indice = obtenerIndice(path);

	if (indice == -1) {
		return NULL;
	}else {
		return &tablaDeArchivos[indice];
	}

}

//leer archivo incompleto
int leer_archivo(char* path) {
	osada_file* archivo = obtenerArchivo(path);

	if (archivo == NULL) {
		return -1;
	}


	return 0;
}

//ls
t_list* listaDeHijosDelArchivo(int indiceDelPadre)
{
	t_list* listaDeHijos = list_create();
	int i;
	for(i=0; i < 2048; i++)
	{
		osada_file* archivoHijo = &tablaDeArchivos[i];
		if(archivoHijo->parent_directory== indiceDelPadre && archivoHijo->state != 0)
		{
			list_add(listaDeHijos, archivoHijo);
		}
	}
	return listaDeHijos;
}

//borrar directorio vacio
int borrar_directorio(char* path)
{
	osada_file* archivo = obtenerArchivo(path);
	int indiceArchivo = obtenerIndice(path);
	t_list* listaDeHijos = listaDeHijosDelArchivo(indiceArchivo);
	if(archivo->state == 2 && list_is_empty(listaDeHijos))
	{
		//empezar a remover del fileSystem

	}
}

int main () {

	reconocerOSADA();
	int error = leer_archivo("/directorio/archivo.txt");
	return 0;

}
