#include <stdint.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <commons/string.h>

#ifndef __OSADA_H__
#define __OSADA_H__

#define OSADA_BLOCK_SIZE 64
#define OSADA_FILENAME_LENGTH 17

typedef unsigned char osada_block[OSADA_BLOCK_SIZE];
typedef uint32_t osada_block_pointer;

// set __attribute__((packed)) for this whole section
// See http://stackoverflow.com/a/11772340/641451
#pragma pack(push, 1)

typedef struct {
	unsigned char magic_number[7]; // OSADAFS
	uint8_t version;
	uint32_t fs_blocks; // total amount of blocks
	uint32_t bitmap_blocks; // bitmap size in blocks
	uint32_t allocations_table_offset; // allocations table's first block number
	uint32_t data_blocks; // amount of data blocks
	unsigned char padding[40]; // useless bytes just to complete the block size
} osada_header;

_Static_assert( sizeof(osada_header) == sizeof(osada_block), "osada_header size does not match osada_block size");

typedef enum __attribute__((packed)) {
    DELETED = '\0',
    REGULAR = '\1',
    DIRECTORY = '\2',
} osada_file_state;

_Static_assert( sizeof(osada_file_state) == 1, "osada_file_state is not a char type");

typedef struct {
	osada_file_state state;
	unsigned char fname[OSADA_FILENAME_LENGTH];
	uint16_t parent_directory;
	uint32_t file_size;
	uint32_t lastmod;
	osada_block_pointer first_block;
} osada_file;

_Static_assert( sizeof(osada_file) == (sizeof(osada_block) / 2.0), "osada_file size does not half osada_block size");

void reconocerOSADA(char* path);

int buscarIndiceConPadre(char* nombreABuscar, int padre);

int obtenerIndice(char* path);

osada_file* obtenerArchivo(char* path);

t_list* listaDeHijosDelArchivo(int indiceDelPadre);

int borrar_directorio_vacio(char* path);

int leer_archivo(char* path, int offset, int tamanioALeer, char* buffer);

int buscarArchivoVacio();

char* adquirirNombreAnterior(char* path);

char* adquirirNombre(char* path);

uint32_t buscarArchivoDelPadre(char* path);

int crear_archivo(char* path, int direcOArch);

int borrar_archivo(char* path);

int renombrar_archivo(char* pathViejo, char* pathNuevo);

osada_file* truncar_archivo(osada_file* archivo, uint32_t size);

int escribir_archivo(char* path, int offset, int tamanioAEscribir, char* bufferConDatos);

int copiar_archivo(char* pathFuente, char*pathDestino);

#pragma pack(pop)

#endif __OSADA_H__
