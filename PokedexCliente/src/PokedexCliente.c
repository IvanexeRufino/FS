/*
 * ejemplo.c
 *
 *  Created on: 26/08/2016
 *      Author: utnso
 */

#include "pokedexcliente.h"

int sockfd;
pthread_mutex_t sendRecv = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sendRecv;

typedef struct {
	uint16_t codigo;
	uint16_t tamanio;
	void* datos;
}__attribute__((__packed__)) t_paquete ;


void* memoria(int cantidad) {
	void* puntero = NULL;
	if (cantidad !=0) {
		while (puntero == NULL) {
  			puntero= malloc(cantidad);
  		}
  	}
  	return puntero;
}

typedef struct {
	uint16_t codigo;
	uint16_t tamanio;
	uint16_t offset;
	uint16_t size;
}__attribute__((__packed__)) t_paquetePro ;

t_paquetePro* empaquetarPro(int codigo, int tamanio, int offset, int size){
	t_paquetePro* paquete= malloc(sizeof(t_paquetePro));
	paquete->codigo= codigo;
	paquete->tamanio= tamanio;
	paquete->offset = offset;
	paquete->size = size;
	return paquete;
}

void* acopladorPro(t_paquetePro* paquete) {
	void* paqueteSalida = memoria(size_header);
	memcpy(paqueteSalida, paquete, size_header);
	return paqueteSalida;
}

t_paquetePro* desacopladorPro(char* buffer) {
  	t_paquetePro* paquete;
  	paquete= memoria(sizeof(t_paquetePro));
  	memcpy(paquete,buffer,size_header);
  	return paquete;
}


t_paquete* empaquetar(int codigo, void* datos, int size){
	t_paquete* paquete= malloc(sizeof(t_paquete));
	paquete->codigo= codigo;
	paquete->datos= datos;
	paquete->tamanio= size;
	return paquete;
}

t_paquete* enviarQueSos(t_paquetePro* paquete, void* path){
	pthread_mutex_lock(&sendRecv);

	void* cosaparaenviar;

	switch(paquete->codigo) {
	case 1:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 2:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 3:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 4:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 5:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 6:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 8:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 9:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 10:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 11:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	case 12:
		cosaparaenviar = acopladorPro(paquete);
		if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		if(send(sockfd, path, paquete->tamanio,0) < 0) {
			puts("ERROR ENVIO");
			exit(1);
		}
		break;
	}

	/////////////////////Recibo el header/////////////////////////

	char bufferHead[size_header];
	if(recv(sockfd, &bufferHead, size_header, MSG_WAITALL) <= 0) {
		puts("ERROR RECIBIR");
		exit(1);
	}
	t_paquetePro* paqueterecv = desacopladorPro(bufferHead);

	/////////////////////Recibo el payload////////////////////////

	char buffer[paqueterecv->tamanio];
	if(recv(sockfd, &buffer, paqueterecv->tamanio, MSG_WAITALL) <= 0) {
		puts("ERROR RECIBIR");
		exit(1);
	}
	void* pathBuffer = malloc(paqueterecv->tamanio);
	memcpy(pathBuffer,buffer,paqueterecv->tamanio);

	pthread_mutex_unlock(&sendRecv);
	return empaquetar(paqueterecv->codigo, pathBuffer, paqueterecv->tamanio);
}

static int ejemplo_getattr(char *path, struct stat *stbuf) {
	int res = 0;
	t_paquete* paquete = enviarQueSos(empaquetarPro(1, strlen(path) + 1,0,0),path);
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;

		return res;
	}
	else {
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
	t_paquete* paquete= enviarQueSos(empaquetarPro(2, strlen(path) + 1,0,0),path);

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
	t_paquete* paquete = enviarQueSos(empaquetarPro(3, strlen(filename) + 1,0,0), filename);

	if(paquete->codigo == 100) {
		return ENAMETOOLONG;
	}
	if(paquete->codigo == 101) {
		return EDQUOT;
	}

	return 0;
}

static int ejemplo_create (char* path, mode_t modo, struct fuse_file_info * info) {
	t_paquete* paquete = enviarQueSos(empaquetarPro(4, strlen(path) + 1,0,0), path);
	if(paquete->codigo == 100) {
		return ENAMETOOLONG;
	}
	if(paquete->codigo == 101) {
		return EDQUOT;
	}
	return 0;
}

static int ejemplo_open(char * path, int info) {
	t_paquete* paquete = enviarQueSos(empaquetarPro(5, strlen(path) + 1,0,0), path);
	if(paquete->codigo == 100) {
		return -ENOENT;
	}
	return 0;
}


static int ejemplo_read(char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	t_paquete* paqueteRec = enviarQueSos(empaquetarPro(6, strlen(path) + 1, offset, size), path);
	if(paqueteRec->codigo == 100) {
			return 0;
		}
	memcpy(buf,paqueteRec->datos,paqueteRec->tamanio);
	return paqueteRec->tamanio;
}

static int ejemplo_write (char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	pthread_mutex_lock(&sendRecv);
	void* cosaparaenviar = acopladorPro(empaquetarPro(7, strlen(path) + 1, offset, size));
	if(send(sockfd,cosaparaenviar, size_header ,0) < 0) {
		puts("ERROR ENVIO");
		exit(1);
	}
	if(send(sockfd, path, strlen(path) + 1,0) < 0) {
		puts("ERROR ENVIO");
		exit(1);
	}
	if(send(sockfd, buf, size,0) < 0) {
		puts("ERROR ENVIO");
		exit(1);
	}

	//////////////////////////////RECIBIR ///////////////////////////////////

	char bufferHead[size_header];
	if(recv(sockfd, &bufferHead, size_header, MSG_WAITALL) <= 0) {
		puts("ERROR RECIBIR");
		exit(1);
	}
	t_paquetePro* paqueterecv = desacopladorPro(bufferHead);

	char buffer[paqueterecv->tamanio];
	if(recv(sockfd, &buffer, paqueterecv->tamanio, MSG_WAITALL) <= 0) {
		puts("ERROR RECIBIR");
		exit(1);
	}
	void* pathBuffer = malloc(paqueterecv->tamanio);
	memcpy(pathBuffer,buffer,paqueterecv->tamanio);
	pthread_mutex_unlock(&sendRecv);

	return paqueterecv->tamanio;
}

static int ejemplo_remove (char* path) {
	t_paquete* paquete = enviarQueSos(empaquetarPro(8, strlen(path) + 1,0,0), path);

	if(paquete->codigo == 100) {
		return ENOENT;
	}
	return 0;
}

static int ejemplo_utimens (char * param1, const struct timespec tv[2] ){
	enviarQueSos(empaquetarPro(9, strlen(param1) + 1, 0, 0), param1);
	return 0;
}

static int ejemplo_truncate(char* path, off_t size) {
	t_paquete* paqueterecv= enviarQueSos(empaquetarPro(10, strlen(path) + 1,0,size), path);
	if(paqueterecv->codigo == 100 ) {
		return -ENOENT;
	}
	return 0;
}

static int ejemplo_rename(char *nombreViejo, char *nombreNuevo){
	char* bufo=malloc(strlen(nombreViejo)+strlen(nombreNuevo)+2);
	strcpy(bufo,nombreViejo);
	strcat(bufo,"%");
	strcat(bufo,nombreNuevo);
	enviarQueSos(empaquetarPro(11, strlen(bufo) + 1,0,0), bufo);
	return 0;
}

static int ejemplo_link (char *archivoOrigen, char *archivoDestino){
	char* bufo=malloc(strlen(archivoOrigen)+strlen(archivoDestino)+2);
	strcpy(bufo,archivoOrigen);
	strcat(bufo,"%");
	strcat(bufo,archivoDestino);
	enviarQueSos(empaquetarPro(12, strlen(bufo) + 1,0,0), bufo);
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
	pthread_mutex_init (&sendRecv,NULL);

	return fuse_main(argc, argv, &ejemplo_oper, NULL );

}
