#include "osada.h"
#include <stdio.h>
#include <stdlib.h>

#define BLOQUES_TOTAL_FS (100*1024*1024/OSADA_BLOCK_SIZE)
#define HEADER 1
#define BITMAP (BLOQUES_TOTAL_FS/8/OSADA_BLOCK_SIZE)
#define TABLA_ARCHI 1024
#define FAT ((BLOQUES_TOTAL_FS-1-BITMAP-TABLA_ARCHI)*4/64)
#define DATOS (BLOQUES_TOTAL_FS-HEADER-TABLA_ARCHI-FAT)


int main (void) {

	FILE *archivo;
	osada_header *header;

	archivo = fopen("disco.bin","r");
	fread(header,sizeof(osada_header),1,archivo);
	printf("El identificador ocupa (bytes): %d\n",sizeof(header->magic_number));
	printf("La version ocupa (bytes): %d\n",sizeof(header->version));
	printf("El tamaño del FS ocupa (bytes): %d\n",sizeof(header->fs_blocks));
	printf("El tamaño del bitmap ocupa (bytes): %d\n",sizeof(header->bitmap_blocks));
	printf("El inicio de la tabla de asignaciones ocupa (bytes): %d\n",sizeof(header->allocations_table_offset));
	printf("El tamaño de datos ocupa (bytes): %d\n",sizeof(header->data_blocks));
	printf("El relleno ocupa (bytes): %d\n\n",sizeof(header->padding));

	printf("Usando un disco de 100MB... \n\n");

	printf("Bloques totales FS: %d\n", BLOQUES_TOTAL_FS);
	printf("Bloques header: %d\n", HEADER);
	printf("Bloques bitmap: %d\n", BITMAP);
	printf("Bloques tabla archivos: %d\n",TABLA_ARCHI);
	printf("Inicio de la tabla de asignaciones (bloque): %d\n", (1+BITMAP+1024));
	printf("Bloques relleno: %d\n",FAT);
	printf("Bloques datos: %d\n",DATOS);

	fclose(archivo);

	return EXIT_SUCCESS;

}

