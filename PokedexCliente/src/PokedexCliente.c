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

typedef struct {
	char *header;
	const char *path;
	int size;
}t_package;

typedef struct {
	int header;
	const char *path;
	void *buf;
	fuse_fill_dir_t filler;
	off_t offset;
	struct fuse_file_info *fi;
}readdir_struct;

static int getattr_callback(const char *path, struct stat *buffer){
	memset(buffer,0,sizeof(struct stat));
	int resultado= pedir_atributos(path,buffer);
	return resultado;
}

int pedir_atributos(const char *path, struct stat *buffer){
	t_package *package= malloc(sizeof(package));
	package->header="1";
	package->path=path;

	char* buf= malloc(sizeof(package));
	strcpy(buf,package->header);
	strcat(buf,package->path);

	int socket = conectarConServer();
	send(socket,buf,sizeof(buf),0);
	free(buf);

	return 1;
}


int enviar_msg (char *msg){

	int socket = conectarConServer();
	send(socket,msg,sizeof(msg),0);
	free(msg);
	puts("fjdsfjdsb");
	return 0;
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
}


//static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
//		off_t offset, struct fuse_file_info *fi) {
//
//		readdir_struct readdir_struct;
//		readdir_struct.header= 2;
//		readdir_struct.path= *path;
//		readdir_struct.buf= *buf;
//		readdir_struct.filler= filler;
//		readdir_struct.offset= offset;
//		readdir_struct.fi= *fi;
//
//		return 0;
//}
//
//static int open_callback(const char *path, struct fuse_file_info *fi){
//	return 0;
//}
//
//static int read_callback(const char *path, char *buf, size_t size, off_t offset,
//    struct fuse_file_info *fi) {
//	return 0;
//}
//
//static int write_callback(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi){
//	return 0;
//}

static struct fuse_operations fuse_pokedex_cliente = {
  .getattr = getattr_callback,
//  .open = open_callback,
//  .read = read_callback,
//  .readdir=readdir_callback,
//  .write = write_callback,
};


int main(int argc, char *argv[]) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);





	return fuse_main(argc,argv,&fuse_pokedex_cliente,NULL);
}
