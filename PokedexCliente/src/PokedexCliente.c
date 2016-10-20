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
	char* header;
	const char *body;
	int size;
}t_package;

static int getattr_callback(const char *path, struct stat *buffer){
	//memset(buffer,0,sizeof(struct stat));
	pedir_atributos("1",path,buffer);
	return 0;
}

t_list* pedir_atributos(char* num, const char *path, struct stat *buffer){
	t_package *package= malloc(sizeof(package));
	package->header=num;
	package->body=path;

	char* buf= malloc(sizeof(package));
	strcpy(buf,package->header);
	strcat(buf,package->body);

	int socket = conectarConServer();
	send(socket,buf,sizeof(buf),0);
	puts("enviado");

	char* bufferRecieve = malloc(sizeof(package));
	recv(socket,bufferRecieve,sizeof(bufferRecieve),0);
	puts("recibi algo");
	strcpy(package->header, bufferRecieve[0]);
	t_list listaDeHijos = list_create();
	int i;
	for(i=0;i < sizeof(bufferRecieve) - sizeof(bufferRecieve[0]); i++) {
		list_set(listaDeHijos,bufferRecieve[i+1]);
	}

	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);
	for(int i = 0; i <list_size(listaDeHijos); i++) {
		char* archivoHijo = list_get(listaDeHijos,i);
		filler(buffer, archivoHijo, NULL, 0);
	}
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
		//printf("Conectado");
	}

	return nuevoSocket;
}


static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {

		//memset(buf,0,sizeof(buf));
		pedir_atributos("2",path,buf);
		return 0;
}

//
//static int open_callback(const char *path, struct fuse_file_info *fi){
//	return 0;
//}
//static int write_callback(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi){
//	return 0;
//}

static struct fuse_operations fuse_pokedex_cliente = {
	.readdir=readdir_callback,
    .getattr = getattr_callback,
//  .open = open_callback,
//  .read = read_callback,
//  .write = write_callback,
};


int main(int argc, char *argv[]) {

//	t_log* logger;
//	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//	log_info(logger, PROGRAM_DESCRIPTION);





	return fuse_main(argc,argv,&fuse_pokedex_cliente,NULL);
}
