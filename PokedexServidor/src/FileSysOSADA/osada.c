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
#define IS_ACTIVE_CONSOLE false
#define T_LOG_LEVEL LOG_LEVEL_INFO


t_log* logger;

int divisionMaxima(int numero, int otroNumero) {
	if(numero % 64 == 0) {
		return numero/otroNumero;
	}
	else {
		return numero/otroNumero + 1;
	}
}

void reconocerOSADA(void) {

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
	bloquesDeDatos = (osada_block*) (disco + (header->fs_blocks - header->data_blocks)*OSADA_BLOCK_SIZE);
}

int dondeEmpezarLectura(int offset) {
	int resultado = offset / OSADA_BLOCK_SIZE;
	return resultado;
}

int offsetDondeEmpezar(int offset) {
	int resultado = offset % OSADA_BLOCK_SIZE;
	return resultado;
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

int minimoEntre(int unNro, int otroNro)
{
	if(unNro<otroNro)
	{
		return unNro;
	}
	return otroNro;
}

int maximoEntre(int unNro, int otroNro)
{
	if(unNro>otroNro)
	{
		return unNro;
	}
	return otroNro;
}

int prepararLista(t_list* lista, int numero) {
	list_add(lista,numero);
	while(tablaDeAsignaciones[numero]!= -1) {
		list_add(lista,tablaDeAsignaciones[numero]);
		numero = tablaDeAsignaciones[numero];
	}
	return 0;
}

int copiarInformacion(int tamanioACopiar, int offset,char* buffer, char* inicio ,osada_file* archivo)
{
	t_list* listaDeBloques = list_create();
	prepararLista(listaDeBloques,archivo->first_block);
	int i=1;
	int copiado = 0;
	int restanteDeMiBloque = OSADA_BLOCK_SIZE - offsetDondeEmpezar(offset);
	while(copiado < tamanioACopiar)
	{
		int tamanioACopiarDentroDelBloque = minimoEntre(tamanioACopiar,restanteDeMiBloque);
		restanteDeMiBloque = OSADA_BLOCK_SIZE;
		//QUE PASA ACA?
		memcpy(buffer + copiado,inicio,tamanioACopiarDentroDelBloque);
		copiado = copiado + tamanioACopiarDentroDelBloque;
		if(copiado < tamanioACopiar)
		{
			inicio = list_get(listaDeBloques, i);
			i++;
		}
	}
	return copiado;
}

//leer archivo incompleto
int leer_archivo(char* path, int offset, int tamanioALeer, char* buffer) {
	osada_file* archivo = obtenerArchivo(path);
	if (archivo == NULL) {
		return -1;
	}
	int* leido = malloc(sizeof(int));
	int tamanioALeerVerdadero = minimoEntre(tamanioALeer,archivo->file_size - offset);
	//empiezo
	char* bloqueDeDatos = (char*) bloquesDeDatos[archivo->first_block];
	*leido = copiarInformacion(tamanioALeerVerdadero, 0,buffer,bloqueDeDatos,archivo);
	if(*leido != tamanioALeerVerdadero)
	{
		return -1;
	}
	return leido;
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
	char* buffer = malloc(tablaDeArchivos[1].file_size);
	int* error = malloc(sizeof(int));
	leer_archivo("/directorio/archivo.txt", 0, tablaDeArchivos[1].file_size,buffer);
	printf("%s\n",buffer);
	return 0;

}
