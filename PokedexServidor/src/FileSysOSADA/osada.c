#include "osada.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>

#define LOG_FILE "osada.log"
#define PROGRAM_NAME "Pokedex Servidor"
#define PROGRAM_DESCRIPTION "Proceso File System"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO


int main (void) {

	FILE *archivo;
	osada_header *header;
	t_log* logger;
	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	log_info(logger, PROGRAM_DESCRIPTION);

	archivo = fopen("./osada-format disco.bin","r");
	fread(header,sizeof(osada_header),1,archivo);
	log_info(logger, printf("Identificador (bytes): %s\n",header->magic_number));
	log_info(logger, printf("Version: %d\n",header->version));
	log_info(logger, printf("Tamaño FS: %d\n",header->fs_blocks));
	log_info(logger, printf("Tamaño Bitmap: %d\n",header->bitmap_blocks));
	log_info(logger, printf("Inicio tabla asignaciones: %d\n",header->allocations_table_offset));
	log_info(logger, printf("Tamaño datos: %d\n",header->data_blocks));
	log_info(logger, printf("Relleno: %d\n\n",header->padding));

	fclose(archivo);

	return EXIT_SUCCESS;

}

