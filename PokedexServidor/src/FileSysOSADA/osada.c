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
#include <errno.h>


#define LOG_FILE "osada.log"
#define PROGRAM_NAME "Pokedex Servidor"
#define PROGRAM_DESCRIPTION "Proceso File System"
#define IS_ACTIVE_CONSOLE true
#define T_LOG_LEVEL LOG_LEVEL_INFO


t_log* logger;
pthread_mutex_t semaforoBitmap, semaforoTablaDeNodos;

int divisionMaxima(int numero) {
	if(numero % 64 == 0) {
		return numero/64;
	}
	else {
		return numero/64 + 1;
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

int minimoEntre(int unNro, int otroNro) {
	if(unNro<otroNro)
	{
		return unNro;
	}
	return otroNro;
}

int maximoEntre(int unNro, int otroNro) {
	if(unNro>otroNro)
	{
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

//borrar directorio vacio
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
	//este bloque de datos tiene que ser el del offset(deuda)
	char* bloqueDeDatos = (char*) bloquesDeDatos[archivo->first_block];
	*leido = copiarInformacion(tamanioALeerVerdadero, offset,buffer,bloqueDeDatos,archivo);
	pthread_mutex_unlock(&semaforoTablaDeNodos);
	if(*leido != tamanioALeerVerdadero)
	{
		return -1;
	}
	return *leido;
}

int buscarArchivoVacio() {
	int i = 0;
	while(i < 2048) {
		if (tablaDeArchivos[i].state == DELETED) {
			 return i;
		}
		i++;
	}
	return -1;
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

void borrarDelBitmap(int punteroAEliminar) {

	int j = inicioDeBloqueDeDatos;
	bitarray_clean_bit(bitmap,j + punteroAEliminar);
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
	int posicionEnLaTabla = buscarArchivoVacio();
	osada_file* archivoNuevo = &tablaDeArchivos[posicionEnLaTabla];
	if(archivoNuevo == NULL)
	{
		pthread_mutex_unlock(&semaforoTablaDeNodos);
		//tabla de archivos lleno
		return -1;
	}
	strcpy(archivoNuevo->fname,adquirirNombre(path));
	archivoNuevo->parent_directory = buscarArchivoDelPadre(path);
	archivoNuevo->file_size = 0;
	archivoNuevo->lastmod = time(NULL);

	if(direcOArch == 1) {
		archivoNuevo->state = REGULAR;
	}
	else {
		archivoNuevo->state = DIRECTORY;
	}
	archivoNuevo->first_block = NULL;
	pthread_mutex_unlock(&semaforoTablaDeNodos);
	return posicionEnLaTabla;
}

void liberarBloquesDeBitmap(osada_file* archivo) {
	int size = archivo->file_size;
	int puntero = archivo->first_block;
	int tamanioLeido = 0;
	while(tamanioLeido<size) {
		tamanioLeido = OSADA_BLOCK_SIZE;
		if(tamanioLeido + OSADA_BLOCK_SIZE > size) {tamanioLeido = size;}
		borrarDelBitmap(puntero);
		puntero = tablaDeAsignaciones[puntero];
	}
}

int borrar_archivo(char* path) {

	osada_file* archivo = obtenerArchivo(path);

	if(archivo->state == 0 || archivo == NULL) {
		//archivo ya borrado o no encontrado en la tabla(ya superpuesto)
		return -1;
	}

	liberarBloquesDeBitmap(archivo);
	archivo->state = 0;

	return 0;
}

int renombrar_archivo(char* pathViejo, char* pathNuevo) {
	osada_file* archivo = obtenerArchivo(pathViejo);
	strcpy(archivo->fname,adquirirNombre(pathNuevo));
	archivo->parent_directory = buscarArchivoDelPadre(pathNuevo);
	archivo->lastmod = time(NULL);
	return 0;
}

//primero escribir
int copiar_archivo(char* pathViejo, char*pathNuevo) {
	osada_file* archivoACopiar = obtenerArchivo(pathViejo);
	int posicionEnLaTabla = crear_archivo(pathNuevo,archivoACopiar->state);


}

int ejemplo_getattr(const char *path, struct stat *st) {
	int res=0;
	memset (st,0,sizeof(st));

	    if(S_ISDIR(st->st_mode)){
				st->st_mode = S_IFDIR | 0755;
	    		st->st_uid = getuid();
	    		st->st_gid = getgid();
	    		st->st_atime = time(NULL);
	    		st->st_mtime = time(NULL);
	    		st->st_nlink = 2;
	        }
	    else if(S_ISREG(st->st_mode)){
	    		st->st_mode = S_IFREG | 0644;
	    		st->st_uid = getuid();
	    		st->st_gid = getgid();
	    		st->st_size = sizeof(st);
	    		st->st_atime = time(NULL);
	    		st->st_mtime = time(NULL);
	        }
	    else {
	    	res = -ENOENT;
	    }

	    return res;
}

/////////////////////////////////////escribir archivo /////////////////////////////////////

int buscarLibresYOcuparEnbitmap() {
	int i=0;
	while(i < cantidadDeBloques) {
		if(bitarray_test_bit(bitmap,i)==false ) {
			bitarray_set_bit(bitmap,i);
			return i;
		}
		i++;
	}
	//no hay espacio libre
	return -1;
}

int agregarTablaDeAsignaciones(uint32_t bloqueNuevo, osada_file* archivo) {
	char* bloqueDeDatos = (char*) bloquesDeDatos[archivo->first_block];
	int* numeroDeTabla = malloc(sizeof(int));
	*numeroDeTabla = tablaDeAsignaciones[archivo->first_block];
	int i=1;
	int leido = 0;
	int tamanioACopiarDentroDelBloque = OSADA_BLOCK_SIZE;
	while(leido < archivo->file_size)
	{
		if(leido + 64 > archivo->file_size) {tamanioACopiarDentroDelBloque = archivo->file_size - leido;}
		leido = leido + tamanioACopiarDentroDelBloque;
		if(leido < archivo->file_size)
		{
			bloqueDeDatos = (char*)bloquesDeDatos[*numeroDeTabla];
			*numeroDeTabla = tablaDeAsignaciones[*numeroDeTabla];
			i++;
		}
	}
	//ya lei toda la tabla de asignaciones
	tablaDeAsignaciones[*numeroDeTabla] = bloqueNuevo;
	free(numeroDeTabla);
	return 1;
}

int agregarBloquesDelBitmap(int bloquesAAgregar, osada_file* archivo)
{
	int i;
	for (i = 0; i < bloquesAAgregar ; i ++)
	{
		uint32_t bloqueNuevo = buscarLibresYOcuparEnbitmap();
		if(bloqueNuevo == -1)
		{
			//no hubo mas espacio en el bitmap se agrega tdo lo qe se puede
			archivo->file_size += i * OSADA_BLOCK_SIZE;
			return -1;
		}
		agregarTablaDeAsignaciones(bloqueNuevo, archivo);
	}

	return 0;

}

osada_file* truncar_archivo(osada_file* archivo, uint32_t size)
{
	int diferenciaDeTamanios = abs(size - archivo->file_size);//Lo que tengo que agregar o quitar

	if(archivo->file_size < size)//Tengo que agregar bloques
	{

		int bloquesAAgregar = divisionMaxima(size) - divisionMaxima(archivo);
		pthread_mutex_lock(&semaforoBitmap);
		if(agregarBloquesDelBitmap(bloquesAAgregar, archivo) != 0)
		{
			pthread_mutex_unlock(&semaforoBitmap);
			return NULL;
		}
		agregarBloquesDelBitmap(diferenciaDeTamanios,archivo);
		pthread_mutex_unlock(&semaforoBitmap);

	}
	if(archivo->file_size > size)//Tengo que quitar bloques
	{
		if(archivo->file_size % OSADA_BLOCK_SIZE < abs(diferenciaDeTamanios))
		{
			int bloquesARestar = divisionMaxima(archivo->file_size) - divisionMaxima(size);
			pthread_mutex_lock(&semaforoBitmap);
			//TERMINAR CON EL BORRADO DE BITMAP ES LO ULTIMO TODO
			quitarBloquesDelBitmap(bloquesARestar, archivo, size);
			pthread_mutex_unlock(&semaforoBitmap);
		}//Si el tamaño ocupado es mayor a lo que quiero quitar, no hace falta sacar bloques
	}
	//Va a ponerle de tamaño a mi archivo lo pasado en size. Incluyendo el agregado o borrado de bloques
	archivo->file_size = size;
	return archivo;
}


int escribir_informacion(int tamanioAEscribir, int offset, char* bufferConDatos, char* inicioDeEscritura, osada_file* archivo ) {
	int* numeroDeTabla = malloc(sizeof(int));
	*numeroDeTabla = tablaDeAsignaciones[archivo->first_block];
	int escrito = 0;
	int i=0;
	int tamanioRestanteAEscribir = tamanioAEscribir;
	int restanteDeMiBloque = OSADA_BLOCK_SIZE - offsetDondeEmpezar(offset);
	while(escrito < tamanioAEscribir) {
		int tamanioAEScribirDentroDelBloque = minimoEntre(tamanioRestanteAEscribir,restanteDeMiBloque);
		memcpy(inicioDeEscritura,bufferConDatos+escrito,tamanioAEScribirDentroDelBloque);
		restanteDeMiBloque = OSADA_BLOCK_SIZE;
		escrito = escrito + tamanioAEScribirDentroDelBloque;
		tamanioRestanteAEscribir -= tamanioAEScribirDentroDelBloque;
		if(escrito < tamanioAEscribir) {
			inicioDeEscritura = (char*)bloquesDeDatos[*numeroDeTabla];
			*numeroDeTabla = tablaDeAsignaciones[*numeroDeTabla];
			i++;
		}
	}
	if(escrito == tamanioAEscribir) {
		return escrito;
	}
	else {
		return -ENOENT;
	}
}


int escribir_archivo(char* path, int offset, char* bufferConDatos, int tamanioAEscribir) {
	osada_file* archivo = obtenerArchivo(path);
	int* escrito = malloc(sizeof(int));

	if(archivo == NULL) {
			return -ENOENT;
		}

	archivo = truncar_archivo(archivo,maximoEntre(offset+tamanioAEscribir,archivo->file_size));

	if(archivo==NULL) {
		return -ENOENT;
	}

	pthread_mutex_lock(&semaforoTablaDeNodos);
	char* inicioDeEscritura = (char*) bloquesDeDatos[archivo->first_block];
	*escrito = escribir_informacion(tamanioAEscribir, offset, bufferConDatos, inicioDeEscritura,archivo);
	pthread_mutex_unlock(&semaforoTablaDeNodos);

	return escrito;
}

//tener cuidado con manejo de errores
int main () {
	reconocerOSADA();
	char* buffer = malloc(tablaDeArchivos[3].file_size);
	leer_archivo("/directorio/subdirectorio/large.txt", 0, tablaDeArchivos[3].file_size,buffer);
//	crear_archivo("/directorio/Mapa",2);
//	crear_archivo("/directorio/Mapa/Medalla.jpg",1);
//	crear_archivo("/directorio/Entrenador",2);
	log_info(logger,"\n%s",buffer);
	log_info(logger,"\n%s",tablaDeArchivos[3].fname);
//	log_info(logger,"\n%s",tablaDeArchivos[5].fname);
//	log_info(logger,"\n%s",tablaDeArchivos[tablaDeArchivos[5].parent_directory].fname);
//	log_info(logger,"\n%s",tablaDeArchivos[4].fname);
//	log_info(logger,"\n%s",tablaDeArchivos[6].fname);
//	renombrar_archivo("/directorio/Mapa/Medalla.jpg", "/directorio/Entrenador/Medalla.jpg");
//	copiar_archivo("/directorio/Mapa/Medalla.jpg", "/directorio/Entrenador/Medalla.jpg");
	log_info(logger,"\n%s",tablaDeArchivos[5].fname);
	log_info(logger,"\n%s",tablaDeArchivos[tablaDeArchivos[5].parent_directory].fname);
	log_info(logger,"\n%s",tablaDeArchivos[tablaDeArchivos[7].parent_directory].fname);
	log_info(logger,"\n%s",tablaDeArchivos[7].fname);
	return 0;


}
