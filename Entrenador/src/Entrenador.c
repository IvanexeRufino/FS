#include "Entrenador.h"

t_log* logger;
entrenador_datos* infoEntrenador;
pid_t pid;
t_list* listaDeNiveles;

void muerteDefinitivaPorSenial(int aSignal)
{
	log_info(logger,"El personaje se desconecto");
	exit(1);
	signal(SIGINT, muerteDefinitivaPorSenial);
}

/*t_nivel* nivelActual*/
void gameOver()
{
	char respuesta= 'a';
	printf("GAME OVER!!! Parece que el personaje %s ha muerto y se ha quedado sin vidas,¿Desea continuar?(S/n).\n",infoEntrenador->nombre);
	scanf("%c", &respuesta);
	while (respuesta != 'S' || respuesta != 'n'){
		printf("Por favor responda si desea continuar solamente con (S/n).\n");
		scanf("%c", &respuesta);
	}
	if (respuesta == 'n')
	{
		printf("Gracias por jugar! Vuelva Pronto!\n");
		return;
	}
	else if (respuesta == 'S')
	{
		printf("Gracias por continuar jugando! Se le han otorgado 3 vidas mas!\n");
	return;
	}
}

void muertePorSenial(int num)
{
	signal(SIGTERM,muertePorSenial);//Por consola kill -15 PID
	infoEntrenador->vidas--;
	if(infoEntrenador->vidas>=0)
	{
		log_info(logger,"El personaje %s perdio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	}
	else
	{
		gameOver();
	}
}

void sumarVida(int aSignal)
{
	 infoEntrenador->vidas++;
	 log_info(logger,"El personaje %s recibio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	 signal(SIGUSR1, sumarVida);
	 return ;
}

char* objetivosDelMapa(char* mapaParaAgregar) {
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

void imprimirClaveYValor(char* key, void* data) {
		char* pokemon = (char *) data;
		printf("Variable: %s  Valor: %s \n", key, pokemon);
		}

int leerConfiguracionEntrenador(entrenador_datos *datos)
{
	char nombre[10];
	printf("%s", "Nombre del Entrenador?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[100] ="/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Entrenador/Entrenadores/";
	strcat(pathconfigMetadata,nombre);
	strcat(pathconfigMetadata,"/metadata");
	t_config* config = config_create(pathconfigMetadata);
	// Verifico que los parametros tengan sus valores OK
	if ( config_has_property(config, "nombre") && config_has_property(config, "simbolo")&& config_has_property(config, "vidas"))
		{
		strcpy(datos->nombre,config_get_string_value(config,"nombre"));
		char* identificadorPokenest = config_get_string_value(config,"simbolo");
		datos->simbolo=identificadorPokenest[0];
		datos->vidas = config_get_int_value(config, "vidas");
		listaDeNiveles = list_create();
		int k = 0;
				//Recorre la hoja de viaje, ciudad por ciudad
			while (&(*config_get_array_value(config, "hojaDeViaje")[k])!= NULL) {
				char* palabraAAgregar =config_get_array_value(config, "hojaDeViaje")[k];
				t_nivel* mapa = malloc(sizeof(t_nivel));
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
				list_add(listaDeNiveles, mapa);
			}

			printf("el nombre del entrenador es: %s\n su simbolo es: %c\n sus vidas son:%d\n"
					,datos->nombre,
					datos->simbolo,
					datos->vidas);
		return 1;
		}
		else
	    {
		return -1;
	    }
}

int leerConfiguracionMapa(t_nivel* datos)
{
	char pathconfigMetadata[90] = "/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/Mapas/";
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

void enviarMensajeInicial(int serverSocket){

	int CoordEnX = enviarCoordenada(infoEntrenador->posicionEnX,serverSocket);
	printf("Estoy enviando la coordenada en X del ENTRENADOR que es %d \n",CoordEnX);
	int CoordEnY = enviarCoordenada(infoEntrenador->posicionEnY,serverSocket);
	printf("Estoy enviando la coordenada en Y del ENTRENADOR que es %d \n",CoordEnY);

	char* buffer = malloc(sizeof(char)*3);
	buffer[0]='0';
	buffer[1]=infoEntrenador->simbolo;
	buffer[2]='\0';
	send(serverSocket,buffer,sizeof(buffer),0);
	printf("Se ha enviado: %s \n",buffer);
	//puts("Se ha enviado:");
	//puts(buffer);
	free(buffer);

}
void recibirCoordenadaPokemon(int *mapaCoordenadaPokemon, int socketMapa)
{
	char* buffer = malloc(sizeof(char)*4);
	recv(socketMapa, buffer,sizeof(buffer), 0);

	char* payload = malloc(sizeof(char)*4);
	strcpy(payload, buffer);
	str_cut(payload,0,1);

	(*mapaCoordenadaPokemon)=atoi(payload);
	free(buffer);
	free(payload);
}

void solicitarPosicion(t_nivel *mapa,char objetivo)
{
	char* buffer = malloc(sizeof(char)*3);
	char* identificador="1";
	strcpy(buffer,identificador);
	char*obj=charToString(objetivo);
	strcat(buffer,obj);
	send(mapa->socketMapa, buffer, sizeof(buffer), 0);

	recibirCoordenadaPokemon(&(mapa->pokemonActualPosicionEnX), mapa->socketMapa);    				//Recibo en X
	printf("La Coordenada del pokemon que solicite en X fue: %d \n",mapa->pokemonActualPosicionEnX);

	recibirCoordenadaPokemon(&(mapa->pokemonActualPosicionEnY), mapa->socketMapa); 					//Recibo en Y
	printf("La Coordenada del pokemon que solicite en Y fue: %d \n",mapa->pokemonActualPosicionEnY);

	free(buffer);
	free(obj);
}

void sendObjetivosMapa(int serverSocket)
{
	char *vector;
	int l= 0;
	t_nivel* mapa = malloc(sizeof(200));
	mapa = list_get(listaDeNiveles, 0);
	int i = list_size(mapa->objetivos);
	char objetivos[i];
	objetivos[i]='\0';
			while(l!=list_size(mapa->objetivos)){
				vector = list_get(mapa->objetivos,l);
				objetivos[l] = *vector;
				l++;
			}
			// ------------ Envio Objetivos
	send(serverSocket,&objetivos,6,0);
	free(mapa);
	puts("conectado");
}

void recibirCoordenadaEntrenador(int* coordenada, int socketMapa)
{
	char* buffer = malloc(sizeof(char)*4);
	recv(socketMapa, buffer,sizeof(buffer), 0);

	char* payload = malloc(sizeof(char)*4);
	strcpy(payload, buffer);
	str_cut(payload,0,1);

	(*coordenada)=atoi(payload);
	free(buffer);
	free(payload);
}
void solicitarAvanzar(t_nivel *mapa,char objetivo){
	char* buffer = malloc(sizeof(char)*3);
	char* identificador="2";
	strcpy(buffer,identificador);
	char*obj=charToString(objetivo);
	strcat(buffer,obj);
	send(mapa->socketMapa, buffer, sizeof(buffer), 0);
	puts("pido avanzar al mapa");
	recibirCoordenadaEntrenador(&(infoEntrenador->posicionEnX), mapa->socketMapa);    				//Recibo la NUEVA coordenada Entrenador en X
	recibirCoordenadaEntrenador(&(infoEntrenador->posicionEnY), mapa->socketMapa); 					//Recibo la NUEVA coordenada Entrenador en Y
	printf("Mi nueva posicion es X: %d Y: %d \n",infoEntrenador->posicionEnX, infoEntrenador->posicionEnY);
	free(buffer);
}

int atraparPokemon(t_nivel *mapa,char objetivo)
{
	char* buffer = malloc(sizeof(char)*3);

	char* identificador="3";
	strcpy(buffer,identificador);
	char*obj=charToString(objetivo);
	strcat(buffer,obj);

	send(mapa->socketMapa, buffer, sizeof(buffer), 0);
	char* recibo=malloc(sizeof(char)*2);
	recv(mapa->socketMapa, recibo, sizeof(recibo),0);

	if(!strcmp(buffer,"1"))
		{
		free(buffer);
		free(recibo);
		return 1;
		}
	else
	{
		free(buffer);
		free(recibo);
		return 0;
	}
}

int main(void) {

	pid = getpid();
	printf("El PID del proceso Personaje es %d\n", pid);
	signal(SIGINT, muerteDefinitivaPorSenial);//la de ctrl+c
	signal(SIGUSR1, sumarVida);//Por consola kill -10 PID
	signal(SIGTERM, muertePorSenial);//Por consola kill -15 PID
	logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
	log_info(logger, PROGRAM_DESCRIPTION);
	infoEntrenador = malloc(sizeof(entrenador_datos));
			if ( leerConfiguracionEntrenador(infoEntrenador) == 1 )
				log_info(logger, "Archivo de configuracion leido correctamente");
			else
				log_error(logger,"Error la leer archivo de configuracion");

			char *vector=malloc(sizeof(char)*10);

	while(1)
	{
	int j;
		for(j = 0 ; j< list_size(listaDeNiveles); j++)
		{
			t_nivel* mapa = list_get(listaDeNiveles,j);
			leerConfiguracionMapa(mapa);				      					   //Busco en los archivos de config la ip y el socket
			int socketServidor = conectarConServer(mapa->ipMapa, mapa->puertoMapa); //Me conecto con el Mapa
			mapa->socketMapa = socketServidor;
			log_info(logger, "Conectado al servidor");							// Lo reflejo en el log
			infoEntrenador->posicionEnX = 0;									//Estaria en la posicion 0 en el nuevo mapa
			infoEntrenador->posicionEnY = 0;
			enviarMensajeInicial(mapa->socketMapa);								//Le envio el simbolo al Mapa - HEADER ID es el 0
			int k=0;

			//La utilizo para moverme entre objetivos de pokemones
			for(k = 0 ; k< list_size(mapa->objetivos); k++)
					{

				int i = list_size(mapa->objetivos);
				char objetivos[i];
				vector = list_get(mapa->objetivos,k);
				objetivos[k] = *vector;

				printf("El objetivo actual es %c \n", objetivos[k]);

				solicitarPosicion(mapa,objetivos[k]);										   //Le envio en el header el ID 1

							while((infoEntrenador->posicionEnX != mapa->pokemonActualPosicionEnX ||
									infoEntrenador->posicionEnY != mapa->pokemonActualPosicionEnY))
							{
								solicitarAvanzar(mapa,objetivos[k]);									//(Le envio en el header el ID 2)
							}

						if(infoEntrenador->posicionEnX == mapa->pokemonActualPosicionEnX && infoEntrenador->posicionEnY == mapa->pokemonActualPosicionEnY)
							{
							int atrapado = atraparPokemon(mapa,objetivos[k]);  								//Le envio en el header el ID 3
							if (atrapado == 1)
								{
								printf("Felicitaciones, capturaste el pokemon nro %d \n",k);
								}
							}
					}
			printf("Felicitaciones, terminaste de capturar todos los pokemons del mapa nro %d \n",j);
		}
	}
	return EXIT_SUCCESS;
}
