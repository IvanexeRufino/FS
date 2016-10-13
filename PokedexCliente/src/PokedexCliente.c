/*
 ============================================================================
 Name        : PokedexCliente.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//#include "Configuracion.c"
#include "pokedexcliente.h"

static int getattr_callback(const char *path, struct stat *stbuf){
		return 0;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {
	return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi){
	return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi) {
	return 0;
}

static int write_callback(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi){
	return 0;
}

static struct fuse_operations fuse_pokedex_cliente = {
  .getattr = getattr_callback,
  .readdir = readdir_callback,
  .open = open_callback,
  .read = read_callback,
  .write = write_callback,

};

int main(int argc, char *argv[]) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);

	int sock;
	struct sockaddr_in server;
	char message [1000], server_reply[2000];

	sock= socket(AF_INET,SOCK_STREAM,0);
	if (sock==1){
		printf("No se pudo creer el socket");
	}
	puts("Socket creado");

	server.sin_addr.s_addr= inet_addr("127.0.0.1");
	server.sin_family= AF_INET;
	server.sin_port= htons(8888);

	//conectar al servidor
	if(connect(sock,(struct sockaddr*)&server,sizeof(server))<0){
		perror("Fallo la conexion");
		return 1;
	}
	puts("Conectado\n");

	//comunicando con el servidor
	while (1){
		printf("Escribir mensaje:");
		scanf("%s",message);

		//enviar datos
		if(send(sock,message,strlen(message),0)<0){
			puts("Fallo el envio");
			return 1;
		}

		//recibir respuesta del servidor
		if(recv(sock,server_reply,2000,0)<0){
			puts("Fallo la recepcion");
			break;
		}
		puts("Respuesta del servidor:");
		puts(server_reply);
	}

	close(sock);
	return 0;
	// return fuse_main(argc,argv,&fuse_pokedex_cliente,NULL);
}
