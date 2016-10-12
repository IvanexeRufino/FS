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

/*
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	int buf;
	int i;
	ssize_t nbytes;

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	int enviar = 1;
	char message[PACKAGESIZE];

	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");
//	log_info(logger, "conectado al servidor, puede enviar mensajes. 'exit' para salir");

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);			// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
		if (!strcmp(message,"exit\n"))
			{
			enviar = 0;			// Chequeo que el usuario no quiera salir
			}
		if (enviar)
			{
			send(serverSocket, message, strlen(message) + 1, 0);
//			log_info (logger, "enviando mensaje");// Solo envio si el usuario no quiere salir.
			}
	}

	close(serverSocket);
	return 0;*/

	return fuse_main(argc,argv,&fuse_pokedex_cliente,NULL);
}
