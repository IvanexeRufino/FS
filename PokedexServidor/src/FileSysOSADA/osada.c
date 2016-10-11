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
#include <pthread.h>


#define LOG_FILE "osada.log"
#define PROGRAM_NAME "Pokedex Servidor"
#define PROGRAM_DESCRIPTION "Proceso File System"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO


t_log* logger;
pthread_mutex_t semaforoBitmap, semaforoTablaDeNodos;

int divisionMaxima(int numero, int otroNumero) {
	if(numero % 64 == 0) {
		return numero/otroNumero;
	}
	else {
		return numero/otroNumero + 1;
	}
}

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
	bitmap = bitarray_create(&disco[OSADA_BLOCK_SIZE],(header->bitmap_blocks));
	tablaDeAsignaciones = (osada_block_pointer*) (disco + (header->allocations_table_offset) * OSADA_BLOCK_SIZE);
	inicioDeBloqueDeDatos = header->fs_blocks - header->data_blocks;
	bloquesDeDatos = (osada_block*) (disco + inicioDeBloqueDeDatos*OSADA_BLOCK_SIZE);
	cantidadDeBloques = header->fs_blocks;
	pthread_mutex_init (&semaforoBitmap,NULL);
	pthread_mutex_init (&semaforoTablaDeNodos,NULL);
	log_info(logger,"%d",header->bitmap_blocks);
	log_info(logger,"%d",header->fs_blocks);
	log_info(logger,"%d",header->data_blocks);
}

int dondeEmpezarLectura(int offset) {
	int resultado = offset / OSADA_BLOCK_SIZE;
	return resultado;
}

int offsetDondeEmpezar(int offset) {
	int resultado = offset % OSADA_BLOCK_SIZE;
	return resultado;
}

int buscarIndiceConPadre(char* nombreABuscar, int padre) {
	int i;
	for(i = 0; i < 2048; i++) {
		if(strcmp(tablaDeArchivos[i].fname, nombreABuscar) == 0 && tablaDeArchivos[i].parent_directory == padre && tablaDeArchivos[i].state != DELETED) {
			return i;
		}
	}
	return -1;
}


int obtenerIndice(char* path) {
	char** arrayPath = string_split(path + 1,"/");
	int i = 0;
	int archivo = 65535;
	while(arrayPath[i] != NULL) {
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

int minimoEntre(int unNro, int otroNro) {

	if(unNro<otroNro) {
		return unNro;
	}
	return otroNro;
}

int maximoEntre(int unNro, int otroNro) {

	if(unNro>otroNro) {
		return unNro;
	}
	return otroNro;
}

int copiarInformacion(int tamanioACopiar, int offset,char* buffer, char* inicio ,osada_file* archivo) {
	int* numeroDeTabla = malloc(sizeof(int));
	*numeroDeTabla = tablaDeAsignaciones[archivo->first_block];
	int i=1;
	int copiado = 0;
	int restanteDeMiBloque = OSADA_BLOCK_SIZE - offsetDondeEmpezar(offset);
	while(copiado < tamanioACopiar)
	{
 		int tamanioACopiarDentroDelBloque = minimoEntre(tamanioACopiar,restanteDeMiBloque);
		restanteDeMiBloque = OSADA_BLOCK_SIZE;
		if(copiado+64 > tamanioACopiar) {tamanioACopiarDentroDelBloque = tamanioACopiar-copiado;}
		memcpy(buffer + copiado,inicio,tamanioACopiarDentroDelBloque);
		copiado = copiado + tamanioACopiarDentroDelBloque;
		if(copiado < tamanioACopiar)
		{
			inicio = (char*)bloquesDeDatos[*numeroDeTabla];
			*numeroDeTabla = tablaDeAsignaciones[*numeroDeTabla];
			i++;
		}
	}
	free(numeroDeTabla);
	return copiado;
}

//ls
t_list* listaDeHijosDelArchivo(int indiceDelPadre) {
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

int borrar_directorio_vacio(char* path) {

	osada_file* archivo = obtenerArchivo(path);
	int indiceArchivo = obtenerIndice(path);
	t_list* listaDeHijos = listaDeHijosDelArchivo(indiceArchivo);
	if(archivo->state == DIRECTORY && list_is_empty(listaDeHijos)) {
		archivo->state = DELETED;
	}
	return 0;
}

int leer_archivo(char* path, int offset, int tamanioALeer, char* buffer) {
	osada_file* archivo = obtenerArchivo(path);
	if (archivo == NULL) {
		return -1;
	}
	int* leido = malloc(sizeof(int));
	int tamanioALeerVerdadero = minimoEntre(tamanioALeer,archivo->file_size - offset);
	pthread_mutex_lock(&semaforoTablaDeNodos);
	char* bloqueDeDatos = (char*) bloquesDeDatos[archivo->first_block];
	*leido = copiarInformacion(tamanioALeerVerdadero, 0,buffer,bloqueDeDatos,archivo);
	pthread_mutex_unlock(&semaforoTablaDeNodos);
	if(*leido != tamanioALeerVerdadero)
	{
		return -1;
	}
	return *leido;
}

osada_file* buscarArchivoVacio() {
	int i = 0;
	while(i < 2048) {
		if (tablaDeArchivos[i].state == DELETED) {
			 return &tablaDeArchivos[i];
		}
		i++;
	}
	return NULL;
}

char* adquirirNombreAnterior(char* path)
{
	char* hijo = strrchr(path, '/');
	return string_substring_until(path, abs(path - hijo));
}

char* adquirirNombre(char* path)
{
	char** arrayPath = string_split(path + 1, "/");
	int i=0;
	while(arrayPath[i]!= NULL)
	{
		i++;
	}
	return string_duplicate(arrayPath[i - 1]);
}

uint32_t buscarArchivoDelPadre(char* path)
{
	char* nombreABuscar = adquirirNombreAnterior(path);
	int indice = obtenerIndice(nombreABuscar);
	free(nombreABuscar);
	return indice;
}

int buscarBloqueVacio() {
	int i = 0;
	int j = inicioDeBloqueDeDatos;
	while(j < cantidadDeBloques) {
		 if(bitarray_test_bit(bitmap,j) == false) {
				bitarray_set_bit(bitmap,j);
				return i;
		 }
		j++;
		i++;
	}
	//no hay bits vacios
	return -1;
}

int crear_archivo(char* path, int direcOArch)
{
	pthread_mutex_lock(&semaforoTablaDeNodos);
	osada_file* archivoNuevo = buscarArchivoVacio();
	if(archivoNuevo == NULL)
	{
		pthread_mutex_unlock(&semaforoTablaDeNodos);
		//tabla de archivos lleno
		return -1;
	}
	strcpy(archivoNuevo->fname,adquirirNombre(path));
	archivoNuevo->parent_directory = buscarArchivoDelPadre(path);
	archivoNuevo->file_size = 0;
	archivoNuevo->lastmod = 0000000;

	if(direcOArch == 1) {
		archivoNuevo->state = REGULAR;
		archivoNuevo->first_block = buscarBloqueVacio();
	}
	else {
		archivoNuevo->state = DIRECTORY;
	}

	pthread_mutex_unlock(&semaforoTablaDeNodos);
	return 0;
}

/////////////////////////////////////escribir archivo /////////////////////////////////////

int escribir_archivo(char* path, int offset, char* bufferConDatos, int tamanioAEscribir) {

}


//tener cuidado con manejo de errores
int main () {
	reconocerOSADA();
	char* buffer = malloc(tablaDeArchivos[3].file_size);
	leer_archivo("/directorio/subdirectorio/large.txt", 0, tablaDeArchivos[3].file_size,buffer);
	crear_archivo("/directorio/algo.txt", 1);
	log_info(logger,"\n%s",buffer);
	log_info(logger,"\n%s",bloquesDeDatos[1]);
	return 0;

}
