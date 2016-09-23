#include "osada.h"
#include "Global.h"
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

	int fd;
	struct stat my_stat;
	fd = open("/home/utnso/workspace/Pokedex Servidor/PokedexServidor/src/FileSysOSADA/disco.bin",O_RDWR);
	if(fd == -1) {
		perror("open");
		exit(-1);
	}
	if(fstat(fd, &my_stat) < 0) {
		perror("fstat");
		close(fd);
		exit(-1);
	}
	puntero =  mmap(0,my_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	if(puntero == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(-1);
	}
	close(fd);

	header = (osada_header*) &puntero[0];
	tablaDeArchivos = (osada_file*) (1 + (header->bitmap_blocks) * OSADA_BLOCK_SIZE);
	bitmap = bitarray_create(&puntero[OSADA_BLOCK_SIZE],((my_stat.st_size / OSADA_BLOCK_SIZE) / 8));



	log_info(logger, "Identificador (bytes): %s\n",header->magic_number);
	log_info(logger, "Version: %d\n",header->version);
	log_info(logger, "Tamaño FS: %d\n",header->fs_blocks);
	log_info(logger, "Tamaño Bitmap: %d\n",header->bitmap_blocks);
	log_info(logger, "Inicio tabla asignaciones: %d\n",header->allocations_table_offset);
	log_info(logger, "Tamaño datos: %d\n",header->data_blocks);
	log_info(logger, "Relleno: %d\n\n",header->padding);

	inicioTablaDeArchivos = (1 + header->bitmap_blocks)*2;

}

void cargarTablaDeArchivos() {

}

//ACA ESTOY

int buscarIndiceConPadre(char* nombreAbuscar, int padre)
{
	int i;
	for(i = 0; i < 2048; i++)
	{
		if(strcmp(tablaDeArchivos[i].fname,nombreAbuscar) == 0 && tablaDeArchivos[i].parent_directory == padre && tablaDeArchivos[i].state != 0)
		{
			return i;
		}
	}

	return -1;
}

int obtenerIndice(char* path) {
	int archivo = 0xFFFF;
	int i = -1;
	char* token;
	char** string;
	path = strdup(path);
	if (path != NULL) {
	  while ((token = strsep(&path, "/")) != NULL)
	  {
		if(i == -1){}
		else {
		string[i] = token;
		}
		i++;
	  }
	}
	i=0;
	while(string[i] != NULL)
	{
			archivo = buscarIndiceConPadre(string[i], archivo);
			if (archivo == -1) {
				return -1;
			}
			i ++;
	}
		return archivo ;
}

osada_file*  obtenerArchivo(char* path) {

	int indice = obtenerIndice(path);

	if (indice == -1) {
		return NULL;
	}else {
		return &tablaDeArchivos[indice];
	}

}

//leer archivo
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

	for(int i=0; i < 2048; i++)
	{
		osada_file* archivoHijo = &tablaDeArchivos[i];
		if(archivoHijo->parent_directory== indiceDelPadre && posibleHijo->state != 0)
		{
			list_add(listaDeHijos, archivoHijo);
		}
	}
	return listaDeHijos;
}

int main () {

	reconocerOSADA();
	cargarTablaDeArchivos();
	obtenerArchivo("/dir1/dir2/algo.jpg");
	return 0;

}
