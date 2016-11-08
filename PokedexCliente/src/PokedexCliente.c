/*
 * ejemplo.c
 *
 *  Created on: 26/08/2016
 *      Author: utnso
 */

#include "pokedexcliente.h"
int sockfd;

void* memoria(int cantidad) {
	void* puntero = NULL;
	if (cantidad !=0) {
		while (puntero == NULL) {
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
}

t_paquete* empaquetar(int codigo, void* datos, int size){
	t_paquete* paquete= malloc(sizeof(t_paquete));
	paquete->codigo= codigo;
	paquete->datos= datos;
	paquete->tamanio= size;
	return paquete;
}

void* acoplador1(t_paquete* paquete) /*transforma una estructura de tipo t_paquete en un stream*/
{
	void* paqueteSalida = memoria(size_header + strlen(paquete->datos) + 1);
	memcpy(paqueteSalida, paquete, size_header);
	memcpy(paqueteSalida + size_header, paquete->datos, strlen(paquete->datos) + 1);
	return paqueteSalida;
}

void* acoplador(t_paquete* paquete) /*transforma una estructura de tipo t_paquete en un stream*/
{
	void* paqueteSalida = memoria(size_header + paquete->tamanio);
	memcpy(paqueteSalida, paquete, size_header);
	memcpy(paqueteSalida + size_header, paquete->datos, paquete->tamanio);
	return paqueteSalida;
}

t_paquete* desacoplador(char* buffer)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
  {
  	t_paquete* paquete;
  	paquete= memoria(sizeof(t_paquete));
  	memcpy(paquete,buffer,size_header);
  	paquete->datos= memoria(paquete->tamanio);
  	memcpy(paquete->datos, buffer + size_header, paquete->tamanio);
  	return paquete;
  }

int conectarConServer()
  {
  	struct sockaddr_in socket_info;
  	int nuevoSocket;
  	// Se carga informacion del socket
  	socket_info.sin_family = AF_INET;
  	socket_info.sin_addr.s_addr = inet_addr("127.0.0.1");
  	socket_info.sin_port = htons(9999);

  	// Crear un socket:
  	// AF_INET, SOCK_STREM, 0
  	nuevoSocket = socket (AF_INET, SOCK_STREAM, 0);
  	if (nuevoSocket < 0)
  		return -1;
  	// Conectar el socket con la direccion 'socketInfo'.
  	int conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
  	int mostrarEsperaAconectar=0;
  	while (conecto != 0){
  		mostrarEsperaAconectar++;
  		if (mostrarEsperaAconectar == 1){
  			printf("Esperando...\n");
  		}
  		conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
  		printf("Conectado");
  	}

  	return nuevoSocket;
};

t_paquete* enviarQueSos(int nroop, void* path, int size){
	t_paquete* paquete = empaquetar(nroop,path,size);
	void* cosaparaenviar = acoplador(paquete);

	if(send(sockfd,cosaparaenviar,paquete->tamanio + size_header ,0)<0)
		{
			puts("ERROR ENVIO");
//			log_error(logDelPersonaje,"Error al enviar datos del cliente \n");
			exit(1);
		}
	free(cosaparaenviar);

	char buffer[MAX_BUFFERSIZE];
	int sizebytes;
	if((sizebytes = recv(sockfd, &buffer, MAX_BUFFERSIZE - 1,0)) <= 0)
	{
		puts("ERROR RECIBIR");
		//log_error(logDelPersonaje, "Error al recibir paquete del cliente \n");
		exit(1);
	}
	return paquete = desacoplador(buffer);
}

static int ejemplo_getattr(char *path, struct stat *stbuf) {
	t_paquete* paquete = enviarQueSos(1,path, strlen(path) + 1);
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else {
		osada_file* archivo = paquete->datos;
		if(paquete->codigo == 100 || archivo->state == 0) {
		return -ENOENT;
	} else if (archivo->state == 2) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} 	else {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = archivo->file_size;
	}
	}
	return res;
}

static int ejemplo_readdir(char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {

	int res=0;
	int i=0;
	t_paquete* paquete= enviarQueSos(2,path,strlen(path)+1);

	filler(buf,".",NULL,0);
	filler(buf,"..",NULL,0);

	if(paquete->codigo == 100){
		return res;
	}

	char* buforecibido= paquete->datos;
	char** bufonuevo= string_split(buforecibido,"/");
	while(bufonuevo[i]!=NULL){
		filler(buf,bufonuevo[i],NULL,0);
		i++;
	}
	return res;
}

static int ejemplo_mkdir(char* filename, mode_t modo){
	enviarQueSos(3, filename, strlen(filename) + 1);
	return 0;
}

static int ejemplo_create (char* path, mode_t modo, struct fuse_file_info * info) {
	enviarQueSos(4, path, strlen(path) + 1);
	return 0;
}

static int ejemplo_open(char * path, int info) {
	t_paquete* paquete = enviarQueSos(5, path, strlen(path) + 1);
	osada_file* archivo = paquete->datos;
	if(paquete->codigo == 100 || archivo->state == 0) {
		return -ENOENT;
	}
	return 0;
}


static int ejemplo_read(char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	t_paquete* paqueteRead1 = empaquetar(offset, path, size);
	void* streamRead1 = acoplador1(paqueteRead1);
	t_paquete* paqueteRec = enviarQueSos(6, streamRead1, strlen(path) + 1 + size_header);
	memcpy(buf,paqueteRec->datos,paqueteRec->tamanio);
	return paqueteRec->tamanio;
}

static int ejemplo_write (char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	char* bufosend= malloc(strlen(path) + size + 1);
	memcpy(bufosend,path,strlen(path) + 1);
	memcpy(bufosend + strlen(path), buf, size);
	t_paquete* paquetewrite= empaquetar(offset,bufosend,size);
	void* streamwrite= acoplador1(paquetewrite);
	t_paquete* paqueterec= enviarQueSos(7,streamwrite,strlen(path) + 1 + size + size_header);
	return paqueterec->tamanio;
}

static int ejemplo_remove (char* path) {
	t_paquete* paquete = enviarQueSos(8, path, strlen(path) + 1);
	if(strcmp(paquete->datos,"error")) {
		return -ENOENT;
	}
	return 0;
}

static int ejemplo_utimens (char * param1, const struct timespec tv[2] ){
	enviarQueSos(9, param1, strlen(param1) + 1);
	return 0;
}

static int ejemplo_truncate(char* path, off_t size) {
	t_paquete* paquetet = empaquetar(0, path, size);
	void* streamtrun = acoplador1(paquetet);
	t_paquete* paqueterecv= enviarQueSos(10, streamtrun, strlen(path) + 1 + size_header);
	if(paqueterecv->codigo == 100 ) {
		return 0;
	}
	return 0;
}

static int ejemplo_rename(char *nombreViejo, char *nombreNuevo){
	char* bufo=malloc(strlen(nombreViejo)+strlen(nombreNuevo)+1);
	strcpy(bufo,nombreViejo);
	strcat(bufo,"%");
	strcat(bufo,nombreNuevo);
	enviarQueSos(11, bufo, strlen(bufo) + 1);
	return 0;
}

static int ejemplo_link (char *archivoOrigen, char *archivoDestino){
	char* bufo=malloc(strlen(archivoOrigen)+strlen(archivoDestino)+1);
	strcpy(bufo,archivoOrigen);
	strcat(bufo,"%");
	strcat(bufo,archivoDestino);
	enviarQueSos(12, bufo, strlen(bufo) + 1);
	return 0;
}

static struct fuse_operations ejemplo_oper = {
		.getattr = ejemplo_getattr,
		.readdir = ejemplo_readdir,
		.open = ejemplo_open,
		.read = ejemplo_read,
		.create = ejemplo_create,
		.unlink = ejemplo_remove,
		.mkdir = ejemplo_mkdir,
		.write = ejemplo_write,
		.utimens = ejemplo_utimens,
		.rmdir = ejemplo_remove,
		.truncate = ejemplo_truncate,
		.rename = ejemplo_rename,
		.link = ejemplo_link,
};


int main(int argc, char *argv[]) {

	struct sockaddr_in socket_info;
	  	// Se carga informacion del socket
	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	socket_info.sin_port = htons(9999);

	  	// Crear un socket:
	  	// AF_INET, SOCK_STREM, 0
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return -1;
	// Conectar el socket con la direccion 'socketInfo'.
	int conecto = connect (sockfd,(struct sockaddr *)&socket_info,sizeof (socket_info));
	int mostrarEsperaAconectar=0;
	while (conecto != 0){
		mostrarEsperaAconectar++;
		if (mostrarEsperaAconectar == 1){
			printf("Esperando...\n");
		}
		conecto = connect (sockfd,(struct sockaddr *)&socket_info,sizeof (socket_info));
		printf("Conectado");
	}

	return fuse_main(argc, argv, &ejemplo_oper, NULL );

}
