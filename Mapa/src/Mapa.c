#include "Mapa.h"
#include <dirent.h>


#define BACKLOG 10

t_list* listaPokenest;
t_list* items;
mapa_datos* infoMapa;
t_list* entrenadoresActivos;
t_list* entrenadoresBloqueados;
pthread_mutex_t mutex_EntrenadoresActivos = PTHREAD_MUTEX_INITIALIZER;


void leerConfiguracionPokenest(char* mapa, char* path);
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int leerConfiguracionMapa( t_list* listaPokenest)
{

	char nombre[20];
	printf("%s", "Nombre del Mapa?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[256] = "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/Mapas/";
	char path[256];
	strcat(pathconfigMetadata, nombre);
	strcpy(path,pathconfigMetadata);
	strcat(path, "/PokeNests");
	strcat(pathconfigMetadata,  "/metadata");
	t_config* config = config_create(pathconfigMetadata);
	// Verifico que los parametros tengan sus valores OK
	if ( config_has_property(config, "IP") && config_has_property(config, "Puerto")
	&& config_has_property(config, "algoritmo") && config_has_property(config, "quantum")
	&& config_has_property(config, "retardo") && config_has_property(config, "Batalla")
	&& config_has_property(config, "TiempoChequeoDeadlock"))
	{
		infoMapa->nombre = nombre;
		infoMapa->tiempoChequeoDeadlock = config_get_int_value(config, "TiempoChequeoDeadlock");
		infoMapa->batalla = config_get_int_value(config, "Batalla");
		infoMapa->algoritmo  = config_get_string_value(config, "algoritmo");
		infoMapa->quantum = config_get_int_value(config, "quantum");
		infoMapa->retardo = config_get_int_value(config, "retardo");
		infoMapa->ipEscucha = config_get_string_value(config, "IP");
		infoMapa->puertoEscucha  = config_get_string_value(config, "Puerto");

		printf(" El nombre del mapa es: %s\n su tiempoChequeoDeadlock es: %d\n su batalla es: %d\n "
				"su algoritmo es: %s\n su quantum es de: %d\n su retardo es: %d\n su ipEscucha es: %s\n "
				"su puertoEscucha es: %s\n"
								,infoMapa->nombre,
								infoMapa->tiempoChequeoDeadlock,
								infoMapa->batalla,
								infoMapa->algoritmo,
								infoMapa->quantum,
								infoMapa->retardo,
								infoMapa->ipEscucha,
								infoMapa->puertoEscucha);
		//Leo todas la pokenest ******************************************************* POR AHORA UNA

		DIR *dp;
		struct dirent *ep;
		dp = opendir (path);
			if (dp != NULL)
			{
			while (ep = readdir (dp)){

				  if(ep->d_name[0]!='.'){
//					  puts (ep->d_name);
					  leerConfiguracionPokenest(nombre,ep->d_name);
				  }
			}
				  (void) closedir (dp);

			}

		else
			perror ("Couldn't open the directory");

	}
	else
    {
		return -1;
    }
	return 1 ;
}

void leerConfiguracionPokenest(char mapa[20], char pokemon[256]){
	int cantidadPokemon = 0;
	char pathpokenestMetadata[256] = "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/Mapas/";
	strcat(pathpokenestMetadata, mapa);
	char path[256];
	strcpy(path,pathpokenestMetadata);
	strcat(path,"/PokeNests/");
	strcat(path,pokemon);
	strcpy(pathpokenestMetadata,path);
	strcat(pathpokenestMetadata,"/metadata");
	strcat(path,"/");
	t_config* configNest = config_create(pathpokenestMetadata);
	t_registroPokenest* pokenest = malloc(sizeof(t_registroPokenest));

	//puts(path);
		DIR *dp;
		  struct dirent *ep;
		  dp = opendir (path);

		  if (dp != NULL)
		  {
		    while (ep = readdir (dp)){
		    	 if(ep->d_name[0]!='.' && ep->d_name[0]!='m' ){
		    		 cantidadPokemon ++;
//				      puts (ep->d_name);
		    	 }

		    }

		    (void) closedir (dp);
		  }
		  else
		    perror ("Couldn't open the directory");


		  pokenest->cantidadDisp = cantidadPokemon;

	if (config_has_property(configNest, "Tipo") && config_has_property(configNest, "Identificador")){

		pokenest->tipo = config_get_string_value(configNest, "Tipo");
		pokenest->identificador= config_get_string_value(configNest, "Identificador");
		pokenest->x =config_get_int_value(configNest,"X");
		pokenest->y = config_get_int_value(configNest,"Y");

//		printf("\n El Tipo del Nest es: %s\n su posicion es X: %d\n Y es: %d\n "
//							"su identificador: %s\n Y hay %d Pokemons de ese Tipo\n"
//											,pokenest->tipo,pokenest->x, pokenest->y, pokenest->identificador, pokenest->cantidadDisp);

	CrearCaja(items, config_get_string_value(configNest, "Identificador")[0] , pokenest->x , pokenest->y ,pokenest->cantidadDisp);

	list_add(listaPokenest,pokenest);


	}

}
void funcionDelThread (int newfd){

	printf("%d",newfd);
}

t_registroPersonaje *get_personaje_en_socket(int socket) {
	int _with_socket(t_registroPersonaje *p) {
		return (p->socket == socket);
	}

	return list_find(entrenadoresActivos, (void*)_with_socket);
}

t_registroPokenest *get_pokenest_identificador(char identificador) {
	int _with_identificador(t_registroPokenest *p) {
		return (p->identificador == identificador);
	}

	return list_find(entrenadoresActivos, (void*)_with_identificador);
}

void str_cut(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return;
}


void recibirEntrenador(int newfd){
	char* buffer = malloc(2);
	recv(newfd, buffer, sizeof(char) * 2, 0);
	//int a;


	t_registroPersonaje *nuevoPersonaje;
	nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	//memcpy(&(a), buffer, sizeof(char));

	char* bufferRecortado = malloc(2);
	strcpy(bufferRecortado, buffer);
	str_cut(bufferRecortado,0,1);
	memcpy((nuevoPersonaje->identificador), bufferRecortado,  sizeof(char));

	                    //----RECIBO OBJETIVOS
	recv(newfd, nuevoPersonaje->objetivos,sizeof(char)*7,0);
	//puts(nuevoPersonaje->objetivos);
	nuevoPersonaje->socket=newfd;
	//printf("reciving char: %c\n", a);
	//printf("reciving char: %c\n", nuevoPersonaje->identificador);
	nuevoPersonaje->x = 1;
	nuevoPersonaje->y = 1;
	char id = (nuevoPersonaje->identificador)[0];
    CrearPersonaje(items, nuevoPersonaje->identificador , nuevoPersonaje->x, nuevoPersonaje->y);
    pthread_mutex_lock(&mutex_EntrenadoresActivos);

    list_add(entrenadoresActivos, nuevoPersonaje);
    pthread_mutex_unlock(&mutex_EntrenadoresActivos);
    free(buffer);
//    free(nuevoPersonaje);
    free(bufferRecortado);
    //nuevoPersonaje = NULL;


}

int crearSocketServidor(char *puerto) {
	int BACKLOOG = 5;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(NULL, puerto, &hints, &serverInfo);
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	int yes =1;
	setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOOG);
	return listenningSocket;
}

int IniciarSocketServidor(int puertoServer)
{
	struct sockaddr_in socketInfo;
		int socketEscucha;
		int optval = 1;

		// Crear un socket
		socketEscucha = socket (AF_INET, SOCK_STREAM, 0);
		if (socketEscucha == -1)
		 	return -1;

		setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval,
				sizeof(optval));
		socketInfo.sin_family = AF_INET;
		socketInfo.sin_port = htons(puertoServer);
		socketInfo.sin_addr.s_addr = INADDR_ANY;
		if (bind (socketEscucha,(struct sockaddr *)&socketInfo,sizeof (socketInfo)) != 0)
		{
			close (socketEscucha);
			return -1;
		}

		/*
		* Se avisa al sistema que comience a atender llamadas de clientes
		*/
		if (listen (socketEscucha, 10) == -1)
		{
			close (socketEscucha);
			return -1;
		}
		/*
		* Se devuelve el descriptor del socket servidor
		*/
		return socketEscucha;
	}



int AceptarConexionCliente(int socketServer)
{
	socklen_t longitudCliente;//esta variable tiene inicialmente el tamaño de la estructura cliente que se le pase
	struct sockaddr cliente;
	int socketNuevaConexion;//esta variable va a tener la descripcion del nuevo socket que estaria creando
	longitudCliente = sizeof(cliente);
	socketNuevaConexion = accept (socketServer, &cliente, &longitudCliente);//acepto la conexion del cliente
	if (socketNuevaConexion < 0)
		return -1;

	return socketNuevaConexion;

}



int main(int argc, char **argv)
{

	int rows;
	int cols;
	listaPokenest = malloc(sizeof(t_registroPokenest));
	listaPokenest = list_create();
	items = list_create();
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&rows, &cols);
	entrenadoresActivos = malloc(sizeof(t_list));
	entrenadoresBloqueados = malloc (sizeof(t_list));

		/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//		log_info(logger, PROGRAM_DESCRIPTION);

	//--------
	  infoMapa = malloc((sizeof(char*)*4 + sizeof(int)*4));
	  if (leerConfiguracionMapa (listaPokenest) == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");

	  nivel_gui_inicializar();
	  		nivel_gui_get_area_nivel(&rows, &cols);
	  		nivel_gui_dibujar(items,&argv);
//	  int ii = 0;
//	  while(ii!= list_size(listaPokenest)){
//		  pokenestPrueba = list_get(listaPokenest,ii);
//		   printf("%d",pokenestPrueba->cantidadDisp);
//		   puts(pokenestPrueba->identificador);
//		  ii++;
//	  }


// pthread_t idHiloPlanificador;
//
// pthread_create (&idHiloPlanificador, NULL, (void*) funcionDelThread, 5);
//
// pthread_join(idHiloPlanificador,0);






    // bucle principal
     	 int socketServidor;
     	 int newfd;
    	socketServidor = crearSocketServidor(infoMapa->puertoEscucha);
    	IniciarSocketServidor(infoMapa->puertoEscucha);
    	newfd = AceptarConexionCliente(socketServidor);
    	printf("%d", newfd);

    	pthread_t idHilo;
    	pthread_create (&idHilo, NULL, (void*) funcionDelThread, newfd);
    	pthread_join(idHilo,0);


    	while(1)
    		 nivel_gui_dibujar(items, argv );

 return 0;

}
