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


t_registroPokenest *pokemonActual;
t_registroPersonaje *nuevoPersonaje;
t_registroPersonaje *hiloEscucha;
mapa_datos* infoMapa;

int filas, columnas;
t_log* logger;

sem_t colaDeListos;
sem_t pasoDeEntrenador;
sem_t turnoMain;

int threadAEjecutar;




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

t_registroPersonaje* calcularMasCercanoASuObjetivo ()
{
	t_registroPersonaje* entrenador = (t_registroPersonaje*)list_get(entrenadores_listos, 0);
	t_registroPersonaje* entrenadorMinimo = entrenador;
	int distanciaMinima= distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo);
	int i;
	int indice;
	for(i=0; i <= list_size(entrenadores_listos); i++)
	{
		entrenador = list_get(entrenadores_listos,i);
		if(distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo)<distanciaMinima)
		{
			entrenadorMinimo = entrenador;
			distanciaMinima = distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo);
			indice = i;
		}
	}
	return entrenadorMinimo;
}
void mover (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual)
{
	if(personaje->ultimoRecurso == 1) //ultimo movimiento fue en Y => me muevo en X
	{
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
	//MoverPersonaje(items, personaje->identificador, personaje->x, personaje->y );
	//nivel_gui_dibujar(items,infoMapa->nombre);
	enviarCoordenada(personaje->x,personaje->socket);
	enviarCoordenada(personaje->y,personaje->socket);
	log_info(logger, "*MOVER* Se mueve %c (%s) por la coordenada X: %d Y: %d para llegar a: %c \n",personaje->identificador,personaje->nombre,personaje->x,personaje->y,pokemonActual->identificador);
}

void envioQueSeAtrapoPokemon (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual)
{
	if(pokemonActual->cantidadDisp >= 1)
	{
		pokemonActual->cantidadDisp --;
		log_info(logger, "/*--------------------El Personaje: %c , atrapo al pokemon %c --------------------*/ \n",personaje->identificador, pokemonActual->identificador);

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

		cargoDatosPokemonActual(payload[0],pokemonActual);
		nuevoPersonaje->proximoObjetivo = payload[0];
		coordenadaX = enviarCoordenada(pokemonActual->x,nuevoPersonaje->socket);				//Envio coordenada del pokemon en X

		coordenadaY = enviarCoordenada(pokemonActual->y,nuevoPersonaje->socket);				//Envio coordenada del pokemon en Y
		log_info(logger,"*ENVIAR POSICION INICIAL* La coordenada de: %c (%s) en X: %d Y: %d \n",nuevoPersonaje->identificador,nuevoPersonaje->nombre,coordenadaX,coordenadaY);

		break;


	case ('2'):

		mover(nuevoPersonaje,pokemonActual);

		break;

	case ('3'):

		//Si hay deadlock lo meto en la entrenadores_bloqueados
		envioQueSeAtrapoPokemon(nuevoPersonaje,pokemonActual);

		break;

	case ('\0'):
	case ('4'):
		recuperarPokemonDeEntrenador(nuevoPersonaje);
		nuevoPersonaje->estado='T';
		close(nuevoPersonaje->socket);
		pthread_exit(NULL);
		break;

	}

}

void accion_entrenador (t_registroPersonaje* nuevoPersonaje,t_registroPokenest* pokemonActual)
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

	case 'B':
		  log_info(logger,"El entrenador %s se encuentra bloqueado \n", nuevoPersonaje->nombre);
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

//void planificarEntrenador(int cantidadEntrenadores/*t_registroPersonaje* entrenador*/)
//{
//	int j;
//	for(j=0; j<cantidadEntrenadores; j++)
//	{
//		t_registroPersonaje* entrenador=list_get(entrenadores_listos,j);
//		if(!strcmp(infoMapa->algoritmo,"RR"))
//		{
//			sem_init(&(entrenador->turno),1,infoMapa->quantum);
//
//			//planificarRoundRobin();
//		}
//		else
//				{
//			planificarSRDF();
//				}
//		//accion_entrenador(entrenador, pokemonActual);
//
//	return;
//	}
//}

char esperarSerPlanificado(int threadId, t_registroPersonaje* entrenador)
  {

      if (threadId == entrenador->threadId)
        {
          accion_entrenador(entrenador, pokemonActual);
        }

      return entrenador->estado;

  }


void ejecutar_Entrenador(parametros_entrenador* param)
{
	 nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	 pokemonActual=malloc(sizeof(t_registroPokenest));
	 nuevoPersonaje->threadId = param->idHilo;
	 sem_init(&nuevoPersonaje->comienzoTurno,1,0);
	 sem_init(&nuevoPersonaje->finTurno,1,0);
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
		log_info(logger, "Nose para que te conectaste si no queres jugar xD ");		// No deberia tirar nunca este printf
	}
// 	------------------------------------------
// 	while(pthread_mutex_lock(&mutex_Ejecucion));
// 	------------------------------------------
// 	while(nuevoPersonaje->estado == 'E')
// 	{
//		while(nuevoPersonaje->quantumFaltante != 0 && nuevoPersonaje->estado!='T')
//		{
//			nuevoPersonaje->quantumFaltante--;
//			accion_entrenador(nuevoPersonaje,pokemonActual);
//		}
//		list_add(entrenadores_listos,nuevoPersonaje);	//Cuando termine mi quantum, lo vuelvo a agregar a la cola de listos
//		sem_post(&pasoDeEntrenador);					//Le doy paso al siguiente entrenador de la lista

// 		nuevoPersonaje->estado = esperarSerPlanificado(threadAEjecutar, nuevoPersonaje);
// 	   if (threadAEjecutar == nuevoPersonaje->threadId && (nuevoPersonaje->quantumFaltante < infoMapa->quantum))
// 	        {
// 		   	   nuevoPersonaje->quantumFaltante++;
// 		   	   accion_entrenador(nuevoPersonaje, pokemonActual);
// 	        }
// 	   if(nuevoPersonaje->quantumFaltante == infoMapa->quantum)
// 	   {
// 		  pthread_mutex_unlock(&mutex_bloqPlanificador);
// 	   }
// 	}
 		//sem_wait(&(nuevoPersonaje->ejecutar1));
 		//accion_entrenador(nuevoPersonaje, pokemonActual);
// 	------------------------------------------------------------
// 		recibirQueHacer(nuevoPersonaje,pokemonActual);
// 		pthread_mutex_unlock(&mutex_siguienteQuantum);
//-------------------------------------------------------------------
 		//sem_post(&(nuevoPersonaje->ejecutar2));
// 	}
// 	while(1)
// 	{
// 		sem_wait(&nuevoPersonaje->comienzoTurno);
//
//
// 			 		recibirQueHacer(nuevoPersonaje,pokemonActual);
// 			 		pthread_mutex_unlock(&mutex_Ejecucion);
//
//
//
// 	}
 	while(1)
 		{
 			sem_wait(&nuevoPersonaje->comienzoTurno);
 			recibirQueHacer(nuevoPersonaje, pokemonActual);
 			sem_post(&nuevoPersonaje->finTurno);
 		}
}

void ejecutarTrainer(t_registroPersonaje* entrenador)
{


}
void planificarNico()
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
			t_registroPersonaje* aux = malloc(sizeof(t_registroPersonaje));
			aux = (list_get(entrenadores_listos,j));
			if(aux->estado!= 'B')
			{
				for(s=0; s<infoMapa->quantum; s++)
				{
					nuevoPersonaje->quantumFaltante=0;
					pthread_mutex_lock(&mutex_threadAEjecutar);
					threadAEjecutar = aux->threadId;
					pthread_mutex_unlock(&mutex_threadAEjecutar);
				}

			}
    	}
     }
}

void planificarNuevo()
{
	int i,j;
	while(1)


	{
//		int cantidadEntrenadores = entrenadores_listos->elements_count;
//		for(i=0;i<cantidadEntrenadores;i++)
//		{
//
//			cantidadEntrenadores = entrenadores_listos->elements_count;
			sem_wait(&colaDeListos);
			t_registroPersonaje* entrenador = malloc(sizeof(t_registroPersonaje));
			pthread_mutex_lock(&mutex_EntrenadoresActivos);
			 entrenador = list_remove(entrenadores_listos,0);
			 pthread_mutex_unlock(&mutex_EntrenadoresActivos);
//			sem_init(&(entrenador->ejecutar1),1,0);
//			sem_init(&(entrenador->ejecutar2),1,0);

			if(!strcmp(infoMapa->algoritmo,"RR"))
			{

				for(j=0;j<infoMapa->quantum;j++)
				{
					if(entrenador->estado == 'M')
					{

															//Esto es por si consideramos al Main como un proceso mas
					entrenador->estado = 'E';
					sem_post(&(entrenador->comienzoTurno));
					//ejecutarTrainer(entrenador);
					sem_wait(&(entrenador->finTurno));
					entrenador->estado = 'M';
					j = 3;
					//pthread_mutex_unlock(&mutex_Ejecucion);

				}
					else
					{
						entrenador->estado = 'E';
						sem_post(&(entrenador->comienzoTurno));
						//ejecutarTrainer(entrenador);
						sem_wait(&(entrenador->finTurno));
						entrenador->estado = 'L';
					}
				}

			}
			pthread_mutex_lock(&mutex_EntrenadoresActivos);
			list_add(entrenadores_listos,entrenador);
			pthread_mutex_unlock(&mutex_EntrenadoresActivos);
			sem_post(&colaDeListos);

//			if(i == cantidadEntrenadores)
//						{
//							i=0;
//						}
//		}
	}
}

void planificarGabi()
{
	sem_wait(&colaDeListos);
//	int s,j;
//	int cantidadEntrenadores;
//	pthread_mutex_lock(&mutex_EntrenadoresActivos);
//	cantidadEntrenadores = entrenadores_listos->elements_count;
//	pthread_mutex_unlock(&mutex_EntrenadoresActivos);
//	while(cantidadEntrenadores!=0)
//	{
//		for(j=0; j<cantidadEntrenadores; j++)
//		{
//			for(s=0; s<infoMapa->quantum; s++)
//			{
//				planificarEntrenador(cantidadEntrenadores/*list_get(entrenadores_listos,j)*/);
//			}
//		}
//	}
	while(!list_is_empty(entrenadores_listos))
	{
		t_registroPersonaje* entrenador=list_get(entrenadores_listos,0);	//Que me devuelva el primero que va a ser el que se ejecute
		if(!strcmp(infoMapa->algoritmo,"RR"))		//Aca planifico RoundRobin
		{
		entrenador->quantumFaltante=infoMapa->quantum;
		list_remove(entrenadores_listos,0);			//Al ejecutarse lo saco de la cola de listos
		sem_wait(&pasoDeEntrenador);				//Este semaforo se queda esperando en bloqueado hasta que termine el quantum
		}											//Si el entrenador no termino de hacer lo que le faltaba , se agrega al final de la cola de listos
		else
		{
		planificarSRDF();
		}

	}
}

void releerconfig(int aSignal)
{
	 reLeerConfiguracionMapa();
	 signal(SIGUSR2,releerconfig);
	 return ;
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

	  if (leerConfiguracionMapa () == 1)
		  		  log_info(logger, "Archivo de configuracion leido correctamente");
			  else
				  log_error(logger,"Error la leer archivo de configuracion");
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
