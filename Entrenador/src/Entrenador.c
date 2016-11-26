#include "Entrenador.h"

int conectado;
t_log* logger;
entrenador_datos* infoEntrenador;
pid_t pid;
t_list* listaDeMapas;
t_mapa* mapa;
char rutaArgv[100];
int contadorMapa;
int contadorObjetivo;
float tiempoBloqueo;
int atrapados;
int reinicio ;
int completado;
void devolverMedallas()
{
	log_info(logger, "Devolviendo medallas..");
	char ruta[300];
	char comando[300];

	strcpy(ruta,rutaArgv);
	strcat(ruta,"/Entrenadores/");
	strcat(ruta,infoEntrenador->nombre);
	strcat(ruta,"/medallas/");

	DIR *dp;
	struct dirent *ep;
	dp = opendir (ruta);
	if (dp != NULL)
	{
		ep = readdir (dp);
		while (ep)
		{
			if(ep->d_name[0]!='.')
			{
				strcpy(comando, "rm ");
				strcat(comando,ruta);
			    strcat(comando,ep->d_name);
			    system(comando);
			}
			ep = readdir (dp);
		}
		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory");
	log_info(logger,"Medallas devueltas!");
}

void muerteDefinitivaPorSenial(int aSignal)
{
	log_info(logger,"El personaje se desconecto");
	if(conectado == 1) informarFinalizacion();
	exit(1);
}

void gameOver()
{
	devolverMedallas();
	char respuesta = 's';
	log_info(logger,"GAME OVER!!! Parece que el personaje %s ha muerto y se ha quedado sin vidas,¿Desea continuar?(S).\n",infoEntrenador->nombre);
	scanf("%c", &respuesta);
	informarFinalizacion();

	switch(respuesta)
	{
		case 'S':
		case 's':
			reinicio = 1;
			conectado = 0;
			log_info(logger,"reiniciando...");
			contadorObjetivo = 99;
			contadorMapa = - 1;
			infoEntrenador->reintentos ++;
			infoEntrenador->vidas = 3;
			log_info(logger, "Gracias por continuar jugando! Se le han otorgado 3 vidas mas!\n");
			break;

	default:
		log_info(logger, "Gracias por jugar! Vuelva Pronto!\n");
		exit(1);
	}
}

void muertePorSenial(int num)
{
	infoEntrenador->vidas --;
	if(infoEntrenador->vidas>0)
	{
		log_info(logger,"El personaje %s perdio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	}
	else
	{
		conectado = 0;
		gameOver();
	}
}
void muertePorDeadlock(){

	infoEntrenador->vidas--;
	reinicio = 1;
	conectado = 0;
	contadorObjetivo = 99;
	if(infoEntrenador->vidas<=0)
	{
			gameOver();
	}
	else
	{
		contadorMapa = contadorMapa-1;
		atrapados = 0;
		log_info(logger,"El personaje %s perdio una vida por deadlock y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	}
}

void sumarVida(int aSignal)
{
	 infoEntrenador->vidas++;
	 log_info(logger,"El personaje %s recibio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	 return ;
}

char* objetivosDelMapa(char* mapaParaAgregar)
{
	char* objetoYparentesisIzquierdo= "obj[";
	char*parentesisDerecho="]";
	char * new= malloc(strlen(mapaParaAgregar)+strlen(objetoYparentesisIzquierdo)+strlen(parentesisDerecho)+1);
	new[0]='\0';
	strcat(new,objetoYparentesisIzquierdo);
	strcat(new,mapaParaAgregar);
	strcat(new,parentesisDerecho);
	int tamanio=strlen(new);
	new[tamanio]='\0';
	return new;
}

void imprimirClaveYValor(char* key, void* data)
{
	char* pokemon = (char *) data;
	printf("Variable: %s  Valor: %s \n", key, pokemon);
}

int leerConfiguracionEntrenador()
{
	char pathconfigMetadata[100];
	strcpy(pathconfigMetadata, rutaArgv);
	strcat(pathconfigMetadata, "/Entrenadores/");
	strcat(pathconfigMetadata,infoEntrenador->nombre);
	strcat(pathconfigMetadata,"/metadata");
	t_config* config = config_create(pathconfigMetadata);
	// Verifico que los parametros tengan sus valores OK
	if ( config_has_property(config, "nombre") && config_has_property(config, "simbolo")&& config_has_property(config, "vidas"))
	{
		strcpy(infoEntrenador->nombre,config_get_string_value(config,"nombre"));
		char* identificadorPokenest = config_get_string_value(config,"simbolo");

		infoEntrenador->simbolo=identificadorPokenest[0];
		infoEntrenador->vidas = config_get_int_value(config, "vidas");

		listaDeMapas = list_create();
		int k = 0;
				//Recorre la hoja de viaje, ciudad por ciudad
		while (&(*config_get_array_value(config, "hojaDeViaje")[k])!= NULL)
		{
			char* palabraAAgregar =config_get_array_value(config, "hojaDeViaje")[k];
			mapa = malloc(sizeof(t_mapa));
			mapa->nombreMapa=palabraAAgregar; //Nombre
			mapa->objetivos = list_create();
			if(config_has_property(config,objetivosDelMapa(palabraAAgregar)))
			{
				int i=0;
				//Recorre cada objetivo del mapa, pokemon a pokemon
				while(&(*config_get_array_value(config, objetivosDelMapa(palabraAAgregar))[i])!= NULL)
				{
					void *valor = malloc(sizeof(char));
					valor = config_get_array_value(config, objetivosDelMapa(palabraAAgregar))[i];
					list_add(mapa->objetivos, valor);
					i++;
				}
			}
			k++;
			list_add(listaDeMapas, mapa);
		}

		log_info(logger, "Nombre del entrenador: %s, Simbolo: %c Vidas: %d Reintentos: %d "
				,infoEntrenador->nombre,
				infoEntrenador->simbolo,
				infoEntrenador->vidas,
				infoEntrenador->reintentos);
		return 1;
	}
		else
	{
		return -1;
	}
}

int leerConfiguracionMapa(t_mapa* datos)
{
	char pathconfigMetadata[100];
	strcpy(pathconfigMetadata, rutaArgv);
	strcat(pathconfigMetadata, "/Mapas/");
	strcat(pathconfigMetadata, datos->nombreMapa);
	strcat(pathconfigMetadata,  "/metadata");
	puts(pathconfigMetadata);
	t_config* config = config_create(pathconfigMetadata);
	if ( config_has_property(config, "Puerto"))
	{
		datos->puertoMapa = config_get_int_value(config, "Puerto");
		datos->ipMapa = config_get_string_value(config, "IP");
		return 1;
	}
	else
		return -1;
}

void enviarMensajeInicial(int serverSocket)
{
	int CoordEnX = enviarCoordenada(infoEntrenador->posicionEnX,serverSocket);
	int CoordEnY = enviarCoordenada(infoEntrenador->posicionEnY,serverSocket);
	log_info(logger, "Estoy enviando la coordenada en X del ENTRENADOR que es %d Y: %d",CoordEnX,CoordEnY);

	send(serverSocket, infoEntrenador->nombre, sizeof(infoEntrenador->nombre), 0);
	log_info(logger, "Estoy enviando mi nombre %s\n",infoEntrenador->nombre);

	char* buffer = string_new();
	string_append(&buffer,string_itoa(BIENVENIDA));
	char* simbolo=charToString(infoEntrenador->simbolo);
	string_append(&buffer,simbolo);

	send(serverSocket,buffer,4,0);
	free(simbolo);

	log_info(logger, "Se ha enviado: %s \n",buffer);

}

void solicitarPosicion(t_mapa *mapa,char objetivo)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(SOLICITARPOSICION));
	char* objetivoString=charToString(objetivo);
	string_append(&buffer,objetivoString);

	send(mapa->socketMapa,buffer,4, 0);
	free(objetivoString);

	int x = recibirCoordenada(mapa->socketMapa);	//Recibo en X
	mapa->pokemonActualPosicionEnX = x;

	int y = recibirCoordenada(mapa->socketMapa);	//Recibo en Y
	mapa->pokemonActualPosicionEnY = y;

	log_info(logger, "Las coordenadas del pokemon que solicite X: %d Y: %d", mapa->pokemonActualPosicionEnX, mapa->pokemonActualPosicionEnY);
}

void informarFinalizacion(void)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(FINALIZACION));
	send(mapa->socketMapa, buffer,4,0);
	log_info(logger, "Le informo al Mapa que finalice mi mision aqui");
	recv(mapa->socketMapa,buffer,4,0);
	log_info(logger, "Recibo el mensaje de despedida %s",buffer);
	//close(mapa->socketMapa);
}

void solicitarAvanzar(t_mapa *mapa,char objetivo)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(SOLICITARAVANZAR));
	char* objetivoString=charToString(objetivo);
	string_append(&buffer,objetivoString);

	send(mapa->socketMapa, buffer,4, 0);
	free(objetivoString);

	int x = recibirCoordenada(mapa->socketMapa);	//Recibo la NUEVA coordenada Entrenador en X
	infoEntrenador->posicionEnX = x;

	int y = recibirCoordenada(mapa->socketMapa);	//Recibo la NUEVA coordenada Entrenador en Y
	infoEntrenador->posicionEnY = y;

	log_info(logger, "Mi nueva posicion es X: %d Y: %d \n",infoEntrenador->posicionEnX, infoEntrenador->posicionEnY);
}

int atraparPokemon(t_mapa *mapa,char objetivo)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(ATRAPARPOKEMON));
	char* objetivoString=charToString(objetivo);
	string_append(&buffer,objetivoString);

	send(mapa->socketMapa, buffer,4, 0);
	free(objetivoString);

	char* recibo = string_new();
	recv(mapa->socketMapa, recibo,4,0);

	if(!strcmp(recibo,"1"))
		return 1;
	else if(!strcmp(recibo,"2"))
		return 2;
	else
		return 0;
}

void copiarMedalla(char entrenador[20],char* nombre)
{
	log_info(logger, "Recogiendo medalla");
	char source[120];
	char dest[120];
	char str[300];

	strcpy(source,rutaArgv);
	strcat(source,"/Mapas/");
	strcat(source, nombre);
	strcat(source, "/medalla-");
	strcat(source, nombre);
	strcat(source,".jpg");
	strcpy(dest,rutaArgv);
	strcat(dest,"/Entrenadores/");
	strcat(dest,entrenador);
	strcat(dest,"/medallas/");

	strcpy(str,"cp -r ");
	strcat(str, source);
	strcat(str," ");
	strcat(str, dest);
	system(str);

	return;
}

void desconectar()
{
	if(conectado == 1 && reinicio == 1)
	{
		informarFinalizacion();
		conectado = 0;
	}
}

int jugar(){

	int nivelesCompletados = 0;

	devolverMedallas();

	int gane = 0;

	reinicio=0;

	log_info(logger,"------------------------*Tu Aventura pokemon esta por comenzar*----------------------");

	char *vector=malloc(sizeof(char)*10);

			for(contadorMapa = 0 ; contadorMapa< list_size(listaDeMapas) && reinicio != 1; contadorMapa++)

			{



				contadorObjetivo = 0;

				atrapados = 0;

				mapa = list_get(listaDeMapas,contadorMapa);

				leerConfiguracionMapa(mapa);				      					   //Busco en los archivos de config la ip y el socket

				int socketServidor = conectarConServer(mapa->ipMapa, mapa->puertoMapa); //Me conecto con el Mapa

				mapa->socketMapa = socketServidor;

				log_info(logger, "Conectado al servidor");							// Lo reflejo en el log

				infoEntrenador->posicionEnX = 1;									//Estaria en la posicion 1 en el nuevo mapa

				infoEntrenador->posicionEnY = 1;									// La (0,0) esta fuera de los limites

				enviarMensajeInicial(mapa->socketMapa);								//Le envio el simbolo al Mapa - HEADER ID es el 0

				log_info(logger,"Me conecto al socket: %d", mapa->socketMapa);

				//La utilizo para moverme entre objetivos de pokemones

				for(contadorObjetivo = 0 ; contadorObjetivo< list_size(mapa->objetivos) && reinicio != 1; contadorObjetivo++)

				{

					log_info(logger, "El Mapa Actual es %s ", mapa->nombreMapa);

					int i = list_size(mapa->objetivos);

					char objetivos[i];

					vector = list_get(mapa->objetivos,contadorObjetivo);

					objetivos[contadorObjetivo] = *vector;



					log_info(logger, "El objetivo actual es %c \n", objetivos[contadorObjetivo]);



					solicitarPosicion(mapa,objetivos[contadorObjetivo]);		//Le envio en el header el ID 1



					while((infoEntrenador->posicionEnX != mapa->pokemonActualPosicionEnX ||

					infoEntrenador->posicionEnY != mapa->pokemonActualPosicionEnY) && reinicio != 1)

					{

						solicitarAvanzar(mapa,objetivos[contadorObjetivo]);		//(Le envio en el header el ID 2)

					}

					if(infoEntrenador->posicionEnX == mapa->pokemonActualPosicionEnX

						&& infoEntrenador->posicionEnY == mapa->pokemonActualPosicionEnY && reinicio != 1)

					{

						int atrapado = 0;

						while(atrapado == 0 && reinicio != 1)

						{

							clock_t inicioBloqueo=clock();

							atrapado = atraparPokemon(mapa,objetivos[contadorObjetivo]);  								//Le envio en el header el ID 3

							log_info(logger,"Del pokemon que solicite hay %d instancias",atrapado);

							clock_t finBloqueo=clock();

							tiempoBloqueo = tiempoBloqueo + (finBloqueo - inicioBloqueo);

						}

						if (atrapado == 1 && reinicio != 1)

						{

							atrapados ++;

							log_info(logger,"Felicitaciones, capturaste a %c (pokemon nro %d de la hoja de viaje de este mapa) \n",objetivos[contadorObjetivo],contadorObjetivo);

							if(atrapados == list_size(mapa->objetivos))

							{

								copiarMedalla(infoEntrenador->nombre, mapa->nombreMapa);

								log_info(logger, "Felicitaciones!Capturaste todos los pokemons del mapa %s (nro %d) \n",mapa->nombreMapa,contadorMapa);

								nivelesCompletados++;

								informarFinalizacion();

							}

						}

						else if(atrapado == 2 && reinicio != 1)

						{//Esperando el pokemon para atrapar el mapa me indica que mori por deadlock

							informarFinalizacion();

							muertePorDeadlock();

							log_info(logger,"Parece que te mataron, Reiniciando entrenador....");

						}

					}

				}

				sleep(3); // Lo pongo a descansar al terminar un mapa!

			}

			if(nivelesCompletados == list_size(listaDeMapas)){

				gane = 1;

			}



	return gane;

}



void sig_handler(int signo)

{

    if (signo == SIGUSR1)

    	sumarVida(signo);

    else if (signo == SIGINT)

    	muerteDefinitivaPorSenial(signo);

    else if (signo == SIGTERM)

    	muertePorSenial(signo);

}





void cargarParametros(int argc, char **argv){



	switch(argc)

		{

		case (1):	//Si la cant de param es 1 osea solo ./Entrenador aplica por defecto los 2 argumentos de abajo

		//		log_info(logger, "Cantidad de parametros: %d, Aplicando datos por Defecto",argc);

		 		strcpy(infoEntrenador->nombre, "Red");

				strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");

		 		break;

		case (2):	//Si la cant de param es 2 osea ./Entrenador 'pokemon x'

		 //		log_info(logger, "Cantidad de parametros: %d, Aplicando datos por Defecto para la Ruta",argc);

		 		strcpy(infoEntrenador->nombre,argv[1]);

		 		strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");

		 		break;

		case (3):	//Si la cant de param es 3 osea ./Entrenador 'pokemon x' 'rutaDelPokedex'

		 //		log_info(logger, "Cantidad de parametros: %d",argc);

		 		strcpy(infoEntrenador->nombre,argv[1]);

		 		strcpy(rutaArgv,argv[2]);

				break;

		default:

		// 		log_info(logger, "ERROR: Ingresaste %d parametros",argc);

		 		printf("INGRESAR NOMBRE DEL ENTRENADOR ");

		 		scanf("%s",infoEntrenador->nombre);

		 		printf("INGRESAR RUTA DE LA POKEDEX ");

		 		scanf("%s",rutaArgv);

		 		break;

		 }

}






int main(int argc, char **argv)
{
	completado = 0;

		pid = getpid();

		tiempoBloqueo = 0;

		reinicio = 0;



		infoEntrenador = malloc(sizeof(entrenador_datos));

		infoEntrenador->reintentos=0;

		//----------------------CARGO LAS RUTAS Y NOMBRES

		cargarParametros(argc,argv);



		//-----------



		char* nombreLog = string_new();

		string_append(&nombreLog,infoEntrenador->nombre);

		string_append(&nombreLog,LOG_FILE);

		logger = log_create(nombreLog, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);

		log_info(logger, PROGRAM_DESCRIPTION);

		log_info(logger, "ID DEL PROCESO ENTRENADOR: %d NOMBRE: %s",pid,infoEntrenador->nombre);

		log_info(logger, "Ruta Pokedex %s",rutaArgv);

		log_info(logger, "Id del proceso: %d",pid);





		if ( leerConfiguracionEntrenador() == 1 )

			log_info(logger, "Archivo de configuracion leido correctamente");

		else

			log_error(logger,"Error al leer archivo de configuracion");





		//-------------------MANEJADOR DE SEÑALES----------------//

		if (signal(SIGUSR1, sig_handler) == SIG_ERR)

		        printf("\ncan't catch SIGUSR1\n");

		if (signal(SIGINT, sig_handler) == SIG_ERR)

		        printf("\ncan't catch SIGINT\n");

		if (signal(SIGTERM, sig_handler) == SIG_ERR)

		        printf("\ncan't catch SIGTERM\n");



		clock_t inicio=clock();

		while(completado == 0){

			completado = jugar();

		}

		clock_t fin=clock();

		log_info(logger, "------TE CONVERTISTE EN MAESTRO POKEMON------ \n");

		log_info(logger, "El tiempo total que tardo la aventura fue: %f segundos \n", (fin-inicio)*1000/(double)CLOCKS_PER_SEC);

		log_info(logger, "Estuviste esperando %f Segundos", tiempoBloqueo *1000 /(double)CLOCKS_PER_SEC);

		log_info(logger, "Solo te costo %d intentos", infoEntrenador->reintentos);

		list_destroy(listaDeMapas);

		list_destroy(mapa->objetivos);

		free(infoEntrenador);

		free(mapa);



		return EXIT_SUCCESS;

	}

