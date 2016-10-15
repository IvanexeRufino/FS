#include "Mapa.h"

t_list* listaPokenest;
t_list* items;
mapa_datos* infoMapa;
t_list* entrenadoresActivos;
t_list* entrenadoresBloqueados;
pthread_mutex_t mutex_EntrenadoresActivos = PTHREAD_MUTEX_INITIALIZER;
t_registroPokenest *pokemonActual;
t_registroPersonaje *nuevoPersonaje;

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
		strcpy(infoMapa->nombre,nombre);
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
		strcpy(pokenest->tipo,(config_get_string_value(configNest, "Tipo")));
		char* identificadorPokenest= config_get_string_value(configNest, "Identificador");
		pokenest->x =config_get_int_value(configNest,"X");
		pokenest->y = config_get_int_value(configNest,"Y");
		pokenest->identificador=identificadorPokenest[0];

//		printf("\n El Tipo del Nest es: %s\n su posicion es X: %d\n Y es: %d\n "
//							"su identificador: %s\n Y hay %d Pokemons de ese Tipo\n"
//											,pokenest->tipo,pokenest->x, pokenest->y, pokenest->identificador, pokenest->cantidadDisp);

	CrearCaja(items, config_get_string_value(configNest, "Identificador")[0] , pokenest->x , pokenest->y ,pokenest->cantidadDisp);

	list_add(listaPokenest,pokenest);


	}

}

void recibirCoordenada(int *coordenada, int socketEntrenador)
{
	char* buffer = malloc(sizeof(char)*3);
	recv(socketEntrenador, buffer,sizeof(buffer), 0);

	char* payload = malloc(sizeof(char)*3);
	strcpy(payload, buffer);
	str_cut(payload,0,1);

	(*coordenada)=atoi(payload);
	free(buffer);
	free(payload);
}

void recibirBienvenidaEntrenador(int newfd,t_registroPersonaje *nuevoPersonaje)
{
	recibirCoordenada(&(nuevoPersonaje->x),newfd);		//Recibo coordenada Entrenador en X
	printf("La coordenada INICIAL en X del ENTRENADOR es %d \n", nuevoPersonaje->x);
	recibirCoordenada(&(nuevoPersonaje->y),newfd);		//Recibo coordenada Entrenador en Y
	printf("La coordenada INICIAL en Y del ENTRENADOR es %d \n", nuevoPersonaje->y);

	char* buffer = malloc(sizeof(char)*3);
	recv(newfd,buffer,sizeof(buffer),0);
	printf("Lo que recibio del cliente %d es esto: %s\n", newfd,buffer);

	char bufferConAccion;
	bufferConAccion=buffer[0];
	printf("La accion que pide hacer el entrenador es %c\n", bufferConAccion);

	char bufferConID;
	bufferConID=buffer[1];
	char identificador= bufferConID;

	nuevoPersonaje->identificador=identificador;
	printf("El ID del entrenador es %c\n", nuevoPersonaje->identificador);

	pthread_mutex_lock(&mutex_EntrenadoresActivos);
	list_add(entrenadoresActivos, nuevoPersonaje);
	pthread_mutex_unlock(&mutex_EntrenadoresActivos);

	free(buffer);

}

void cargoDatosPokemonActual(char pokemonQueRecibo,t_registroPokenest* pokemonActual)
{
	int j;
	char pok= pokemonQueRecibo;
	for(j = 0 ; j< list_size(listaPokenest); j++)
		{
			t_registroPokenest* pokenest = list_get(listaPokenest,j);
			if (pokenest->identificador == pok)
				{
					pokemonActual->identificador=pokenest->identificador;
					//pokemonActual->tipo=pokenest->tipo;
					strcpy(pokemonActual->tipo,pokenest->tipo);
					pokemonActual->x=pokenest->x;
					pokemonActual->y=pokenest->y;
					pokemonActual->cantidadDisp=pokenest->cantidadDisp;
				}
		}
}

void mover (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual){

	if(personaje->ultimoRecurso == 1){  //ultimo movimiento fue en Y => me muevo en X
		if(personaje->x <= pokemonActual->x)
			personaje->x +=1;
		else
			personaje->x -=1;
	personaje->ultimoRecurso = 0;
	}
	else{
		if(personaje->y <= pokemonActual->y)
			personaje->y +=1;
		else
			personaje->y -=1;
	 personaje->ultimoRecurso = 1 ;
	}
	if(personaje->x == pokemonActual->x) personaje->ultimoRecurso = 0;
	if(personaje->y == pokemonActual->y) personaje->ultimoRecurso = 1;
	enviarCoordenada(personaje->x,personaje->socket);
	printf("Estoy enviando la coordenada en X que es %d \n",personaje->x);
	enviarCoordenada(personaje->y,personaje->socket);
	printf("Estoy enviando la coordenada en Y que es %d \n",personaje->y);
}

void envioQueSeAtrapoPokemon (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual){
	if(pokemonActual->cantidadDisp >= 1) {
		pokemonActual->cantidadDisp --;
		printf("/*--------------------El Personaje: %c , atrapo al pokemon %c --------------------*/ \n",personaje->identificador, pokemonActual->identificador);
		send(personaje->socket, "1", sizeof(int), 0);
	}
	else send(personaje->socket, "0", sizeof(int), 0);
}

void recibirQueHacer(t_registroPersonaje *nuevoPersonaje,t_registroPokenest* pokemonActual)
{
	char* buffer = malloc(sizeof(char)*2);
	recv(nuevoPersonaje->socket,buffer,sizeof(buffer),0);
	printf("Lo que recibio para hacer es esto: %s\n",buffer);

	char bufferConAccion;        //Vendria a ser el header
	bufferConAccion=buffer[0];
	printf("Separo el header y me queda: %c\n",bufferConAccion);

	char* payload = malloc(sizeof(char)*2);

	strcpy(payload, buffer);
	str_cut(payload,0,1);
	printf("Separo el payload y me queda esto: %s\n",payload);

	int coordenadaX,coordenadaY;

	//free(buffer);

	switch(bufferConAccion)
	{
	case ('1'):

		cargoDatosPokemonActual(payload[0],pokemonActual);

		coordenadaX = enviarCoordenada(pokemonActual->x,nuevoPersonaje->socket);				//Envio coordenada del pokemon en X
		printf("Estoy enviando la coordenada en X que es %d \n",coordenadaX);

		coordenadaY = enviarCoordenada(pokemonActual->y,nuevoPersonaje->socket);				//Envio coordenada del pokemon en Y
		printf("Estoy enviando la coordenada en Y que es %d \n",coordenadaY);

		break;
	case ('2'):

		mover(nuevoPersonaje,pokemonActual);

		break;
	case ('3'):

		envioQueSeAtrapoPokemon(nuevoPersonaje,pokemonActual);

		break;
	}

}

void funcionDelThread (int newfd)
{

	nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	nuevoPersonaje->socket=newfd;

	pokemonActual=malloc(sizeof(t_registroPokenest));

	recibirBienvenidaEntrenador(newfd,nuevoPersonaje);

	//Recibo del planificador el quantum que me otorga y lo uso
	//for (i=0,i<quantum),quantum--);
	while(1)
		{
		recibirQueHacer(nuevoPersonaje,pokemonActual);
		}

}

//void recibirEntrenador(int newfd){
//	char* buffer = malloc(2);
//	recv(newfd, buffer, sizeof(char) * 2, 0);
//	//int a;
//
//	t_registroPersonaje *nuevoPersonaje;
//	nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
//	//memcpy(&(a), buffer, sizeof(char));
//
//	char* bufferRecortado = malloc(2);
//	strcpy(bufferRecortado, buffer);
//	str_cut(bufferRecortado,0,1);
//	memcpy((nuevoPersonaje->identificador), bufferRecortado,  sizeof(char));
//
//	                    //----RECIBO OBJETIVOS
//	recv(newfd, nuevoPersonaje->objetivos,sizeof(char)*7,0);
//	//puts(nuevoPersonaje->objetivos);
//	nuevoPersonaje->socket=newfd;
//	//printf("reciving char: %c\n", a);
//	//printf("reciving char: %c\n", nuevoPersonaje->identificador);
//	nuevoPersonaje->ultimoRecurso = 0;
//	nuevoPersonaje->x = 1;
//	nuevoPersonaje->y = 1;
//	//char id = (nuevoPersonaje->identificador)[0];
//    CrearPersonaje(items, nuevoPersonaje->identificador, nuevoPersonaje->x, nuevoPersonaje->y);
//    pthread_mutex_lock(&mutex_EntrenadoresActivos);
//    list_add(entrenadoresActivos, nuevoPersonaje);
//    pthread_mutex_unlock(&mutex_EntrenadoresActivos);
//    free(buffer);
////    free(nuevoPersonaje);
//    free(bufferRecortado);
//    //nuevoPersonaje = NULL;
//
//}

int main(int argc, char **argv)
{
	int rows;
	int cols;
	listaPokenest = malloc(sizeof(t_registroPokenest));
	listaPokenest = list_create();
	items = list_create();
	//nivel_gui_inicializar();
	//nivel_gui_get_area_nivel(&rows, &cols);
	entrenadoresActivos = malloc(sizeof(t_list));
	entrenadoresBloqueados = malloc (sizeof(t_list));

		/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//		log_info(logger, PROGRAM_DESCRIPTION);

	  infoMapa = malloc(sizeof(mapa_datos));
	  if (leerConfiguracionMapa (listaPokenest) == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");

	//  nivel_gui_inicializar();
	  	//	nivel_gui_get_area_nivel(&rows, &cols);
	  	//	nivel_gui_dibujar(items,&argv);
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
    	IniciarSocketServidor(atoi(infoMapa->puertoEscucha));
    	newfd = AceptarConexionCliente(socketServidor);
    	printf("El cliente nuevo se ha conectado por el socket %d\n", newfd);

//    	pthread_t idHilo;
//    	pthread_create (&idHilo,NULL,(void*)funcionDelThread,(void*)newfd);
//    	pthread_join(idHilo,0);
    	funcionDelThread(newfd);

    	//while(1)
    		// nivel_gui_dibujar(items, argv );

 return 0;
}
