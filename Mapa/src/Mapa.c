#include "Mapa.h"
char rutaArgv[100];
pid_t pid;

t_list* listaPokenest;
t_list* items;

t_list* entrenadores_listos;
t_list* entrenadores_bloqueados;

pthread_mutex_t mutex_EntrenadoresActivos = PTHREAD_MUTEX_INITIALIZER;

t_registroPokenest *pokemonActual;
t_registroPersonaje *nuevoPersonaje;
mapa_datos* infoMapa;

int filas, columnas;
t_log* logger;

sem_t colaDeListos;

void recuperarPokemonDeEntrenador(t_registroPersonaje *personaje){
	char origen[300];
	char destino[300];
	char comando[500];
	char pokemon[30];

	strcpy(origen,rutaArgv);
	strcat(origen,"/Entrenadores/");
	strcat(origen,personaje->nombre);
	strcat(origen,"/DirdeBill/");

	strcpy(destino, rutaArgv);
	strcpy(destino,"/Mapas/");
	strcat(destino, infoMapa->nombre);
	strcat(destino, "/PokeNests/");


	DIR *dp;
	struct dirent *ep;
	dp = opendir (origen);
	if (dp != NULL)
	  {
		ep = readdir (dp);
			while (ep)
			{
			    if(ep->d_name[0]!='.' && ep->d_name[0]!='m' ){
			    	strcpy(pokemon,ep->d_name);
			    	int i = strlen(ep->d_name) -7;
			    	ep->d_name[i] = '\0';
			    	puts(ep->d_name);
			    	strcpy(comando,"cp -r ");
			    	strcat(comando,  origen);
			    	strcat(comando,  pokemon);
			    	strcat(comando,  " ");
			    	strcat(comando, destino);
			    	strcat(comando, ep->d_name);
			    	strcat(comando, "/");
			    	system(comando);
			    	strcpy(comando, "rm ");
			    	strcat(comando, origen);
			    	strcat(comando, pokemon);
			   	system(comando);
			    }
			    ep = readdir (dp);
			}
	(void) closedir (dp);
	  }
		else
			perror ("Couldn't open the directory");
}


void copiarPokemonAEntrenador(t_registroPersonaje *personaje, t_registroPokenest* pokenest){
	char origen[300];
	char destino[300];
	char comando[500];
	strcpy(origen, rutaArgv);
	strcat(origen, "/Mapas/");
	strcat(origen, infoMapa->nombre);
	strcat(origen, "/PokeNests/");
	strcat(origen, pokenest->nombre);

	strcpy(destino,rutaArgv);
	strcat(destino,"/Entrenadores/");
	strcat(destino,personaje->nombre);
	strcat(destino,"/DirdeBill/");


	DIR *dp;
	struct dirent *ep;
	dp = opendir (origen);
	if (dp != NULL)
	  {
		ep = readdir (dp);
			while (ep)
			{
			    if(ep->d_name[0]!='.' && ep->d_name[0]!='m' ){
//   puts(ep->d_name);
			    	break;
			    }
			    ep = readdir (dp);
			}
	strcat(origen,"/");
	strcat(origen, ep->d_name);
	(void) closedir (dp);
	  }
		else
			perror ("Couldn't open the directory");

		strcpy(comando, "cp -r ");
		strcat(comando,origen);
		strcat(comando, " ");
		strcat(comando, destino);
		system(comando);

		strcpy(comando, "rm ");
		strcat(comando, origen);
		system(comando);
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int leerConfiguracionMapa()
{
	char pathconfigMetadata[256];
	strcpy(pathconfigMetadata, rutaArgv);
	strcat(pathconfigMetadata,"/Mapas/");
	char path[256];
	strcat(pathconfigMetadata, infoMapa->nombre);
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
				ep = readdir (dp);
			while (ep){

				  if(ep->d_name[0]!='.'){
//					  puts (ep->d_name);
					  leerConfiguracionPokenest(infoMapa->nombre,ep->d_name);
				  }
				  ep = readdir (dp);
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


int reLeerConfiguracionMapa()
{
	char pathconfigMetadata[256];
	strcpy(pathconfigMetadata, rutaArgv);
	strcat(pathconfigMetadata,"/Mapas/");
	char path[256];
	strcat(pathconfigMetadata, infoMapa->nombre);
	strcpy(path,pathconfigMetadata);
	strcat(path, "/PokeNests");
	strcat(pathconfigMetadata,  "/metadata");
	t_config* config = config_create(pathconfigMetadata);
	// Verifico que los parametros tengan sus valores OK
	if (config_has_property(config, "algoritmo") && config_has_property(config, "quantum")
	&& config_has_property(config, "retardo") && config_has_property(config, "Batalla")
	&& config_has_property(config, "TiempoChequeoDeadlock"))
	{
		infoMapa->tiempoChequeoDeadlock = config_get_int_value(config, "TiempoChequeoDeadlock");
		infoMapa->batalla = config_get_int_value(config, "Batalla");
		infoMapa->algoritmo  = config_get_string_value(config, "algoritmo");
		infoMapa->quantum = config_get_int_value(config, "quantum");
		infoMapa->retardo = config_get_int_value(config, "retardo");

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
	}
	else
    {
		return -1;
    }
	return 1 ;
}

void leerConfiguracionPokenest(char mapa[20], char pokemon[256]){
	int cantidadPokemon = 0;

	char pathpokenestMetadata[256] ;
	strcpy(pathpokenestMetadata, rutaArgv);
	strcat(pathpokenestMetadata, "/Mapas/");
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
			  ep = readdir (dp);
		    while (ep){
		    	 if(ep->d_name[0]!='.' && ep->d_name[0]!='m' ){
		    		 cantidadPokemon ++;
//				      puts (ep->d_name);
		    	 }
		    	 ep = readdir (dp);
		    }

		    (void) closedir (dp);
		  }
		  else
		    perror ("Couldn't open the directory");

		  pokenest->cantidadDisp = cantidadPokemon;

	if (config_has_property(configNest, "Tipo") && config_has_property(configNest, "Identificador")){
		strcpy(pokenest->tipo,(config_get_string_value(configNest, "Tipo")));
		char* identificadorPokenest= config_get_string_value(configNest, "Identificador");
		char* array= config_get_string_value(configNest, "Posicion");
		char** pos = string_split(array, ";");
		pokenest->x = atoi(pos[0]);
		pokenest->y = atoi(pos[1]);
		pokenest->identificador=identificadorPokenest[0];
		strcpy(pokenest->nombre , pokemon);

		printf("\n El Tipo del Nest es: %s\n su posicion es X: %d\n Y es: %d\n "
							"su identificador: %c\n Y hay %d Pokemons de ese Tipo\n"
											,pokenest->tipo,pokenest->x, pokenest->y, pokenest->identificador, pokenest->cantidadDisp);


		if(pokenest->x> columnas || pokenest->y > filas)
		{
			log_error(logger, "La Pokenest %c supera los limites de filas/columnas del nivel. \n", pokenest->identificador);
			exit(1);
		}

		bool distanciaEntreCajas (t_registroPokenest* unaNest)
		{
			return (abs(unaNest->x - pokenest->x) + abs(unaNest->y - pokenest->y) <= 4);
		}

		if(list_any_satisfy(listaPokenest, (void*) distanciaEntreCajas))
		{
			log_error(logger, "La Pokenest %c no respeta las distancias con otra Pokenest. \n", pokenest->identificador);
			exit(1);
		}


	CrearCaja(items, config_get_string_value(configNest, "Identificador")[0] , pokenest->x , pokenest->y ,pokenest->cantidadDisp);
	list_add(listaPokenest,pokenest);

	}

}

int recibirCoordenada(int socketEntrenador)
{
	int coordenada;
	char* buffer = string_new();
	recv(socketEntrenador, buffer,sizeof(buffer), 0);
	char* payload = string_new();
	payload =string_duplicate(buffer);
	str_cut(payload,0,1);

	coordenada=atoi(payload);
	return coordenada;

}

char recibirBienvenidaEntrenador(int newfd,t_registroPersonaje *nuevoPersonaje)
{
	nuevoPersonaje->socket=newfd;	// Lleno con el campo del socket donde me voy a comunicar
	printf("Se conecto con el cliente por el socket %d \n", newfd);

	int x = recibirCoordenada(newfd);
	nuevoPersonaje->x=x;					// Lleno con el campo de la ubicacion en X

	int y = recibirCoordenada(newfd);
	nuevoPersonaje->y=y;					// Lleno con el campo de la ubicacion en Y

	printf("La coordenada INICIAL en X del ENTRENADOR es %d \n", nuevoPersonaje->x);
	printf("La coordenada INICIAL en Y del ENTRENADOR es %d \n", nuevoPersonaje->y);

	char nombre[40];
	recv(newfd,nombre,sizeof(nombre),0);
	strcpy(nuevoPersonaje->nombre,nombre);	// Lleno con el campo del nombre
	printf("Su nombre es: %s\n", nuevoPersonaje->nombre);

	char* buffer = string_new();
	recv(newfd,buffer,sizeof(buffer),0);
	printf("Lo que recibio del entrenador %d es esto: %s\n", newfd,buffer);

	char bufferConAccion;
	bufferConAccion=buffer[0];
	printf("La accion que pide hacer el entrenador es %c\n", bufferConAccion);

	nuevoPersonaje->identificador=buffer[1];	// Lleno con el campo con el identificador
	printf("El ID del entrenador es %c\n", nuevoPersonaje->identificador);
	return (buffer[0]);
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
					strcpy(pokemonActual->nombre, pokenest->nombre);
					pokemonActual->identificador=pokenest->identificador;
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

		copiarPokemonAEntrenador(personaje,pokemonActual);


		char* buffer = string_new();
		string_append(&buffer,string_itoa(1));
		send(personaje->socket,buffer, sizeof(buffer), 0);

	}
	else
	{
		char* buffer = string_new();
		string_append(&buffer,string_itoa(0));
		send(personaje->socket,buffer, sizeof(buffer), 0);
	}
}

void recibirQueHacer(t_registroPersonaje *nuevoPersonaje,t_registroPokenest* pokemonActual)
{
	char* buffer = string_new();
	recv(nuevoPersonaje->socket,buffer,sizeof(buffer),0);
	printf("Lo que recibio para hacer es esto: %s\n",buffer);

	char bufferConAccion;        //Vendria a ser el header
	bufferConAccion=buffer[0];
	printf("Separo el header y me queda: %c\n",bufferConAccion);

	char* payload = string_new();
	payload =string_duplicate(buffer);
	str_cut(payload,0,1);
	printf("Separo el payload y me queda esto: %s\n",payload);

	int coordenadaX,coordenadaY;

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

		//Si hay deadlock lo meto en la entrenadores_bloqueados
		envioQueSeAtrapoPokemon(nuevoPersonaje,pokemonActual);

		break;

	case ('4'):
		recuperarPokemonDeEntrenador(nuevoPersonaje);
		nuevoPersonaje->estado='T';
		list_remove(entrenadores_listos,0);
		break;
	}

}

void thread_entrenador (t_registroPersonaje* nuevoPersonaje,t_registroPokenest* pokemonActual)
{
	//Lo deberia sacar de la lista de listos, y al finalizar con su operacion ponerlo al final de la cola de listos

	switch(nuevoPersonaje->estado)
	{
	case 'L':

	nuevoPersonaje->estado = 'E';
	recibirQueHacer(nuevoPersonaje,pokemonActual);
	break;


	case 'E':

	recibirQueHacer(nuevoPersonaje,pokemonActual);
	break;

	default:
		break;

	}
}

void planificarRoundRobin()
{
	return;
}

void planificarSRDF()
{
	return;
}

void planificarEntrenador(t_registroPersonaje* entrenador)
	{

		if(!strcmp(infoMapa->algoritmo, "RR"))
				{
			planificarRoundRobin();
				}
		else
				{
			planificarSRDF();
				}
		thread_entrenador(entrenador, pokemonActual);

	return;
	}

void planificar_Entrenadores(parametros_entrenador* param)
{
	 nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	 pokemonActual=malloc(sizeof(t_registroPokenest));
	 nuevoPersonaje->threadId = param->idHilo;
	 char estaListoParaJugar = recibirBienvenidaEntrenador(param->newfd,nuevoPersonaje);	//Realizo el handshake

 	if (estaListoParaJugar == '0')
	{
 		nuevoPersonaje->estado='L';
		pthread_mutex_lock(&mutex_EntrenadoresActivos);
		list_add(entrenadores_listos,nuevoPersonaje);
		pthread_mutex_unlock(&mutex_EntrenadoresActivos);
		sem_post(&colaDeListos);
	}
		else
	{
		printf("Nose para que te conectaste si no queres jugar xD \n");		// No deberia tirar nunca este printf
	}
// 	while(nuevoPersonaje->estado !='T')
// 	{
//  		//pthread_t hiloPlanificador;
//    	//pthread_create (&hiloPlanificador,NULL,(void*)planificarEntrenador,NULL);
//    	//t_registroPersonaje entrenadorQueVaAejecutar=planificarEntrenador();	//Me devuelve cual es el entrenador que ejecuta
//    thread_entrenador(nuevoPersonaje,pokemonActual);	//tendria que mandar en un futuro el que me dice que puedo ejecutar el planificador
// 	}


 	free(pokemonActual);
 	free(nuevoPersonaje);
 }
void planificar()
	{


		sem_wait(&colaDeListos);

			int s,j;
			int cantidadEntrenadores;
			pthread_mutex_lock(&mutex_EntrenadoresActivos);
			cantidadEntrenadores = entrenadores_listos->elements_count;
			pthread_mutex_unlock(&mutex_EntrenadoresActivos);
			while(cantidadEntrenadores!=0)
			{

		for(j=0; j<cantidadEntrenadores; j++)
		{


			for(s=0; s<infoMapa->quantum; s++)
				{


					planificarEntrenador(list_get(entrenadores_listos,j));

				}
		}
			}
	}



void iniciarHiloPlanificador(pthread_t hilo)
	{


		pthread_create (&hilo,NULL,(void*)planificar,NULL);
		return;
	}



void releerconfig(int aSignal)
{
	 reLeerConfiguracionMapa();
	 signal(SIGUSR2,releerconfig);
	 return ;
}



int main(int argc, char **argv)
{
	  infoMapa = malloc(sizeof(mapa_datos));
	if(argc != 3){
		printf("Cantidad de parametros incorrectos, Aplicando por defecto \n");
		strcpy(infoMapa->nombre,"PuebloPaleta");
		strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");
	}
	else{
		strcpy(infoMapa->nombre,argv[1]);
		strcpy(rutaArgv, argv[2]);
	}
	listaPokenest = malloc(sizeof(t_registroPokenest));
	listaPokenest = list_create();
	pid = getpid();
	printf("El PID del proceso Mapa es %d\n", pid);
	signal(SIGUSR2,releerconfig);//Por consola kill -SIGUSR2 -PID
	filas = 30;
	columnas = 30;
	items = list_create();									//Para usar despues en las cajas
	//nivel_gui_inicializar();
	//nivel_gui_get_area_nivel(&filas, &columnas);
	entrenadores_listos = malloc(sizeof(t_list));
	entrenadores_listos = list_create();
	entrenadores_bloqueados = malloc (sizeof(t_list));
	entrenadores_bloqueados = list_create();

		/* Inicializacion y registro inicial de ejecucion */
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
//		log_info(logger, PROGRAM_DESCRIPTION);


	  if (leerConfiguracionMapa () == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");

	//  nivel_gui_inicializar();
	  	//	nivel_gui_get_area_nivel(&rows, &cols);
	  	//	nivel_gui_dibujar(items,&argv);

// ------------------- Descomentar para probar si cargaron bien las nests -------------------------//
//	  t_registroPokenest* pokenestPrueba = malloc(sizeof(t_registroPokenest));
//	  int ii = 0;
//	  while(ii!= list_size(listaPokenest)){
//		  pokenestPrueba = list_get(listaPokenest,ii);
//		   printf("%d",pokenestPrueba->cantidadDisp);
//		   puts(pokenestPrueba->nombre);
//		  ii++;
//	  }

    // bucle principal
	  int socketServidor;
	  int newfd;
	      socketServidor = crearSocketServidor(infoMapa->puertoEscucha);
	      IniciarSocketServidor(atoi(infoMapa->puertoEscucha));
	      pthread_t hiloPlanificador;
	      //void iniciarHiloPlanificador(hiloPlanificador);

	      pthread_create (&hiloPlanificador,NULL,(void*)planificar,NULL);
	      int ret;
	      int count = 0;

	      /* to be shared among processes */
	      ret = sem_init(&colaDeListos, 1, count);

	     //Hacemos un while 1 porque siempre queremos recibir conexiones entrantes
	     //Y ademas creamos un hilo para que mientras que escuche conexiones nuevas, me delegue lo que llego para trabajar

	     while(1)
	     {
	    	 newfd = AceptarConexionCliente(socketServidor);

	    	 //Aca conviene hacer DATACHABLES PARA QUE CUANDO TERMINE EL HILO LIBERE TODOS SUS RECURSOS
	    	 //No quiero quedarme esperando a que termine de hacer lo del hilo, por eso no pongo el join
	    	 //Quiero seguir escuchando conexiones entrantes

	    	 pthread_t idHilo;

	    	  parametros_entrenador* param = malloc(sizeof(parametros_entrenador));
	    	  param->idHilo = idHilo;
	    	  param->newfd = newfd;
	    	  pthread_create (&idHilo,NULL,(void*)planificar_Entrenadores,param);
	    	  //pthread_join(&hiloPlanificador,0);
	     }

    	//while(1)
    		// nivel_gui_dibujar(items, argv );


puts("Se finalizaron las operaciones con todos los entrenadores que estaban conectados");
puts("-----El proceso mapa se cerrara, gracias por jugar-----");
 return 0;
}
