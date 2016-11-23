#include "Entrenador.h"

int conectado;
t_log* logger;
entrenador_datos* infoEntrenador;
pid_t pid;
t_list* listaDeMapas;
t_nivel* mapa;
char rutaArgv[100];
int contadorMapa;
int contadorObjetivo;
float tiempoBloqueo;
int atrapados;
int reinicio = 0;


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
			if(ep->d_name[0]!='.'){
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
	if(conectado == 1)informarFinalizacion(mapa);
	exit(1);
	signal(SIGINT, muerteDefinitivaPorSenial);
}

void gameOver()
{
	devolverMedallas();
	char respuesta = 's';
	log_info(logger,"GAME OVER!!! Parece que el personaje %s ha muerto y se ha quedado sin vidas,¿Desea continuar?(S/n).\n",infoEntrenador->nombre);
	scanf("%c", &respuesta);
	informarFinalizacion(mapa);
	switch(respuesta){
		case 'S':
		case 's':
			//reinicio = 1;
			//conectado = 0;
			log_info(logger,"reiniciando...");
			//contadorObjetivo = 99;
			//contadorMapa = - 1;
			infoEntrenador->reintentos ++;
			infoEntrenador->vidas = 1;
			log_info(logger, "Gracias por continuar jugando! Se le han otorgado 1 vidas mas!\n");

			break;

	default:

		log_info(logger, "Gracias por jugar! Vuelva Pronto!\n");
		exit(1);
	}
}
void muertePorDeadlock(){

	infoEntrenador->vidas--;
	reinicio = 1;
	conectado = 0;
	contadorObjetivo = 99;
	if(infoEntrenador->vidas<=0)
	{
		contadorMapa = - 1;
		gameOver();

	}
	else
	{
		contadorMapa = contadorMapa-1;
		atrapados = 0;
		log_info(logger,"El personaje %s perdio una vida por deadlock y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	}
}

void muertePorSenial(int num)
{
	//signal(SIGTERM,muertePorSenial);//Por consola kill -15 PID
	//informarFinalizacion(mapa);
	muertePorDeadlock();
//	infoEntrenador->vidas--;
//	//contadorObjetivo = -1;		//siguiente pokenest
//	//atrapados = 0;
//	//contadorMapa = contadorMapa - 1;
//	if(infoEntrenador->vidas>0)
//	{
//		log_info(logger,"El personaje %s perdio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
//	}
//	else
//	{
//		contadorMapa = -1;
//		contadorObjetivo = 99;
//		reinicio = 1;
//		conectado = 0;
//		gameOver();
//	}
	informarFinalizacion(mapa);
}

void sumarVida(int aSignal)
{
	 infoEntrenador->vidas++;
	 log_info(logger,"El personaje %s recibio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	 signal(SIGUSR1, sumarVida);
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
			while (&(*config_get_array_value(config, "hojaDeViaje")[k])!= NULL) {
				char* palabraAAgregar =config_get_array_value(config, "hojaDeViaje")[k];
				//t_nivel* mapa = malloc(sizeof(t_nivel));
				mapa = malloc(sizeof(t_nivel));
				mapa->nivel=palabraAAgregar; //Nombre
				mapa->objetivos = list_create();
				if(config_has_property(config,objetivosDelMapa(palabraAAgregar))){
					int i=0;
					//Recorre cada objetivo del mapa, pokemon a pokemon
					while(&(*config_get_array_value(config, objetivosDelMapa(palabraAAgregar))[i])!= NULL){
						void *valor = malloc(sizeof(char));
						valor = config_get_array_value(config, objetivosDelMapa(palabraAAgregar))[i];
						list_add(mapa->objetivos, valor);
						i++;
					}
				}
				k++;
				list_add(listaDeMapas, mapa);
			}

			log_info(logger, "NOMBRE: %s SIMBOLO: %c VIDAS: %d"
					,infoEntrenador->nombre,
					infoEntrenador->simbolo,
					infoEntrenador->vidas);

		return 1;
		}
		else
	    {
		return -1;
	    }
}

int leerConfiguracionMapa(t_nivel* datos)
{
	char pathconfigMetadata[100];
	strcpy(pathconfigMetadata, rutaArgv);
	strcat(pathconfigMetadata, "/Mapas/");
	strcat(pathconfigMetadata, datos->nivel);
	strcat(pathconfigMetadata,  "/metadata");
	puts(pathconfigMetadata);
	t_config* config = config_create(pathconfigMetadata);
	if ( config_has_property(config, "Puerto"))
	{
		datos->puertoMapa = config_get_int_value(config, "Puerto");
		datos->ipMapa = config_get_string_value(config, "IP");
			return 1;
	}
	else		return -1;
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
	string_append(&buffer,charToString(infoEntrenador->simbolo));

	send(serverSocket,buffer,sizeof(buffer),0);
	log_info(logger, "Se ha enviado: %s \n",buffer);

}

void solicitarPosicion(t_nivel *mapa,char objetivo)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(SOLICITARPOSICION));
	string_append(&buffer,charToString(objetivo));

	send(mapa->socketMapa, buffer, sizeof(buffer), 0);

	int x = recibirCoordenada(mapa->socketMapa);	//Recibo en X
	mapa->pokemonActualPosicionEnX = x;

	int y = recibirCoordenada(mapa->socketMapa);	//Recibo en Y
	mapa->pokemonActualPosicionEnY = y;

	log_info(logger, "Las coordenadas del pokemon que solicite X: %d Y: %d", mapa->pokemonActualPosicionEnX, mapa->pokemonActualPosicionEnY);
}

void informarFinalizacion(t_nivel *mapa)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(FINALIZACION));
	send(mapa->socketMapa, buffer, sizeof(buffer), 0);
	log_info(logger, "Le informo al Mapa que finalice mi mision aqui");
	close(mapa->socketMapa);
}

void solicitarAvanzar(t_nivel *mapa,char objetivo)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(SOLICITARAVANZAR));
	string_append(&buffer,charToString(objetivo));

	send(mapa->socketMapa, buffer, sizeof(buffer), 0);
	//log_info(logger,"pido avanzar al mapa");

	int x = recibirCoordenada(mapa->socketMapa);	//Recibo la NUEVA coordenada Entrenador en X
	infoEntrenador->posicionEnX = x;

	int y = recibirCoordenada(mapa->socketMapa);	//Recibo la NUEVA coordenada Entrenador en Y
	infoEntrenador->posicionEnY = y;

	log_info(logger, "Mi nueva posicion es X: %d Y: %d \n",infoEntrenador->posicionEnX, infoEntrenador->posicionEnY);

}

int atraparPokemon(t_nivel *mapa,char objetivo)
{
	char* buffer = string_new();
	string_append(&buffer,string_itoa(ATRAPARPOKEMON));
	string_append(&buffer,charToString(objetivo));

	send(mapa->socketMapa, buffer, sizeof(buffer), 0);
	char* recibo = string_new();
	recv(mapa->socketMapa, recibo, sizeof(recibo),0);

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

void desconectar(){
	if(conectado == 1 && reinicio == 1){
		informarFinalizacion(mapa);
		conectado = 0;
	}
}

int main(int argc, char **argv)
{
	int lecturaArchivoConfig=0;
	tiempoBloqueo = 0;
	conectado = 0;
	reinicio = 0;
	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	pid = getpid();

	infoEntrenador = malloc(sizeof(entrenador_datos));	//Arranca el proceso Entrenador y armo su estructura
	infoEntrenador->reintentos=0;

	while(lecturaArchivoConfig == 0)
	{
		switch(argc)
		{
			case (1):	//Si la cant de param es 1 osea solo ./Entrenador aplica por defecto los 2 argumentos de abajo
				log_info(logger, "Cantidad de parametros: %d, Aplicando datos por Defecto",argc);
				strcpy(infoEntrenador->nombre, "Test");
				strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");
				break;
			case (2):	//Si la cant de param es 2 osea ./Entrenador 'pokemon x'
				log_info(logger, "Cantidad de parametros: %d, Aplicando datos por Defecto para la Ruta",argc);
				strcpy(infoEntrenador->nombre,argv[1]);
				strcpy(rutaArgv, "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Pokedex");
				break;
			case (3):	//Si la cant de param es 3 osea ./Entrenador 'pokemon x' 'rutaDelPokedex'
				log_info(logger, "Cantidad de parametros: %d",argc);
				strcpy(infoEntrenador->nombre,argv[1]);
				strcpy(rutaArgv,argv[2]);
				break;
			default:
				log_info(logger, "ERROR: Ingresaste %d parametros",argc);
				printf("INGRESAR NOMBRE DEL ENTRENADOR ");
				scanf("%s",infoEntrenador->nombre);
				printf("INGRESAR RUTA DE LA POKEDEX ");
				scanf("%s",rutaArgv);
				break;
		}
		log_info(logger, "ID DEL PROCESO ENTRENADOR: %d NOMBRE: %s",pid,infoEntrenador->nombre);
		log_info(logger, "Ruta Pokedex %s",rutaArgv);

		if ( leerConfiguracionEntrenador() == 1)
		{
			log_info(logger, "Archivo de configuracion leido correctamente");
			lecturaArchivoConfig = 1;
		}
		else
		{
			log_error(logger,"Error la leer archivo de configuracion");
			lecturaArchivoConfig = 0;
		}
	}
	signal(SIGINT, muerteDefinitivaPorSenial);//la de ctrl+c
	signal(SIGUSR1, sumarVida);//Por consola kill -10 PID
	signal(SIGTERM, muertePorSenial);//Por consola kill -15 PID

	char *vector=malloc(sizeof(char)*10); //Lo uso para recorrer lectura de pokemons dentro del mapa en el archivo de config
	clock_t inicio=clock();	//El entrenador inicia su aventura para convertirse en maestro pokemon
	devolverMedallas();

	for(contadorMapa = 0 ; contadorMapa< list_size(listaDeMapas); contadorMapa++)
	{
		reinicio=0;
		contadorObjetivo = 0;
		atrapados = 0;
		t_nivel* mapa = list_get(listaDeMapas,contadorMapa);	// Me da la lista de mapas, el mapa actual
		leerConfiguracionMapa(mapa);				      		//Busco en los archivos de config la ip y el socket
		int socketServidor = conectarConServer(mapa->ipMapa, mapa->puertoMapa); //Me conecto con el Mapa
		mapa->socketMapa = socketServidor;	//Se lo asigno al campo de mi estructura para un mejor manejo
		conectado = 1;
		log_info(logger, "Conectado al servidor");			// Lo reflejo en el log
		infoEntrenador->posicionEnX = 1;					//Estaria en la posicion 1 en el nuevo mapa
		infoEntrenador->posicionEnY = 1;					//La posicion (0,0) esta fuera del mapa
		enviarMensajeInicial(mapa->socketMapa);				//Le envio el simbolo al Mapa - HEADER ID es el 0
		desconectar();
			//La utilizo para moverme entre objetivos de pokemones
		for(contadorObjetivo = 0 ; contadorObjetivo< list_size(mapa->objetivos); contadorObjetivo++)
		{
			int i = list_size(mapa->objetivos);
			char objetivos[i];
			vector = list_get(mapa->objetivos,contadorObjetivo);
			objetivos[contadorObjetivo] = *vector;
			log_info(logger, "El objetivo actual es %c \n", objetivos[contadorObjetivo]);
			solicitarPosicion(mapa,objetivos[contadorObjetivo]);	//Le envio en el header el ID 1

			while((infoEntrenador->posicionEnX != mapa->pokemonActualPosicionEnX ||	//Mientras que la pos del entrenador sea distinta
			infoEntrenador->posicionEnY != mapa->pokemonActualPosicionEnY) && reinicio != 1)	//De la del objetivo dentro del mapa, seguira
			{
				solicitarAvanzar(mapa,objetivos[contadorObjetivo]);			//(Le envio en el header el ID 2)
			}
			if(infoEntrenador->posicionEnX == mapa->pokemonActualPosicionEnX && infoEntrenador->posicionEnY == mapa->pokemonActualPosicionEnY)
			{
				int atrapado = 0;
				while(atrapado == 0 && reinicio != 1)
				{
					clock_t inicioBloqueo=clock();
					atrapado = atraparPokemon(mapa,objetivos[contadorObjetivo]);  								//Le envio en el header el ID 3
					log_info(logger,"%d",atrapado);
					clock_t finBloqueo=clock();
					tiempoBloqueo = tiempoBloqueo + (finBloqueo - inicioBloqueo);
				}
				if (atrapado == 1)
				{
					atrapados ++;
					log_info(logger,"Felicitaciones, capturaste el pokemon nro %d \n",contadorObjetivo);
					if(atrapados == list_size(mapa->objetivos))
					{
						copiarMedalla(infoEntrenador->nombre, mapa->nivel);
						log_info(logger, "Felicitaciones, terminaste de capturar todos los pokemons del mapa nro %d que es %s\n",contadorMapa,mapa->nivel);
						conectado = 0;
						informarFinalizacion(mapa);
					}
				}
				else if(atrapado == 2 )
				{		//Esperando el pokemon para atrapar el mapa me indica que mori por deadlock
					informarFinalizacion(mapa);
					muertePorDeadlock();
					log_info(logger,"Parece que te mataron, Reiniciando nivel....");
				}
			}
		}//Aca termina de recorrer todos los objetivos del mapa actual

			sleep(3); // Lo pongo a descansar al terminar un mapa!

	}	//Aca termina de recorrer todos los mapas

		clock_t fin=clock();

		log_info(logger, "------TE CONVERTISTE EN MAESTRO POKEMON------ \n");
		log_info(logger, "El tiempo total que tardo la aventura fue: %f segundos \n", (fin-inicio)*10000/(double)CLOCKS_PER_SEC);
		log_info(logger, "Estuviste bloqueado %f Segundos", tiempoBloqueo *10000 /(double)CLOCKS_PER_SEC);
		log_info(logger, "Solo te costo %d intentos", infoEntrenador->reintentos);
		list_destroy(listaDeMapas);
		list_destroy(mapa->objetivos);
		free(vector);
		free(infoEntrenador);
		free(mapa);

	return EXIT_SUCCESS;
}
