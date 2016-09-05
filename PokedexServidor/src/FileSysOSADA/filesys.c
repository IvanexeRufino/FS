/*
 * filesys.c
 *
 *  Created on: 5/9/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <commons/bitarray.h>

typedef struct {
	int identificador[7];
	int version[1];
	int tamanioFS[4]; //T/BLOCK_SIZE//
	int tamanioBM[4]; //N = T/8/BLOCK_SIZE//
	int* inicioTA[4]; //1+N+1024//
	int tamanioDatos[4];//F-1-N-1024-A//
	int relleno[40];
} header;

typedef struct {
	t_bitarray bitmap; //N = F/8/BLOCKSIZE//
} bitArray;

typedef struct {
	int estado[1];
	char nombreArchivo[17];
	int* bloquePadre[2];
	int tamanioArchivo[4];
	time_t fechaUltimaModif[4];
	int* bloqueInicial[4];
} osadaFile;

typedef struct {
	struct osadaFile archivos[2048];
} tablaArchivos;

typedef struct {
	int asignaciones []; //A = (F-1-N-1024)*4/BLOCKSIZE//
} tablaAsignaciones;

typedef struct {
	int bloques []; //F-1-N-1024-A//
} bloquesDeDatos;

int main(void) {
	return EXIT_SUCCESS;
}

