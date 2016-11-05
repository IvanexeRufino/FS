#include "Mapa.h"

char rutaArgv[100];
pid_t pid;

t_list* listaPokenest;
t_list* items;

t_list* entrenadores_listos;
t_list* entrenadores_bloqueados;

pthread_mutex_t mutex_EntrenadoresActivos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_threadAEjecutar = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_bloqPlanificador = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_entrenadorEnEjecucion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Ejecucion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_siguienteQuantum = PTHREAD_MUTEX_INITIALIZER;

//manejo de pokenests
t_dictionary *available;
t_dictionary *request;
t_dictionary *alloc;
//detección de deadlock
int systemInDeadlock = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


t_registroPersonaje *hiloEscucha;
mapa_datos* infoMapa;

int filas, columnas;
t_log* logger;

sem_t colaDeListos;
sem_t pasoDeEntrenador;
sem_t turnoMain;

int threadAEjecutar;


void liberar_recurso(char*, char*, int);
char *liberar_recursos(char*);
int asignar_recurso(char*, char*, int);
t_pokemon* leerDatosBill(char* , char*);

t_pokemon* pokemonMasFuerteDe(t_registroPersonaje *personaje){
	t_pokemon* masFuerte = malloc(sizeof(t_pokemon));
	t_pokemon* actual = malloc(sizeof(t_pokemon));
	masFuerte = NULL;
	char ruta[300];
	char aux[300];
	strcpy(ruta,rutaArgv);
	strcat(ruta,"/Entrenadores/");
	strcat(ruta,personaje->nombre);
	strcat(ruta,"/DirdeBill/");
	strcpy(aux,ruta);
	DIR *dp;
			struct dirent *ep;
			dp = opendir (ruta);
				if (dp != NULL)
				{
					ep = readdir (dp);
				while (ep){

					  if(ep->d_name[0]!='.' && ep->d_name[0]!='m'){
						puts(ep->d_name);
						  actual = leerDatosBill(ep->d_name , ruta);
						  puts(actual->species);
						  if(masFuerte == NULL || actual->level > masFuerte->level)
						  {

							  masFuerte = actual;
						  }

					  }
					  ep = readdir (dp);
				}
					  (void) closedir (dp);

				}

			else
				perror ("Couldn't open the directory");
	log_info(logger,"El pokemon mas fuerte de %s es : %s %d",personaje->nombre,masFuerte->species,masFuerte->level);
	return masFuerte;
}
t_pokemon* leerDatosBill(char* nombre , char* ruta){
	char* pokemonNombre = string_new();
	puts("hola1");
	strcpy(pokemonNombre,nombre);
	int i = strlen(pokemonNombre) -7;
	puts("hola2");
	pokemonNombre[i] = '\0';
	puts("hola4");
	t_pokemon* actual = malloc(sizeof(t_pokemon));
	puts("hola3");
	strcat(ruta,nombre);
	puts(ruta);
	t_config* config = config_create(ruta);
	if (config_has_property(config, "Nivel")) {
		actual->level = config_get_int_value(config, "Nivel");
		strcpy(actual->species,pokemonNombre);
	}
	puts(actual->species);
	return actual;
}

void recuperarPokemonDeEntrenador(t_registroPersonaje *personaje)
{
	log_info(logger,"Recuperando Pokemons de %s", personaje->nombre);
	char origen[300];
	char destino[300];
	char comando[500];
	char pokemon[30];

	strcpy(origen,rutaArgv);
	strcat(origen,"/Entrenadores/");
	strcat(origen,personaje->nombre);
	strcat(origen,"/DirdeBill/");

	strcpy(destino, rutaArgv);
	strcat(destino,"/Mapas/");
	strcat(destino, infoMapa->nombre);
	strcat(destino, "/PokeNests/");

//	puts(personaje->nombre);
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
//			    	puts(ep->d_name);
			    	strcpy(comando,"cp -r ");
			    	strcat(comando,  origen);
			    	strcat(comando,  pokemon);
			    	strcat(comando,  " ");
			    	strcat(comando, destino);
			    	strcat(comando, ep->d_name);
			    	strcat(comando, "/");
//			    	puts(comando);
			    	system(comando);
			    	strcpy(comando, "rm ");
			    	strcat(comando, origen);
			    	strcat(comando, pokemon);
//			    	puts(comando);
			    	system(comando);
			    }
			    ep = readdir (dp);
			}
	(void) closedir (dp);
	  }
		else
			perror ("Couldn't open the directory");
	log_info(logger,"Pokemons Recuperados");
}


void copiarPokemonAEntrenador(t_registroPersonaje *personaje, t_registroPokenest* pokenest)
{
	log_info(logger," Entregando Pokemon %s, a %s",pokenest->nombre, personaje->nombre);
	char origen[300];
	char destino[300];
	char comando[500];
	strcpy(origen, rutaArgv);
	strcat(origen, "/Mapas/");
	strcat(origen, infoMapa->nombre);
	strcat(origen, "/PokeNests/");
	strcat(origen, pokenest->nombre);

//	puts(personaje->nombre);
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
//		puts(comando);
		system(comando);

		strcpy(comando, "rm ");
		strcat(comando, origen);
//		puts(comando);
		system(comando);
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

		log_info(logger," El nombre del mapa es: %s su tiempoChequeoDeadlock es: %d su batalla es: %d "
				"su algoritmo es: %s su quantum es de: %d su retardo es: %d su ipEscucha es: %s "
				"su puertoEscucha es: %s"
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
	log_info(logger, "Se Solicito Releer los datos del Mapa");
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

		log_info(logger, " El nombre del mapa es: %s su tiempoChequeoDeadlock es: %d su batalla es: %d "
				"su algoritmo es: %s su quantum es de: %d su retardo es: %d su ipEscucha es: %s "
				"su puertoEscucha es: %s"
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

void leerConfiguracionPokenest(char mapa[20], char pokemon[256])
{
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

		log_info(logger, " El Tipo del Nest es: %s su posicion es X: %d Y es: %d "
							"su identificador: %c Y hay %d Pokemons de ese Tipo"
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

char recibirBienvenidaEntrenador(int newfd,t_registroPersonaje *nuevoPersonaje)
{
	nuevoPersonaje->socket=newfd;	// Lleno con el campo del socket donde me voy a comunicar
	log_info(logger,"Se conecto con el cliente por el socket %d ", newfd);

	int x = recibirCoordenada(nuevoPersonaje->socket);
	nuevoPersonaje->x=x;					// Lleno con el campo de la ubicacion en X

	int y = recibirCoordenada(nuevoPersonaje->socket);
	nuevoPersonaje->y=y;					// Lleno con el campo de la ubicacion en Y

	char nombre[40];
	recv(newfd,nombre,sizeof(nombre),0);
	strcpy(nuevoPersonaje->nombre,nombre);	// Lleno con el campo del nombre

	log_info(logger, "La coordenada INICIAL del Entrenador %s es X: %d Y: %d",nuevoPersonaje->nombre, nuevoPersonaje->x, nuevoPersonaje->y);

	char* buffer = string_new();
	recv(newfd,buffer,sizeof(buffer),0);

	char bufferConAccion;
	bufferConAccion=buffer[0];
	nuevoPersonaje->identificador=buffer[1];	// Lleno con el campo con el identificador
	nuevoPersonaje->distanciaARecurso = -1;
	log_info(logger,"*BIENVENIDA* Recibi de %s: %s,con ID %c y accion: %c",nuevoPersonaje->nombre,buffer,nuevoPersonaje->identificador,bufferConAccion);
	//CrearPersonaje(items, nuevoPersonaje->identificador , nuevoPersonaje->x, nuevoPersonaje->y);
	//nivel_gui_dibujar(items,infoMapa->nombre);
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

int distanciaAProxObjetivo(t_registroPersonaje* pj, char obj)
{
	t_registroPokenest* pok= malloc(sizeof(t_registroPokenest));
	cargoDatosPokemonActual(obj, pok);
	return (abs(pj->x - pok->x) + abs(pj->y - pok->y));
}

int calcularMasCercanoASuObjetivo ()
{
	t_registroPersonaje* entrenador = (t_registroPersonaje*)list_get(entrenadores_listos, 0);
	t_registroPersonaje* entrenadorMinimo = entrenador;
	int distanciaMinima= distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo);
	int i;
	int indice = 0;
	for(i=0; i < list_size(entrenadores_listos); i++)
	{
		entrenador = list_get(entrenadores_listos,i);
		if(distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo)<distanciaMinima && entrenador->estado == 'L')
		{
			entrenadorMinimo = entrenador;
			distanciaMinima = distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo);
			indice = i;
		}
	}
	return indice;
}
void mover (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual)
{
	if(personaje->ultimoRecurso == 1) //ultimo movimiento fue en Y => me muevo en X
	{
		if(personaje->x <= personaje->pokemonActual->x)
			personaje->x +=1;
		else
			personaje->x -=1;
	personaje->ultimoRecurso = 0;
	}
	else{
		if(personaje->y <= personaje->pokemonActual->y)
			personaje->y +=1;
		else
			personaje->y -=1;
	 personaje->ultimoRecurso = 1 ;
	}
	if(personaje->x == personaje->pokemonActual->x) personaje->ultimoRecurso = 0;
	if(personaje->y == personaje->pokemonActual->y) personaje->ultimoRecurso = 1;
	//MoverPersonaje(items, personaje->identificador, personaje->x, personaje->y );
	//nivel_gui_dibujar(items,infoMapa->nombre);
	enviarCoordenada(personaje->x,personaje->socket);
	enviarCoordenada(personaje->y,personaje->socket);
	log_info(logger, "*MOVER* Se mueve %c (%s) por la coordenada X: %d Y: %d para llegar a: %c \n",personaje->identificador,personaje->nombre,personaje->x,personaje->y,pokemonActual->identificador);
}

void envioQueSeAtrapoPokemon (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual)
{
	log_info(logger, "El personaje %s solicitó el recurso %c", personaje->nombre, pokemonActual->identificador);
	printf("%d",personaje->pokemonActual->cantidadDisp);
	int _has_symbol(t_registroPokenest *r) {
		return (r->identificador == personaje->proximoObjetivo);
	}
	t_registroPokenest *pokenest = list_find(listaPokenest, (void*) _has_symbol);
	int cant = 1;
	if(asignar_recurso(pokenest->nombre, personaje->nombre, cant) == true) {
		log_info(logger, "/*--------------------El Personaje: %c , atrapo al pokemon %c --------------------*/ \n",personaje->identificador, personaje->pokemonActual->identificador);
		copiarPokemonAEntrenador(personaje,personaje->pokemonActual);
		char* buffer = string_new();
		string_append(&buffer,string_itoa(1));
		send(personaje->socket,buffer, sizeof(buffer), 0);
		personaje->distanciaARecurso = -1;
		restarRecurso(items, pokenest->identificador);
		pokemonMasFuerteDe(personaje);////////////////////////////////////////////ACA!
	} else {
		personaje->estado = 'B';
		char* buffer = string_new();
		string_append(&buffer,string_itoa(0));
		send(personaje->socket,buffer, sizeof(buffer), 0);
		log_info(logger, "La Pokenest esta vacia.");
	}
}


void recibirQueHacer(t_registroPersonaje *nuevoPersonaje)
{

	char* buffer = string_new();
	recv(nuevoPersonaje->socket,buffer,sizeof(buffer),0);
	char bufferConAccion;        //Vendria a ser el header
	bufferConAccion=buffer[0];
	nuevoPersonaje->accion=bufferConAccion;
	char* payload = string_new();
	payload =string_duplicate(buffer);
	str_cut(payload,0,1);
	log_info(logger,"Recibio para hacer: %s Header: %c Payload: %s",buffer,bufferConAccion,payload);

	int coordenadaX,coordenadaY;

	switch(bufferConAccion)
	{
	case ('1'):

		cargoDatosPokemonActual(payload[0],nuevoPersonaje->pokemonActual);
		nuevoPersonaje->proximoObjetivo = payload[0];
		coordenadaX = enviarCoordenada((nuevoPersonaje->pokemonActual)->x,nuevoPersonaje->socket);				//Envio coordenada del pokemon en X

		coordenadaY = enviarCoordenada((nuevoPersonaje->pokemonActual)->y,nuevoPersonaje->socket);				//Envio coordenada del pokemon en Y
		log_info(logger,"*ENVIAR POSICION INICIAL* La coordenada de: %c (%s) en X: %d Y: %d \n",nuevoPersonaje->identificador,nuevoPersonaje->nombre,coordenadaX,coordenadaY);
		nuevoPersonaje->distanciaARecurso = 1;
		break;


	case ('2'):

		mover(nuevoPersonaje,nuevoPersonaje->pokemonActual);

		break;

	case ('3'):

		//Si hay deadlock lo meto en la entrenadores_bloqueados

		envioQueSeAtrapoPokemon(nuevoPersonaje,nuevoPersonaje->pokemonActual);

		break;

	case ('\0'):
	case ('4'):
		nuevoPersonaje->proximoObjetivo = '0';
		recuperarPokemonDeEntrenador(nuevoPersonaje);
		nuevoPersonaje->estado='T';
		liberar_recursos(nuevoPersonaje->nombre);
		close(nuevoPersonaje->socket);
		//sem_wait(&nuevoPersonaje->finTurno);
		//pthread_exit(0);
		break;

	}

}


void ejecutar_Entrenador(parametros_entrenador* param)
{
	 t_registroPersonaje* nuevoPersonaje;
	 nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	 nuevoPersonaje->pokemonActual=malloc(sizeof(t_registroPokenest));
	 nuevoPersonaje->threadId = param->idHilo;
	 sem_init(&nuevoPersonaje->comienzoTurno,1,0);
	 sem_init(&nuevoPersonaje->finTurno,1,0);
	 char estaListoParaJugar = recibirBienvenidaEntrenador(param->newfd,nuevoPersonaje);	//Realizo el handshake

 	if (estaListoParaJugar == '0')
	{
 		log_info(logger,"Se Conecto un entrenador Nuevo");

 		nuevoPersonaje->estado='L';
		pthread_mutex_lock(&mutex_EntrenadoresActivos);
		list_add(entrenadores_listos,nuevoPersonaje);
		log_info(logger,"Ahora hay %d entrenadores", list_size(entrenadores_listos));
		pthread_mutex_unlock(&mutex_EntrenadoresActivos);
		sem_post(&colaDeListos);

	}
		else
	{
		log_info(logger, "Nose para que te conectaste si no queres jugar xD ");		// No deberia tirar nunca este printf
	}
 	return;
 	sem_wait(&nuevoPersonaje->finTurno);
 	pthread_exit(0);
}

void planificarNuevo()
{
	int i,j;
	i=0;

	while(1)


	{
//		int cantidadEntrenadores = entrenadores_listos->elements_count;
//		for(i=0;i<cantidadEntrenadores;i++)
//		{
//
//			cantidadEntrenadores = entrenadores_listos->elements_count;
			sem_wait(&colaDeListos);
			t_registroPersonaje* entrenador = malloc(sizeof(t_registroPersonaje));

//			sem_init(&(entrenador->ejecutar1),1,0);
//			sem_init(&(entrenador->ejecutar2),1,0);
			j=0;
			if(!strcmp(infoMapa->algoritmo,"RR"))
			{
					pthread_mutex_lock(&mutex_EntrenadoresActivos);
					int e;
					for(e=0;list_size(entrenadores_listos)>e;e++){
						entrenador = list_remove(entrenadores_listos,0);
						if(entrenador->estado == 'B')
							list_add(entrenadores_listos,entrenador);
						else break;
					}
					pthread_mutex_unlock(&mutex_EntrenadoresActivos);
				for(i=0;i!=infoMapa->quantum && j == 0 && entrenador->estado == 'L' ;i++)
				{
						recibirQueHacer(entrenador);
						if(entrenador->estado == 'T')
							j = 1;
				}
			}
			else  //NOT RR
					{
						int k=0;
						while(k<list_size(entrenadores_listos)){
							pthread_mutex_lock(&mutex_EntrenadoresActivos);
							entrenador =  list_get(entrenadores_listos,k);
							if(entrenador->distanciaARecurso == -1){
								entrenador =  list_remove(entrenadores_listos,k);
								recibirQueHacer(entrenador);
								list_add(entrenadores_listos,entrenador);
							}
							pthread_mutex_unlock(&mutex_EntrenadoresActivos);
							k++;
						}

						int pos;
						pthread_mutex_lock(&mutex_EntrenadoresActivos);
						pos = calcularMasCercanoASuObjetivo();
						entrenador =  list_remove(entrenadores_listos,pos);
						log_info(logger,"El entrenador mas cercano es %c" , entrenador->identificador);
						pthread_mutex_unlock(&mutex_EntrenadoresActivos);
						char objetivoActual = entrenador->proximoObjetivo;
						while(objetivoActual == entrenador->proximoObjetivo && entrenador->estado == 'L')
						{
							if(entrenador->estado != 'T')
							{
								recibirQueHacer(entrenador);
							}
						}
					}
				if(entrenador->estado!='T')
					{
						pthread_mutex_lock(&mutex_EntrenadoresActivos);
						list_add(entrenadores_listos,entrenador);
						pthread_mutex_unlock(&mutex_EntrenadoresActivos);
						if(entrenador->estado == 'L')
							sem_post(&colaDeListos);  //Agrego este if para que el mapa no se quede loopeando si estan bloqueados los entrenadores
					}															//el sem_post deberia llamarse cuando el deadlock lo diga;
	}
}


void releerconfig(int aSignal)
{
	 reLeerConfiguracionMapa();
	 signal(SIGUSR2,releerconfig);
	 return ;
}

void sumarRecurso(t_list* items, char id) {
    ITEM_NIVEL* item = _search_item_by_id(items, id);

    if (item != NULL) {
        item->quantity = item->quantity > 0 ? item->quantity + 1 : 0;
    } else {
        printf("WARN: Item %c no existente\n", id);
    }
}

char *liberar_recursos(char *nombre_personaje){
	char *recursosString = string_new();
	int liberados = 0, rc, totalLiberados = 0;

	 //lockeo el mutex
	 rc = pthread_mutex_lock(&mutex);

	/********** Critical Section *******************/

	void _list_recursos(t_registroPokenest *r) {
		int i;
		liberados = (int)dictionary_get(dictionary_get((t_dictionary*)alloc, r->nombre), nombre_personaje);
		liberar_recurso(r->nombre, nombre_personaje, liberados);
		if (liberados > 0) {
			string_append(&recursosString, string_from_format("%c|%d|", r->identificador, liberados));
			totalLiberados++;

			for (i = 0; i < liberados; i++) {
				sumarRecurso(items, r->identificador);
			}
		}

	}
	list_iterate(listaPokenest, (void*) _list_recursos);

	/********** end Critical Section *******************/
    rc = pthread_mutex_unlock(&mutex);
   	char *str = string_new();

   	if (strlen(recursosString) > 0) {
		//Informo los recursos liberados
    	recursosString[strlen(recursosString)-1] = '\0';
    	str = string_from_format("%d|%s", totalLiberados, recursosString);
		log_debug(logger, "Recursos liberados: %s", str);
    } else {
    	str = string_from_format("%d", 0);
    	log_debug(logger, "No se liberaron recursos");
    }
	return recursosString;
}

void liberar_recurso(char *recurso, char *personaje, int cant) {
	int liberados = 0, availables = 0;

	liberados = (int)dictionary_get(dictionary_get((t_dictionary*)alloc, recurso), personaje);

	//saco los recursos alocados.
	dictionary_remove(dictionary_get((t_dictionary*)alloc, recurso), personaje);

	//Incremento los recursos disponibles.
	availables = (int)dictionary_get(available, recurso);
	dictionary_remove(available, recurso);
	dictionary_put(available,recurso, (availables + liberados));

	//saco los recursos de request
	dictionary_remove(dictionary_get(request, recurso), personaje);

}


int asignar_recurso(char *pokenest, char *personaje, int cant) {
	int asignados = 0, availables = 0, requested = 0, rc;
	int pudo_asignar = 0;

   //lockeo el mutex
   rc = pthread_mutex_lock(&mutex);

    /********** Critical Section *******************/

	if ((int)dictionary_get(available, pokenest) >= cant) {
		//agrego los pokenests a alloc
		asignados = (int)dictionary_get(dictionary_get(alloc, pokenest), personaje);
		dictionary_remove(dictionary_get(alloc, pokenest), personaje);
		dictionary_put(dictionary_get(alloc, pokenest), personaje, asignados + cant);

		//saco los pokenests de available
		availables = (int)dictionary_get(available, pokenest);
		dictionary_remove(available, pokenest);
		dictionary_put(available, pokenest, availables - cant);

		//si tenia a ese pokenest en request, lo saco
		requested = (int)dictionary_get(dictionary_get(request, pokenest), personaje);
		if (requested > 0) {
			dictionary_remove(dictionary_get(request, pokenest), personaje);
			dictionary_put(dictionary_get(request, pokenest), personaje, requested - cant);
		}

		pudo_asignar = 1;
	} else {
		//actualizo matriz request
		requested = (int)dictionary_get(dictionary_get(request, pokenest), personaje);
		dictionary_remove(dictionary_get(request, pokenest), personaje);
		dictionary_put(dictionary_get(request, pokenest), personaje, requested + cant);
	}

	/********** end Critical Section *******************/
    rc = pthread_mutex_unlock(&mutex);

	return pudo_asignar;
}

// Funcion que detecta si existen personajes interbloqueados
void *detectar_interbloqueo(void *milis) {
	t_dictionary *W;
	int rc;
	/*
	 * Loop principal del hilo. Ejecuta el algoritmo para detectar deadlock entre personajes
	 * Envia un mensaje al proceso en caso de detectarlo.
	 */
	while (1) {
		log_info(logger,"Detectando interbloqueo");
		//hago el chequeo solo si no está en deadlock. Si lo está espero a que se resuelva.
	//	if (!systemInDeadlock) {

			//lockeo el mutex
		   rc = pthread_mutex_lock(&mutex);

		   /********** Critical Section *******************/

			// Inicializo todos los personajes como no marcados y
			// marco todos los personajes que no tengan alocado ningun recurso
			void _list_personajes(t_registroPersonaje *p) {
				p->marcado = false;
				int cant_recursos = 0;

				void _list_recursos1(t_registroPokenest *r) {
					cant_recursos += (int)dictionary_get(dictionary_get(alloc, r->nombre), p->nombre);
				}
				list_iterate(listaPokenest, (void*) _list_recursos1);

				if (cant_recursos == 0) {
					p->marcado = true;
				}
			}
			list_iterate(entrenadores_listos, (void*) _list_personajes);

			//Inicializo el vector W, copia de available.
			W = dictionary_create();
			void _list_recursos4(t_registroPokenest *r) {
				dictionary_put(W, r->nombre, (void*)dictionary_get(available, r->nombre));
			}
			list_iterate(listaPokenest, (void*) _list_recursos4);


			bool disponible;
			void _list_personajes2(t_registroPersonaje *p) {
				disponible = true;

				if (p->marcado == false) {
					void _list_recursos2(t_registroPokenest *r) {
						if ((int)dictionary_get(dictionary_get(request, r->nombre), p->nombre) > (int)dictionary_get(W, r->nombre)) {
							disponible = false;

						}
					}
					list_iterate(listaPokenest, (void*) _list_recursos2);


					if (disponible == true) {
						p->marcado = true;

						// W + allocated para ese personaje
						int nueva_cantidad = 0;
						void _list_recursos3(t_registroPokenest *r) {
							nueva_cantidad = (int)dictionary_get(W, r->nombre) + (int)dictionary_get(dictionary_get((t_dictionary*)alloc, r->nombre), p->nombre);
							dictionary_remove(W, r->nombre);
							dictionary_put(W, r->nombre, (void*)nueva_cantidad);
						}
						list_iterate(listaPokenest, (void*) _list_recursos3);

					}
				}
			}

			list_iterate(entrenadores_listos, (void*) _list_personajes2);



			// chequeo si existen personajes sin marcar == interbloqueo y envio señal al proceso
			int bloqueados = 0;
			char *str = string_new();
			void _list_personajes3(t_registroPersonaje *p) {
					if (p->marcado == false) {
						bloqueados++;
						string_append(&str, p->nombre);
						string_append(&str, "|");

					}
			}

			list_iterate(entrenadores_listos, (void*) _list_personajes3);

			/********** end Critical Section *******************/
		    rc = pthread_mutex_unlock(&mutex);

			str[strlen(str)-1] = '\0';

			if (bloqueados > 1) {
				//Batalla pokemon---------------------------------------------------------//
				log_info(logger,"Se ha detectado un interbloqueo! %s", str);
				void pokemonsDeBloqueados(t_registroPersonaje *p) {
						if (p->marcado == false) {
							pokemonMasFuerteDe(p);
					}
				}
				list_iterate(entrenadores_listos, (void*) pokemonsDeBloqueados);
			}
			else{
				void desbloquear(t_registroPersonaje *p) {
					if(p->estado == 'B'){
						p->estado = 'L';
						sem_post(&colaDeListos);
					}
				}
				list_iterate(entrenadores_listos, (void*) desbloquear);
				log_info(logger,"no hay interbloqueo!");
			}

			dictionary_destroy(W);
//		}
		usleep(((int)milis*1000));
	}
}

int main(int argc, char **argv)
{
	filas = 30;
	columnas =30;
	items = list_create();									//Para usar despues en las cajas
	sem_init(&(pasoDeEntrenador),1,0);
	/* Inicializacion y registro inicial de ejecucion */
	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	log_info(logger, PROGRAM_DESCRIPTION);
	  infoMapa = malloc(sizeof(mapa_datos));
	if(argc == 1)
	{
		log_info(logger, "Cantidad de parametros incorrectos, Aplicando por defecto");
		strcpy(infoMapa->nombre,"PuebloPaleta");
		strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");
	}
	else if(argc==3)
	{
		log_info(logger, "Cantidad de parametros CORRECTOS");
		strcpy(infoMapa->nombre,argv[1]);
		strcpy(rutaArgv, argv[2]);
	}
	else if(argc==2)
	{
		log_info(logger,"Cantidad de parametros Incorrectos, aplicando por defecto para la ruta");
		strcpy(infoMapa->nombre,argv[1]);
		strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");
	}
	log_info(logger, "Nombre Mapa: %s Ruta: %s", infoMapa->nombre, rutaArgv);
	listaPokenest = malloc(sizeof(t_registroPokenest));
	listaPokenest = list_create();
	pid = getpid();
	log_info(logger, "El PID del proceso Mapa es %d\n", pid);
	signal(SIGUSR2,releerconfig);//Por consola kill -SIGUSR2 -PID

	entrenadores_listos = malloc(sizeof(t_list));
	entrenadores_listos = list_create();
	entrenadores_bloqueados = malloc (sizeof(t_list));
	entrenadores_bloqueados = list_create();

    // Inicializo matrices de pokenests
    available = dictionary_create();
    request = dictionary_create();
    alloc = dictionary_create();

	  if (leerConfiguracionMapa () == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");

	  //pokenests
	  		void _list_elements(t_registroPokenest *r) {
	  			dictionary_put(available, r->nombre, (void*)r->cantidadDisp);
	  			dictionary_put(request, r->nombre, dictionary_create());
	  			dictionary_put(alloc, r->nombre, dictionary_create());
	  		}

	  		list_iterate(listaPokenest, (void*) _list_elements);

	  //nivel_gui_inicializar();
		//nivel_gui_get_area_nivel(&filas, &columnas);
		//nivel_gui_dibujar(items,infoMapa->nombre);

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
//	  hiloEscucha = malloc(sizeof(t_registroPersonaje));
//	  hiloEscucha->estado = 'M';
//	  sem_init(&colaDeListos, 1,0);
//	  sem_init(&hiloEscucha->comienzoTurno,1,0);
//	  sem_init(&hiloEscucha->finTurno,1,0);
//	  list_add(entrenadores_listos, hiloEscucha);
//	  sem_post(&colaDeListos);



	  int socketServidor;
	  int newfd;
	      socketServidor = crearSocketServidor(infoMapa->puertoEscucha);
	      IniciarSocketServidor(atoi(infoMapa->puertoEscucha));
	      pthread_t hiloPlanificador;

	      // Creo hilo para chequeo de Interbloqueo
	     	pthread_t hiloDeadlock;
	     	int milis = infoMapa->tiempoChequeoDeadlock;
     		pthread_create(&hiloDeadlock, NULL, detectar_interbloqueo, (void *) milis);


	      pthread_create (&hiloPlanificador,NULL,(void*)planificarNuevo,NULL);

	     //Hacemos un while 1 porque siempre queremos recibir conexiones entrantes
	     //Y ademas creamos un hilo para que mientras que escuche conexiones nuevas, me delegue lo que llego para trabajar

	     while(1)
	     {
	    	 //sem_wait(&hiloEscucha->comienzoTurno);

	    	 newfd = AceptarConexionCliente(socketServidor);




	    	 //No quiero quedarme esperando a que termine de hacer lo del hilo, por eso no pongo el join
	    	 //Quiero seguir escuchando conexiones entrantes

	    		 	 	 pthread_t idHilo;

	    		 	 	 parametros_entrenador* param = malloc(sizeof(parametros_entrenador));
	    		 	 	 param->idHilo = idHilo;
	    		 	 	 param->newfd = newfd;
	    		 	 	 pthread_create (&idHilo,NULL,(void*)ejecutar_Entrenador,param);
	    		 	 	 //sem_post(&hiloEscucha->finTurno);
	    	  //pthread_join(&idHilo,0);



	     }
log_info(logger, "Se finalizaron las operaciones con todos los entrenadores que estaban conectados");
log_info(logger, "-----El proceso mapa se cerrara, gracias por jugar-----");
 return 0;
}
