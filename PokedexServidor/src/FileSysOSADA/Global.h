/*
 * Global.h
 *
 *  Created on: 20/9/2016
 *      Author: utnso
 */

#ifndef POKEDEXSERVIDOR_SRC_FILESYSOSADA_GLOBAL_H_
#define POKEDEXSERVIDOR_SRC_FILESYSOSADA_GLOBAL_H_

#include "osada.h"
#include <commons/bitarray.h>

char * disco;
osada_header* header;
t_bitarray* bitmap;
osada_file* tablaDeArchivos;
int* tablaDeAsignaciones;
int bloques;




#endif /* POKEDEXSERVIDOR_SRC_FILESYSOSADA_GLOBAL_H_ */
