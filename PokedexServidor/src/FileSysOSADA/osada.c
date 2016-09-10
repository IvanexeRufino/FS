#include "osada.h"
#include <stdio.h>
#include <stdlib.h>


int main (void) {

	FILE *archivo;
	osada_header *header;

	archivo = fopen("disco.bin","r");
	fread(header,sizeof(osada_header),1,archivo);
	printf("Identificador (bytes): %s\n",header->magic_number);
	printf("Version: %d\n",header->version);
	printf("Tamaño FS: %d\n",header->fs_blocks);
	printf("Tamaño Bitmap: %d\n",header->bitmap_blocks);
	printf("Inicio tabla asignaciones: %d\n",header->allocations_table_offset);
	printf("Tamaño datos: %d\n",header->data_blocks);
	printf("Relleno: %d\n\n",header->padding);

	fclose(archivo);

	return EXIT_SUCCESS;

}

