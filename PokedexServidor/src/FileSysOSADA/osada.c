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
 #include <commons/string.h>
 #include <commons/collections/list.h>
 #include <pthread.h>
 #include <errno.h>

 pthread_mutex_t semaforoBitmap, semaforoTablaDeArchivos, semaforoTablaDeAsignaciones;

 int posicionVerdaderaDelBloque(int numeroDeBloque) {

 }

 int divisionMaxima(int numero) {
 	if (numero == 0) {
 		return 1;
 	}
 	if(numero % 64 == 0) {
 		return numero/64;
 	}
 	else {
 		return numero/64 + 1;
 	}
 }

 void reconocerOSADA(char* path) {

 	int fd = open(path,O_RDWR);
 	struct stat my_stat;

 	if(fd == -1) {
 		perror("open");
 		exit(-1);
 	}
 	if(stat(path, &my_stat) < 0) {
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
  	inicioDeBloqueDeDatos = header->fs_blocks - header->data_blocks - 8;
  	bloquesDeDatos = (osada_block*) (disco + (header->fs_blocks - header->data_blocks)*OSADA_BLOCK_SIZE);
 	cantidadDeBloques = header->fs_blocks;
 	sobraDeBitmap = (header->fs_blocks - header->data_blocks) % 8;
 	pthread_mutex_init (&semaforoBitmap,NULL);
 	pthread_mutex_init (&semaforoTablaDeArchivos,NULL);
 	pthread_mutex_init (&semaforoTablaDeAsignaciones,NULL);
 }

 int dondeEmpezarLectura(int offset) {
 	if(offset == 0) {return 1;}
 	int resultado = offset / OSADA_BLOCK_SIZE;
 	return resultado;
 }

 int offsetDondeEmpezar(int offset) {
 	int resultado = offset % OSADA_BLOCK_SIZE;
 	return resultado;
  }

  int buscarIndiceConPadre(char* nombreABuscar, int padre) {
 	unsigned char* nombre = malloc(17);
  	int i;
  	for(i = 0; i < 2048; i++) {
		memcpy(nombre, tablaDeArchivos[i].fname,17);
		if(strcmp(nombre, nombreABuscar) == 0 && tablaDeArchivos[i].parent_directory == padre && tablaDeArchivos[i].state != DELETED) {
  			return i;
  		}
  	}
 	return -1;
 }


 int obtenerIndice(char* path) {
 	if(strcmp(path,"/") == 0){return 65535;}
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
 	} else {
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

 int recorrerTablaDeAsignaciones(osada_file* archivo, int numeroDeRecorridos) {
 	int numeroDeTabla = archivo->first_block;
 	int i;
 	for(i = 1; i < numeroDeRecorridos; i++) {
 		numeroDeTabla = tablaDeAsignaciones[numeroDeTabla];
 	}
 	return numeroDeTabla;
 }

 int copiarInformacion(int tamanioACopiar, int offset,char* buffer, char* inicio ,osada_file* archivo) {
 	int numeroDeTabla = recorrerTablaDeAsignaciones(archivo, divisionMaxima(offset + 1));
 	int i=1;
 	int copiado = 0;
 	int restanteDeMiBloque = OSADA_BLOCK_SIZE - offsetDondeEmpezar(offset);
 	while(copiado < tamanioACopiar) {
  		int tamanioACopiarDentroDelBloque = minimoEntre(tamanioACopiar,restanteDeMiBloque);
 		restanteDeMiBloque = OSADA_BLOCK_SIZE;
 		if(copiado+64 > tamanioACopiar) {tamanioACopiarDentroDelBloque = tamanioACopiar-copiado;}
 		memcpy(buffer + copiado,inicio,tamanioACopiarDentroDelBloque);
 		copiado = copiado + tamanioACopiarDentroDelBloque;
 		if(copiado < tamanioACopiar) {
 			numeroDeTabla = tablaDeAsignaciones[numeroDeTabla];
 			inicio = (char*)bloquesDeDatos[numeroDeTabla];
 			i++;
 		}
 	}
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
 	int tamanioALeerVerdadero = minimoEntre(tamanioALeer,archivo->file_size - offset);
 	pthread_mutex_lock(&semaforoTablaDeAsignaciones);
 	int numeroDeTabla = recorrerTablaDeAsignaciones(archivo, divisionMaxima(offset + 1));
 	char* bloqueDeDatos = (char*) bloquesDeDatos[numeroDeTabla] + offsetDondeEmpezar(offset);;
 	int leido = 0;
 	leido = copiarInformacion(tamanioALeerVerdadero, offset,buffer,bloqueDeDatos,archivo);
 	pthread_mutex_unlock(&semaforoTablaDeAsignaciones);
 	if(leido != tamanioALeerVerdadero)
 	{
 		return -1;
 	}
 	return leido;
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
 int buscarBloqueVacio() {
  	int i = 0;
  	int j = inicioDeBloqueDeDatos;
   	while(j < cantidadDeBloques) {
   		 if(bitarray_test_bit(bitmap,j) == false) {
   				bitarray_set_bit(bitmap,j);
   				return i - (8 - sobraDeBitmap);
   		 }
   		j++;
  		i++;
  	}
  	//no hay bits vacios
  	return -1;
  }

 int crear_archivo(char* path, int direcOArch)
 {
 	pthread_mutex_lock(&semaforoTablaDeArchivos);
 	int posicionEnLaTabla = buscarArchivoVacio();
 	if(posicionEnLaTabla == -1) {
 		return -1;
 	}
 	osada_file* archivoNuevo = &tablaDeArchivos[posicionEnLaTabla];
 	if(archivoNuevo == NULL)
 	{
 		pthread_mutex_unlock(&semaforoTablaDeArchivos);
 		//tabla de archivos lleno
 		return -1;
 	}
 	strcpy(archivoNuevo->fname,adquirirNombre(path));
 	archivoNuevo->parent_directory = buscarArchivoDelPadre(path);
 	archivoNuevo->file_size = 0;
 	archivoNuevo->lastmod = time(0);

 	if(direcOArch == 1) {
 		archivoNuevo->state = REGULAR;
 		pthread_mutex_lock(&semaforoBitmap);
 		int posDelBloque = buscarBloqueVacio();
 		if (posDelBloque != -1) {
 			archivoNuevo->first_block = buscarBloqueVacio();
 		} else {
 			archivoNuevo->first_block = -1;
 			return -2;
 		}
 		pthread_mutex_unlock(&semaforoBitmap);
 	}
 	else {
 		archivoNuevo->state = DIRECTORY;
 	}
 	pthread_mutex_unlock(&semaforoTablaDeArchivos);

 	return posicionEnLaTabla;
 }

 int borrar_archivo(char* path) {

 	osada_file* archivo = obtenerArchivo(path);

	pthread_mutex_lock(&semaforoBitmap);
	quitarBloquesDelBitmap(divisionMaxima(archivo->file_size), archivo, 0);
	pthread_mutex_unlock(&semaforoBitmap);

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

 /////////////////////////////////////escribir archivo /////////////////////////////////////

 int agregarTablaDeAsignaciones(uint32_t bloqueNuevo, osada_file* archivo, int numeroDeRecorridos) {

 	pthread_mutex_lock(&semaforoTablaDeAsignaciones);
 	int numeroDeTabla = recorrerTablaDeAsignaciones(archivo, numeroDeRecorridos);
 	tablaDeAsignaciones[numeroDeTabla] = bloqueNuevo;
 	pthread_mutex_unlock(&semaforoTablaDeAsignaciones);
 	return 1;
 }

 int agregarBloquesDelBitmap(int bloquesAAgregar, osada_file* archivo) {
 	int i;
 	int cantidadDeRecorridos;
 	for (i = 0; i < bloquesAAgregar ; i ++) {
 		uint32_t bloqueNuevo = buscarBloqueVacio();
 		if(bloqueNuevo == -1) {
 			archivo->file_size += i * OSADA_BLOCK_SIZE;
 			return -1;
 		}
 		cantidadDeRecorridos = divisionMaxima(archivo->file_size);
 		agregarTablaDeAsignaciones(bloqueNuevo, archivo, cantidadDeRecorridos + i);
 	}

 	return 0;

 }

 int numeroBloqueDelArchivo(uint32_t numeroDeBloque, osada_file* archivo) {
 	int i = 1;
 	int bloque = archivo->first_block;
 	while(i < numeroDeBloque) {
 		bloque = tablaDeAsignaciones[bloque];
 		i++;
 	}
 	return bloque;
 }

 int agregar_informacion(int tamanioAAgregar, int offset, char* inicioDeAgregado, osada_file* archivo ) {
 	int numeroDeTabla = recorrerTablaDeAsignaciones(archivo, divisionMaxima(offset));
 	int agregado = 0;
 	int i=0;
 	int tamanioRestanteAAgregar = tamanioAAgregar;
 	int restanteDeMiBloque = OSADA_BLOCK_SIZE - offsetDondeEmpezar(offset);
 	while(agregado < tamanioAAgregar) {
 		int tamanioAAgregarDentroDelBloque = minimoEntre(tamanioRestanteAAgregar,restanteDeMiBloque);
 		memset(inicioDeAgregado,'\0',tamanioAAgregarDentroDelBloque);
 		restanteDeMiBloque = OSADA_BLOCK_SIZE;
 		agregado += tamanioAAgregarDentroDelBloque;
 		tamanioRestanteAAgregar-= tamanioAAgregarDentroDelBloque;
 		if(agregado < tamanioAAgregar) {
 			numeroDeTabla = tablaDeAsignaciones[numeroDeTabla];
 			inicioDeAgregado = (char*)bloquesDeDatos[numeroDeTabla];
 			i++;
 		}
 	}
 	return agregado;
 }

 int agregarBloques(osada_file* archivo, int diferenciaDeTamanios) {
 	pthread_mutex_lock(&semaforoTablaDeAsignaciones);
 	char* inicioDeAgregado = (char*)bloquesDeDatos[recorrerTablaDeAsignaciones(archivo, divisionMaxima(archivo->file_size + 1))] + offsetDondeEmpezar(archivo->file_size);
 	int agregado = 0;
 	agregado = agregar_informacion(diferenciaDeTamanios, archivo->file_size, inicioDeAgregado, archivo);
 	pthread_mutex_unlock(&semaforoTablaDeAsignaciones);
 	if(agregado != diferenciaDeTamanios) {
 		return -ENOENT;
 	}
 	return 0;
 }

 int quitarBloquesDelBitmap(int bloquesAQuitar, osada_file* archivo, int size_nuevo) {
 	int cant_bloques = divisionMaxima(archivo->file_size);
 	int ultimoBloqueADejar = cant_bloques - bloquesAQuitar;
 	int i;
 	for(i = ultimoBloqueADejar; i < cant_bloques; i++) {
 		int bloqueABorrar = numeroBloqueDelArchivo(i + 1, archivo);
 		bitarray_clean_bit(bitmap,bloqueABorrar + inicioDeBloqueDeDatos + (8 - sobraDeBitmap));
 	}
 	return 0;
 }

 osada_file* truncar_archivo(osada_file* archivo, uint32_t size) {
 	int diferenciaDeTamanios = abs(size - archivo->file_size);//Lo que tengo que agregar o quitar

 	if(archivo->file_size < size) {
 		int bloquesAAgregar = divisionMaxima(size) - divisionMaxima(archivo->file_size);
 	 	pthread_mutex_lock(&semaforoBitmap);
 		if(agregarBloquesDelBitmap(bloquesAAgregar, archivo) != 0) {
 			pthread_mutex_unlock(&semaforoBitmap);
 			return NULL;
 		}
 		agregarBloques(archivo, diferenciaDeTamanios);
 	 	pthread_mutex_unlock(&semaforoBitmap);
 	}
 	if(archivo->file_size > size) {
 		if(archivo->file_size % OSADA_BLOCK_SIZE < abs(diferenciaDeTamanios)) {
 			int bloquesARestar = divisionMaxima(archivo->file_size) - divisionMaxima(size);
 			pthread_mutex_lock(&semaforoBitmap);
 			quitarBloquesDelBitmap(bloquesARestar, archivo,size);
 			pthread_mutex_unlock(&semaforoBitmap);
 		}//Si el tamaño ocupado es mayor a lo que quiero quitar, no hace falta sacar bloques
 	}
 	//Va a ponerle de tamaño a mi archivo lo pasado en size. Incluyendo el agregado o borrado de bloques
 	archivo->file_size = size;
 	return archivo;
 }

 int escribir_informacion(int tamanioAEscribir, int offset, char* bufferConDatos, char* inicioDeEscritura, osada_file* archivo ) {
	int offsetAuxiliar = offset;
 	int numeroDeTabla = recorrerTablaDeAsignaciones(archivo, divisionMaxima(offset + 1));
 	int escrito = 0;
 	int i=0;
 	int tamanioRestanteAEscribir = tamanioAEscribir;
 	int restanteDeMiBloque = OSADA_BLOCK_SIZE - offsetDondeEmpezar(offset);
 	while(escrito < tamanioAEscribir) {
 		int tamanioAEScribirDentroDelBloque = minimoEntre(tamanioRestanteAEscribir,restanteDeMiBloque);
 		memcpy(inicioDeEscritura + offsetDondeEmpezar(offsetAuxiliar),bufferConDatos + escrito,tamanioAEScribirDentroDelBloque);
 		restanteDeMiBloque = OSADA_BLOCK_SIZE;
 		escrito = escrito + tamanioAEScribirDentroDelBloque;
 		tamanioRestanteAEscribir -= tamanioAEScribirDentroDelBloque;
 		if(escrito < tamanioAEscribir) {
 			offsetAuxiliar = 0;
 			numeroDeTabla = tablaDeAsignaciones[numeroDeTabla];
 			inicioDeEscritura = (char*)bloquesDeDatos[numeroDeTabla];
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


 int escribir_archivo(char* path, int offset, int tamanioAEscribir, char* bufferConDatos) {
 	osada_file* archivo = obtenerArchivo(path);
 	int escrito = 0;

 	if(archivo == NULL) {
 		return -ENOENT;
 		}
 	archivo = truncar_archivo(archivo,maximoEntre(offset+tamanioAEscribir,archivo->file_size));
 	if(archivo==NULL) {
 		return -1;
 	}
 	if(archivo->first_block == -1) {
 		return -1;
 	}
 	pthread_mutex_lock(&semaforoTablaDeAsignaciones);
 	int numeroDeTabla = recorrerTablaDeAsignaciones(archivo, divisionMaxima(offset + 1));
 	char* inicioDeEscritura =  (char*) bloquesDeDatos[numeroDeTabla];
 	escrito = escribir_informacion(tamanioAEscribir, offset, bufferConDatos, inicioDeEscritura, archivo);
 	pthread_mutex_unlock(&semaforoTablaDeAsignaciones);

 	return escrito;
 }

 //primero escribir
 int copiar_archivo(char* pathFuente, char*pathDestino) {
 	osada_file* archivoACopiar = obtenerArchivo(pathFuente);
 	int posicionEnLaTabla = crear_archivo(pathDestino,archivoACopiar->state);
 	if(archivoACopiar->state == REGULAR) {
 		char* buffer = malloc(archivoACopiar->file_size);
 		leer_archivo(pathFuente, 0, archivoACopiar->file_size,buffer);
 		escribir_archivo(pathDestino, 0, archivoACopiar->file_size, buffer);
 		free(buffer);
 	}
 	return 0;
 }
