#include "Mapa.h"

void sumarRecurso(t_list* items, char id) {
    ITEM_NIVEL* item = _search_item_by_id(items, id);

    if (item != NULL) {
        item->quantity++;
    } else {
        printf("WARN: Item %c no existente\n", id);
    }
}

void pokemonMasFuerteDe(t_registroPersonaje *personaje){
	//t_pokemon* masFuerte = malloc(sizeof(t_pokemon));
	//t_pokemon* actual = malloc(sizeof(t_pokemon));
	int nivel;
	char ruta[512];
	char directorio[512];

	strcpy(ruta,rutaArgv);
	strcat(ruta,"/Entrenadores/");
	strcat(ruta,personaje->nombre);
	strcpy(directorio,ruta);
	strcat(directorio,"/Dir de Bill/");
	strcat(ruta,"/Dir\\ de\\ Bill/");

	//log_info(logger, "La ruta en pokemonMasFuerteDe es: %s",ruta);
	int nivelAlto = 0;
	char* nombreFuerte = string_new();
	DIR *dp;
	struct dirent *ep;
	dp = opendir (directorio);
	if (dp != NULL)
	{
		ep = readdir (dp);
		while (ep)
		{
			if(ep->d_name[0]!='.' && ep->d_name[0]!='m')
			{
				nivel = leerDatosBill(ep->d_name , directorio);
				int i = strlen(ep->d_name) -7; //Si tengo Charmander002.dat, le saco el 002.dat que son 7 caracteres justos y me queda el nombre Charmander en limpio
				ep->d_name[i] ='\0';
				//if(infoMapa->batalla == 1)log_info(logger,"El pokemon %s de %s Tiene un nivel %d \n",ep->d_name ,personaje->nombre ,nivel);
				if(nivel > nivelAlto)
				{
					nivelAlto = nivel ;
					nombreFuerte = string_duplicate(ep->d_name);
				}
			}
			ep = readdir (dp);
		}
		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory");

	t_pokemon* pokemonNuevo =  create_pokemon(fabricaPokemon, nombreFuerte,nivelAlto);
	t_pokEn* pokEn = malloc(sizeof(pokEn));
	pokEn->entrenador=personaje;
	pokEn->pok = pokemonNuevo;
	list_add(listapokEn,pokEn);
	if(infoMapa->batalla == 1)
		log_info(logger,"El pokemon mas fuerte de %s es : %s nivel %d, de tipo %d y segundo tipo %d",
			personaje->nombre,nombreFuerte,nivelAlto,pokEn->pok->type,pokEn->pok->second_type);
}

int leerDatosBill(char* nombre , char* ruta)
{
	char aux[512];
	strcpy (aux,ruta);
	strcat(aux,nombre);
	t_config* config = config_create(aux);
	if (config_has_property(config, "Nivel"))
	{
		int nivel = config_get_int_value(config, "Nivel");
	//	printf("%d",nivel);
		return nivel;
	}
	else
		return 0;
}


void recuperarPokemonDeEntrenador(t_registroPersonaje *personaje)
{
	log_info(logger,"Recuperando Pokemons de %s", personaje->nombre);
	char origen[512];
	char directorio[512];
	char destino[512];
	char comando[512];
	char pokemon[30];

	strcpy(origen,rutaArgv);
	strcat(origen,"/Entrenadores/");
	strcat(origen,personaje->nombre);
	strcpy(directorio,origen);

	strcat(directorio,"/Dir de Bill/");
	strcat(origen,"/Dir\\ de\\ Bill/");
	log_info(logger, "La ruta en recuperarPokemonDeEntrenador de origen es: %s",origen);

	strcpy(destino, rutaArgv);
	strcat(destino,"/Mapas/");
	strcat(destino, infoMapa->nombre);
	strcat(destino, "/PokeNests/");
	log_info(logger, "La ruta en recuperarPokemonDeEntrenador de destino es: %s",destino);
//	puts(personaje->nombre);
	DIR *dp;
	struct dirent *ep;
	dp = opendir (directorio);

	if (dp != NULL)
	{
		ep = readdir (dp);

		while (ep)
		{
			if(ep->d_name[0]!='.' && ep->d_name[0]!='m' )
			{
				strcpy(pokemon,ep->d_name);
				int i = strlen(ep->d_name) -7;
				ep->d_name[i] = '\0';
	//			puts(ep->d_name);
				strcpy(comando,"cp -r ");
				strcat(comando,  origen);
				strcat(comando,  pokemon);
				strcat(comando,  " ");
				strcat(comando, destino);
				strcat(comando, ep->d_name);
				strcat(comando, "/");
				log_info(logger,"El comando aplicado de copiar es: %s",comando);
	//			puts(comando);
				system(comando);
				strcpy(comando, "rm -r ");
				strcat(comando, origen);
				strcat(comando, pokemon);
				log_info(logger,"El comando aplicado de remover es: %s",comando);
	//			puts(comando);
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
	char origen[512];
	char destino[512];
	char comando[512];
	strcpy(origen, rutaArgv);
	strcat(origen, "/Mapas/");
	strcat(origen, infoMapa->nombre);
	strcat(origen, "/PokeNests/");
	strcat(origen, pokenest->nombre);
//	puts(personaje->nombre);
	strcpy(destino,rutaArgv);
	strcat(destino,"/Entrenadores/");
	strcat(destino,personaje->nombre);
	strcat(destino,"/Dir\\ de\\ Bill/");

	log_info(logger, "La ruta en copiarPokemonAEntrenador es: %s",destino);

	DIR *dp;
	struct dirent *ep;
	dp = opendir (origen);
	if (dp != NULL)
	{
		ep = readdir (dp);
		while (ep)
		{
			if(ep->d_name[0]!='.' && ep->d_name[0]!='m' )
			{
//   		puts(ep->d_name);
			break;
			}
			ep = readdir (dp);
		}
	strcat(origen,"/");
	strcat(origen, ep->d_name);
	(void) closedir (dp);
	}
	else	perror ("Couldn't open the directory");

	strcpy(comando, "cp -r ");
	strcat(comando,origen);
	strcat(comando, " ");
	strcat(comando, destino);
//	puts(comando);
	system(comando);
	strcpy(comando, "rm ");
	strcat(comando, origen);
//	puts(comando);
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
		//Leo todas la pokenest

		DIR *dp;
		struct dirent *ep;
		dp = opendir (path);
		if (dp != NULL)
		{
			ep = readdir (dp);
			while (ep)
			{
				if(ep->d_name[0]!='.')
				{
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
	char pathconfigMetadata[512];
	strcpy(pathconfigMetadata, rutaArgv);
	strcat(pathconfigMetadata,"/Mapas/");
	char path[512];
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
	char pathpokenestMetadata[512] ;
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
		while (ep)
		{
			if(ep->d_name[0]!='.' && ep->d_name[0]!='m' )
			{
				cantidadPokemon ++;
//				puts (ep->d_name);
		    }
		    ep = readdir (dp);
		}
		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory");

	pokenest->cantidadDisp = cantidadPokemon;

	if (config_has_property(configNest, "Tipo") && config_has_property(configNest, "Identificador"))
	{
		strcpy(pokenest->tipo,(config_get_string_value(configNest, "Tipo")));
		char* identificadorPokenest= config_get_string_value(configNest, "Identificador");
		char* array= config_get_string_value(configNest, "Posicion");
		char** pos = string_split(array, ";");
		pokenest->x = atoi(pos[0]);
		pokenest->y = atoi(pos[1]);
		if (pokenest->x == 0){ pokenest->x = pokenest->x + 1;}
		if (pokenest->y == 0){ pokenest->y = pokenest->y + 1;}
		pokenest->identificador=identificadorPokenest[0];
		strcpy(pokenest->nombre , pokemon);

		log_info(logger, " El Tipo del Nest es: %s su posicion es X: %d Y es: %d "
							"su identificador: %c Y hay %d Pokemons de ese Tipo"
											,pokenest->tipo,pokenest->x, pokenest->y, pokenest->identificador, pokenest->cantidadDisp);

		if(pokenest->x> columnas || pokenest->y > filas)
		{
			log_error(logger, "La Pokenest %c supera los limites de filas/columnas del nivel. \n", pokenest->identificador);
			nivel_gui_terminar();
			exit(1);
		}

//		bool distanciaEntreCajas (t_registroPokenest* unaNest)
//		{
//			return (abs(unaNest->x - pokenest->x) + abs(unaNest->y - pokenest->y) <= 4);
//		}

//		bool distanciaEntreCajas (t_registroPokenest* unaNest)
//		{
//			bool a = (abs(unaNest->x - pokenest->x) <= 2);
//			bool b = (abs(unaNest->y - pokenest->y) <= 2);
//			return (a && b);
//
//		}
//
//		if(list_any_satisfy(listaPokenest, (void*) distanciaEntreCajas))
//		{
//			log_error(logger, "La Pokenest %c no respeta las distancias con otra Pokenest. \n", pokenest->identificador);
//			nivel_gui_terminar();
//			exit(1);
//		}
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

	void *buffer2 = malloc(40);
	recv(newfd,buffer2,40,0);
	memcpy(&nombre,buffer2,40);
	free(buffer2);
	strcpy(nuevoPersonaje->nombre,nombre);	// Lleno con el campo del nombre

	log_info(logger, "La coordenada INICIAL del Entrenador %s es X: %d Y: %d",nuevoPersonaje->nombre, nuevoPersonaje->x, nuevoPersonaje->y);

	char* bufferConAccionString;
	int bufferConAccionInt;
	void *buffer = malloc(sizeof(int) + sizeof(char));
	recv(newfd,buffer,sizeof(int) + sizeof(char),0);
	memcpy(&(bufferConAccionInt),buffer, sizeof(int));
	memcpy(&(nuevoPersonaje->identificador),buffer + sizeof(int),sizeof(char));
	free(buffer);
	bufferConAccionString=string_itoa(bufferConAccionInt);

	nuevoPersonaje->distanciaARecurso = -1;

	log_info(logger,"*BIENVENIDA* Recibi de %s el ID %c y accion: %c",nuevoPersonaje->nombre,nuevoPersonaje->identificador,bufferConAccionString[0]);
	CrearPersonaje(items, nuevoPersonaje->identificador , nuevoPersonaje->x, nuevoPersonaje->y);
	nivel_gui_dibujar(items,infoMapa->nombre);
	return (bufferConAccionString[0]);
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
	t_registroPersonaje* entrenador;
	t_registroPersonaje* aux;
	t_registroPersonaje* entrenadorMinimo;
	int i;
	int flag = 0;
	for(i=0; i < list_size(entrenadores_listos); i++)
	{
		aux = list_get(entrenadores_listos,i);
		if(aux->estado == 'L' && flag == 0)
		{
			entrenador = list_get(entrenadores_listos,i);
			flag = 1;
		}
	}

	int distanciaMinima = distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo);
	int indice = 0;
	for(i=0; i < list_size(entrenadores_listos); i++)
	{
		entrenador = list_get(entrenadores_listos,i);
		if(distanciaAProxObjetivo(entrenador,entrenador->proximoObjetivo)<=distanciaMinima && entrenador->estado == 'L')
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
	if(personaje->x == personaje->pokemonActual->x) personaje->ultimoRecurso = 0;
	if(personaje->y == personaje->pokemonActual->y) personaje->ultimoRecurso = 1;
	if(personaje->ultimoRecurso == 1) //ultimo movimiento fue en Y => me muevo en X
	{
		if(personaje->x <= personaje->pokemonActual->x)
			personaje->x +=1;
		else
			personaje->x -=1;
	personaje->ultimoRecurso = 0;
	}
	else
	{
		if(personaje->y <= personaje->pokemonActual->y)
			personaje->y +=1;
		else
			personaje->y -=1;
	 personaje->ultimoRecurso = 1 ;
	}

	MoverPersonaje(items, personaje->identificador, personaje->x, personaje->y );
	nivel_gui_dibujar(items,infoMapa->nombre);
	enviarCoordenada(personaje->x,personaje->socket);
	enviarCoordenada(personaje->y,personaje->socket);
	log_info(logger, "*MOVER* Se mueve %c (%s) por la coordenada X: %d Y: %d para llegar a: %c \n",personaje->identificador,personaje->nombre,personaje->x,personaje->y,pokemonActual->identificador);
}

void envioQueSeAtrapoPokemon (t_registroPersonaje *personaje, t_registroPokenest* pokemonActual)
{
	int finalizacion;
	log_info(logger, "El personaje %s solicitó el recurso %c", personaje->nombre, pokemonActual->identificador);
	int _has_symbol(t_registroPokenest *r)
	{
		return (r->identificador == personaje->proximoObjetivo);
	}
	t_registroPokenest *pokenest = list_find(listaPokenest, (void*) _has_symbol);
	int cant = 1;
	if(asignar_recurso(pokenest->nombre, personaje->nombre, cant) == true)
	{
		log_info(logger, "/*--------------------El Personaje: %c , atrapo al pokemon %c --------------------*/ \n",personaje->identificador, personaje->pokemonActual->identificador);
		copiarPokemonAEntrenador(personaje,personaje->pokemonActual);

		finalizacion=1;
		void *buffer = malloc(sizeof(int));
		memcpy(buffer,&finalizacion,sizeof(int));
		send(personaje->socket,buffer,sizeof(int),0);
		free(buffer);

		personaje->distanciaARecurso = -1;
		restarRecurso(items, pokenest->identificador);
	}
	else
	{
		personaje->estado = 'B';

		finalizacion=0;
		void *buffer = malloc(sizeof(int));
		memcpy(buffer,&finalizacion,sizeof(int));
		send(personaje->socket,buffer,sizeof(int),0);
		free(buffer);

		log_info(logger, "La Pokenest esta vacia.");
	}
}

void recibirQueHacer(t_registroPersonaje *nuevoPersonaje)
{
	int bufferConAccionInt;
	char bufferConPayload;

	void *buffer = malloc(sizeof(int) + sizeof(char));
	recv(nuevoPersonaje->socket,buffer,sizeof(int) + sizeof(char),0);
	memcpy(&(bufferConAccionInt),buffer, sizeof(int));
	memcpy(&bufferConPayload,buffer + sizeof(int),sizeof(char));
	free(buffer);

	nuevoPersonaje->accion=bufferConAccionInt;

	log_info(logger,"Recibio para hacer: Header: %d Payload: %c",bufferConAccionInt,bufferConPayload);


	int coordenadaX,coordenadaY;

	switch(bufferConAccionInt)
	{
	case (1):
		log_info(logger,"Buscando la informacion del pokemon ");
		cargoDatosPokemonActual(bufferConPayload,nuevoPersonaje->pokemonActual);
		nuevoPersonaje->proximoObjetivo = bufferConPayload;
		coordenadaX = enviarCoordenada((nuevoPersonaje->pokemonActual)->x,nuevoPersonaje->socket);				//Envio coordenada del pokemon en X
		log_info(logger," Socket numero :%d -----------------------------",nuevoPersonaje->socket);
		coordenadaY = enviarCoordenada((nuevoPersonaje->pokemonActual)->y,nuevoPersonaje->socket);				//Envio coordenada del pokemon en Y
		log_info(logger,"Posicion %d %d", coordenadaX, coordenadaY);
		log_info(logger,"*ENVIAR POSICION INICIAL* La coordenada de: %c (%s) en X: %d Y: %d \n",nuevoPersonaje->identificador,nuevoPersonaje->nombre,coordenadaX,coordenadaY);
		nuevoPersonaje->distanciaARecurso = 1;
		break;

	case (2):
		log_info(logger,"Moviendo...");
		mover(nuevoPersonaje,nuevoPersonaje->pokemonActual);
		break;

	case (3):
		log_info(logger,"Intentando Atrapar...");
		envioQueSeAtrapoPokemon(nuevoPersonaje,nuevoPersonaje->pokemonActual);

		break;

	default:
		log_info(logger,"Desconectando...");
		nuevoPersonaje->proximoObjetivo = '0';
		nuevoPersonaje->distanciaARecurso = -1;
		BorrarItem(items, nuevoPersonaje->identificador);
		liberar_recursos(nuevoPersonaje->nombre);
		recuperarPokemonDeEntrenador(nuevoPersonaje);
		nuevoPersonaje->estado='T';
		nivel_gui_dibujar(items,infoMapa->nombre);

		int finalizacion=999;
		void *buffer = malloc(sizeof(int));
		memcpy(buffer,&finalizacion,sizeof(int));
		send(nuevoPersonaje->socket,buffer,sizeof(int),0);
		free(buffer);

		shutdown(nuevoPersonaje->socket,2);
		close(nuevoPersonaje->socket);
		break;
	}
}

void ejecutar_Entrenador(parametros_entrenador* param)
{
	 t_registroPersonaje* nuevoPersonaje;
	 nuevoPersonaje = malloc(sizeof(t_registroPersonaje));
	 nuevoPersonaje->pokemonActual=malloc(sizeof(t_registroPokenest));
	 nuevoPersonaje->threadId = param->idHilo;

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
 	pthread_exit(0);
 	return;

}

void planificarNuevo()
{
	int i=0;
	int j;

	while(1)
	{
		//ACA VOY A LOGGEAR LA COLA DE LISTOS--------------------------------------------------
		char *str = string_new();
		int cola = 0;
		void contarListosEnCola(t_registroPersonaje *p)
		{
			if (p->estado == 'L')
			{
				cola++;
				string_append(&str, p->nombre);
				string_append(&str, "|");
			}
		}
		list_iterate(entrenadores_listos, (void*) contarListosEnCola);
		log_info(logger,"Hay %d entrenadores listos, %s", cola, str);
		cola = 0;
		char* str2 = string_new();
		void contarBloqueadosEnCola(t_registroPersonaje *p)
		{
			if (p->estado == 'B')
			{
				cola++;
				string_append(&str2, p->nombre);
				string_append(&str2, "|");
			}
		}
		list_iterate(entrenadores_listos, (void*) contarBloqueadosEnCola);
		log_info(logger,"Hay %d entrenadores bloqueados, %s", cola, str2);
		free(str);
		free(str2);
		//DEJO DE LOGEAR LAS COLAS--------------------------------------------------------------
		sem_wait(&colaDeListos);
		t_registroPersonaje* entrenador = malloc(sizeof(t_registroPersonaje));
		j=0;
		if(!strcmp(infoMapa->algoritmo,"RR"))
		{
			pthread_mutex_lock(&mutex_EntrenadoresActivos);
			int e;
			for(e=0;list_size(entrenadores_listos)>e;e++)
			{
			entrenador = list_remove(entrenadores_listos,0);
			if(entrenador->estado == 'B') list_add(entrenadores_listos,entrenador);
			else break;
			}
			pthread_mutex_unlock(&mutex_EntrenadoresActivos);
			for(i=0;i!=infoMapa->quantum && j == 0 && entrenador->estado == 'L' ;i++)
			{
				recibirQueHacer(entrenador);
				usleep(infoMapa->retardo*1000);
				if(entrenador->estado == 'T') j = 1;
			}
		}
			else  //NOT RR
		{
			int k=0;
			while(k<list_size(entrenadores_listos))
			{
				pthread_mutex_lock(&mutex_EntrenadoresActivos);
				entrenador =  list_get(entrenadores_listos,k);
				if(entrenador->distanciaARecurso == -1)
				{
					entrenador =  list_remove(entrenadores_listos,k);
					log_info(logger,"El entrenador %s no conoce su ubicacion, por eso lo atiendo primero",entrenador->nombre);
					recibirQueHacer(entrenador);
					usleep(infoMapa->retardo*1000);
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
					usleep(infoMapa->retardo*1000);
				}
			}
		}
		if(entrenador->estado!='T')
		{
			pthread_mutex_lock(&mutex_EntrenadoresActivos);
			list_add(entrenadores_listos,entrenador);			//Si no termino de hacer todo lo que necesita, se vuelve a agregar a la cola
			pthread_mutex_unlock(&mutex_EntrenadoresActivos);
			if(entrenador->estado == 'L') sem_post(&colaDeListos);  //Agrego este if para que el mapa no se quede loopeando si estan bloqueados los entrenadores
		}
		else
			free(entrenador);
	}	//Aca termina y  vuelve al while(1)
}

void releerconfig(int aSignal)
{
	 reLeerConfiguracionMapa();
	 return ;
}

char *liberar_recursos(char *nombre_personaje){
	char *recursosString = string_new();
	int liberados = 0, totalLiberados = 0;

	 //lockeo el mutex
	 pthread_mutex_lock(&mutex);

	/********** Critical Section *******************/

	void _list_recursos(t_registroPokenest *r)
	{
		int i;
		liberados = (int)dictionary_get(dictionary_get((t_dictionary*)alloc, r->nombre), nombre_personaje);
		liberar_recurso(r->nombre, nombre_personaje, liberados);
		if (liberados > 0)
		{
			string_append(&recursosString, string_from_format("%c|%d|", r->identificador, liberados));
			totalLiberados++;

			for (i = 0; i < liberados; i++)
			{
				sumarRecurso(items, r->identificador);
			}
		}
	}
	list_iterate(listaPokenest, (void*) _list_recursos);

	/********** end Critical Section *******************/
    pthread_mutex_unlock(&mutex);
   	char *str = string_new();

   	if (strlen(recursosString) > 0)
   	{
		//Informo los recursos liberados
    	recursosString[strlen(recursosString)-1] = '\0';
    	str = string_from_format("%d|%s", totalLiberados, recursosString);
		log_debug(logger, "Recursos liberados: %s", str);
    } else
    {
    	str = string_from_format("%d", 0);
    	log_debug(logger, "No se liberaron recursos");
    }
	return recursosString;
}

void liberar_recurso(char *recurso, char *personaje, int cant)
{
	int liberados = 0, availables = 0;
//	char rec;
//	rec = recurso[0];
//	sumarRecurso(items, rec);
	liberados = (int)dictionary_get(dictionary_get((t_dictionary*)alloc, recurso), personaje);

	//saco los recursos alocados.
	dictionary_remove(dictionary_get((t_dictionary*)alloc, recurso), personaje);

	//Incremento los recursos disponibles.
	availables = (int)dictionary_get(available, recurso);

	dictionary_remove(available, recurso);
	dictionary_put(available,recurso,(void*)(availables + liberados));

	//saco los recursos de request
	dictionary_remove(dictionary_get(request, recurso), personaje);

}

int asignar_recurso(char *pokenest, char *personaje, int cant)
{
	int asignados = 0, availables = 0, requested = 0;
	int pudo_asignar = 0;

   //lockeo el mutex
   pthread_mutex_lock(&mutex);

    /********** Critical Section *******************/

	if ((int)dictionary_get(available, pokenest) >= cant)
	{
		//agrego los pokenests a alloc
		asignados = (int)dictionary_get(dictionary_get(alloc, pokenest), personaje);
		dictionary_remove(dictionary_get(alloc, pokenest), personaje);
		dictionary_put(dictionary_get(alloc, pokenest), personaje,(void*)(asignados + cant));

		//saco los pokenests de available
		availables = (int)dictionary_get(available, pokenest);
		dictionary_remove(available, pokenest);
		dictionary_put(available, pokenest,(void*)(availables - cant));

		//si tenia a ese pokenest en request, lo saco
		requested = (int)dictionary_get(dictionary_get(request, pokenest), personaje);
		if (requested > 0)
		{
			dictionary_remove(dictionary_get(request, pokenest), personaje);
			dictionary_put(dictionary_get(request, pokenest), personaje,(void*)(requested - cant));
		}

		pudo_asignar = 1;
	}
	else
	{
		//actualizo matriz request
		requested = (int)dictionary_get(dictionary_get(request, pokenest), personaje);
		dictionary_remove(dictionary_get(request, pokenest), personaje);
		dictionary_put(dictionary_get(request, pokenest), personaje,(void*)(requested + cant));
	}

	/********** end Critical Section *******************/
     pthread_mutex_unlock(&mutex);

	return pudo_asignar;
}

void batallaPokemon()
{
	t_registroPersonaje* actual = malloc(sizeof(t_registroPersonaje));
	int i;
	for(i=0; list_size(entrenadores_listos)!=i;i++)
	{
		actual = list_get(entrenadores_listos,i);
		if(actual->marcado == false)
		{
			pokemonMasFuerteDe(actual);
		}
	}

	t_pokEn* pokEn = malloc(sizeof(pokEn));
	t_pokEn* aux = malloc(sizeof(pokEn));
	t_pokEn* pokEn2 = malloc(sizeof(pokEn));
	t_pokemon* pokPerdedor = malloc(sizeof(t_pokemon));
	pokEn = list_remove(listapokEn,0);
	pthread_mutex_lock(&mutex_EntrenadoresActivos);
	while(list_size(listapokEn)!=0)
	{
		pokEn2 = list_remove(listapokEn,0);
		pokPerdedor = pkmn_battle(pokEn->pok , pokEn2->pok);
		if(pokPerdedor == pokEn2->pok)
		{
			//Si gana el pokEn1
			if(infoMapa->batalla == 1){
				log_info(logger,"Gano el entrenador %s con su pokemon %s ", pokEn->entrenador->nombre, pokEn->pok->species);
			}
		}
			else
		{
		//Si gana el pokEn2
				if(infoMapa->batalla == 1)
						log_info(logger,"Gano el entrenador %s con su pokemon %s ", pokEn2->entrenador->nombre, pokEn2->pok->species);

				aux = pokEn;
				pokEn = pokEn2;
				pokEn2 = aux;
		}
			int i = 0;
			void eliminar(t_registroPersonaje* entrenador)
			{
				if(entrenador->identificador == pokEn2->entrenador->identificador)
				{
					if(infoMapa->batalla == 1)
						log_info(logger,"Informo a %s , que murio en una batalla.",entrenador->nombre);

					int muerteDeadlock=2;
					void *buffer = malloc(sizeof(int));
					memcpy(buffer,&muerteDeadlock,sizeof(int));		//El 2 que mando es el de muerte por deadlock
					send(entrenador->socket,buffer,sizeof(int),0);
					free(buffer);

					recibirQueHacer(entrenador);
				}
				i++;
			}
			list_iterate(entrenadores_listos, (void*) eliminar);
	}
	pthread_mutex_unlock(&mutex_EntrenadoresActivos);
	if(infoMapa->batalla == 1)log_info(logger,"El ganador de todas las batallas es:%s", pokEn->entrenador->nombre);
}

// Funcion que detecta si existen personajes interbloqueados
void *detectar_interbloqueo(void *milis)
{
	t_dictionary *copiaAvailable;
	/*
	 * Loop principal del hilo. Ejecuta el algoritmo para detectar deadlock entre personajes
	 * Envia un mensaje al proceso en caso de detectarlo.
	 */
	while (1)
	{
		log_info(logger,"Detectando interbloqueo");
		//lockeo el mutex
		pthread_mutex_lock(&mutex);

		   /********** Critical Section *******************/

		// Inicializo todos los personajes como no marcados y
		// marco todos los personajes que no tengan alocado ningun recurso
		void marcarEntrenadores(t_registroPersonaje *p)
		{
			p->marcado = false;
			int cant_recursos = 0;
			void cuentoRecursosDisp(t_registroPokenest *r)
			{
				cant_recursos += (int)dictionary_get(dictionary_get(alloc, r->nombre), p->nombre);
			}
			list_iterate(listaPokenest, (void*) cuentoRecursosDisp);

			if (cant_recursos == 0)
			{
				p->marcado = true;
			}
		}
		list_iterate(entrenadores_listos, (void*) marcarEntrenadores);

		//Inicializo el vector copiaAvailable, copia de available.
		copiaAvailable = dictionary_create();
		void copioDiccionario(t_registroPokenest *r)
		{
			dictionary_put(copiaAvailable, r->nombre, (void*)dictionary_get(available, r->nombre));
		}
		list_iterate(listaPokenest, (void*) copioDiccionario);
		bool disponible;
		void filtroPersonaje2(t_registroPersonaje *p)
		{
			disponible = true;

			if (p->marcado == false)
			{
				void recursosDisp(t_registroPokenest *r)
				{
					if ((int)dictionary_get(dictionary_get(request, r->nombre), p->nombre) > (int)dictionary_get(copiaAvailable, r->nombre))
					{
						disponible = false;
					}
				}
				list_iterate(listaPokenest, (void*) recursosDisp);

				if (disponible == true)
				{
					p->marcado = true;
					// copiaAvailable + allocated para ese personaje
					int nueva_cantidad = 0;
					void recursosDisp2(t_registroPokenest *r)
					{
						nueva_cantidad = (int)dictionary_get(copiaAvailable, r->nombre) + (int)dictionary_get(dictionary_get((t_dictionary*)alloc, r->nombre), p->nombre);
						dictionary_remove(copiaAvailable, r->nombre);
						dictionary_put(copiaAvailable, r->nombre, (void*)nueva_cantidad);
					}
					list_iterate(listaPokenest, (void*) recursosDisp2);

				}
			}
		}
		list_iterate(entrenadores_listos, (void*) filtroPersonaje2);

		// chequeo si existen personajes sin marcar == interbloqueo y envio señal al proceso
		int bloqueados = 0;
		char *str = string_new();

		void contarBloqueados(t_registroPersonaje *p)
		{
			if (p->marcado == false)
			{
				bloqueados++;
				string_append(&str, p->nombre);
				string_append(&str, "|");
			}
		}
		list_iterate(entrenadores_listos, (void*) contarBloqueados);

			/********** end Critical Section *******************/
		pthread_mutex_unlock(&mutex);

		//pthread_mutex_lock(&mutex_deadlock);

		str[strlen(str)] = '\0';
		if (bloqueados > 1)
		{
			//Batalla pokemon---------------------------------------------------------//
			if(infoMapa->batalla == 1)log_info(logger,"--------------------Se ha detectado un interbloqueo! %s", str);
			batallaPokemon();
		}

		else
			log_info(logger,"no hay interbloqueo!");

		log_info(logger,"Desbloqueando entrenadores bloqueados");
		//sleep(1);
		void desbloquear(t_registroPersonaje *p)
		{
			if(p->estado == 'B')
			{
				p->estado = 'L';
				sem_post(&colaDeListos);
			}
		}
		list_iterate(entrenadores_listos, (void*) desbloquear);

		//pthread_mutex_unlock(&mutex_deadlock);

		dictionary_destroy(copiaAvailable);
		usleep(((int)milis*1000));
	}
}

int main(int argc, char **argv)
{
	filas = 30;
	columnas =30;
	items = list_create();	//Para usar despues en las cajas

	infoMapa = malloc(sizeof(mapa_datos));

	switch(argc)
	{
	case (1):
		strcpy(infoMapa->nombre,"Home");
		strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex/01-base");
		break;
	case (2):
		strcpy(infoMapa->nombre,argv[1]);
		strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex/01-base");
		break;
	case (3):
		strcpy(infoMapa->nombre,argv[1]);
		strcpy(rutaArgv, argv[2]);
		break;
	default:
	 	printf("INGRESAR NOMBRE DEL MAPA ");
	 	scanf("%s",infoMapa->nombre);
	 	printf("INGRESAR RUTA DE LA POKEDEX ");
	 	scanf("%s",rutaArgv);
	 	break;
	}

	/* Inicializacion y registro inicial de ejecucion */
	char* nombreLog = string_new();
	string_append(&nombreLog,infoMapa->nombre);
	string_append(&nombreLog,LOG_FILE);
	logger = log_create(nombreLog, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	log_info(logger, PROGRAM_DESCRIPTION);

	log_info(logger, "Nombre Mapa: %s Ruta: %s", infoMapa->nombre, rutaArgv);
	listapokEn = malloc(sizeof(t_pokEn));
	listapokEn = list_create();
	listaPokenest = malloc(sizeof(t_registroPokenest));
	listaPokenest = list_create();
	fabricaPokemon = create_pkmn_factory();
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

    nivel_gui_inicializar();
    nivel_gui_get_area_nivel(&filas, &columnas);

	if (leerConfiguracionMapa () == 1)
		log_info(logger, "Archivo de configuracion leido correctamente");
	else
		log_error(logger,"Error la leer archivo de configuracion");

	//pokenests
	void _list_elements(t_registroPokenest *r)
	{
		dictionary_put(available, r->nombre, (void*)r->cantidadDisp);
	  	dictionary_put(request, r->nombre, dictionary_create());
	  	dictionary_put(alloc, r->nombre, dictionary_create());
	}
	list_iterate(listaPokenest, (void*) _list_elements);

	nivel_gui_dibujar(items,infoMapa->nombre);

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
		newfd = AceptarConexionCliente(socketServidor);

		//No quiero quedarme esperando a que termine de hacer lo del hilo, por eso no pongo el join
		//Quiero seguir escuchando conexiones entrantes,por eso el hilo finaliza por si solo cuando termina

		pthread_t idHilo;
		parametros_entrenador* param = malloc(sizeof(parametros_entrenador));

		param->idHilo = idHilo;
		param->newfd = newfd;
		pthread_create (&idHilo,NULL,(void*)ejecutar_Entrenador,param);
	}

	return 0;
}
