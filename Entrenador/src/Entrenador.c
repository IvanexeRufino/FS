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
		gameOver(NULL);
	}
}


void sumarVida(int aSignal)
{
	 infoEntrenador->vidas++;
	 log_info(logger,"El personaje %s recibio una vida por señal y actualmente tiene %d vidas.\n",infoEntrenador->nombre,infoEntrenador->vidas);
	 signal(SIGUSR1, sumarVida);
	 return ;
}

void gameOver (/*t_nivel* nivelActual*/)
{
	char respuesta;
	printf("GAME OVER!!! Parece que el personaje %s ha muerto y se ha quedado sin vidas.\n.",infoEntrenador->nombre);
	scanf("%c", &respuesta);
	return;
}


char* objetivosDelMapa(char* mapaParaAgregar) {
	char* objetoYparentesisIzquierdo= "obj[";
	char*parentesisDerecho="]";
	char * new= malloc(strlen(mapaParaAgregar)+strlen(objetoYparentesisIzquierdo)+strlen(parentesisDerecho)+1);
	new[0]='\0';
	strcat(new,objetoYparentesisIzquierdo);
	strcat(new,mapaParaAgregar);
	strcat(new,parentesisDerecho);
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
		datos->nombre= config_get_string_value(config,"nombre");
		datos->simbolo = config_get_string_value(config,"simbolo");
		datos->vidas = config_get_int_value(config, "vidas");
		listaDeNiveles = list_create();
		int k = 0;
				//Recorre la hoja de viaje, ciudad por ciudad
			while (&(*config_get_array_value(config, "hojaDeViaje")[k])!= NULL) {
				char* palabraAAgregar =config_get_array_value(config, "hojaDeViaje")[k];
				t_nivel* mapa = malloc(sizeof(int) + sizeof(char) + sizeof(bool) + sizeof (t_list));
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

			printf("el nombre del entrenador es: %s\n su simbolo es: %s\n sus vidas son:%d\n"
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
		datos->socketMapa = config_get_string_value(config, "Puerto");
		datos->ipMapa = config_get_string_value(config, "IP");
			return 1;
	}
	else		return -1;
}


void enviarMensajeInicial(int serverSocket){
	void* buffer = malloc(sizeof(int) + sizeof(char) );
	int identificador = '0';
	memcpy(buffer, &identificador , sizeof(char));
	memcpy(buffer + sizeof(char), (infoEntrenador->simbolo), sizeof(char));
	send(serverSocket, buffer, (2 * sizeof(char)), 0);
	puts(buffer);
	free(buffer);
	puts("conectado");
}

void jugarTurno(int socket){
	int i;
		i=0;
	void* buffer = malloc(sizeof(int) + sizeof(char));
	send(socket, buffer, (2 * sizeof(char)), 0);
	char message[10];
		while(i == 0){
		puts("empezo tu turno, Que vas a hacer?\n");
		puts("1-Solicitar posicion Pokenest\n");
		puts("2-Mover\n");
		puts("3-Atrapar Pokemon\n");
		fgets(message,1,0);
		memcpy(buffer ,message, sizeof(char));
		if(message[0] == '1' || message[0] == '2' || message[0] == '3' ){
			i++;
			printf("%d",i);
			send(socket,buffer,2,0);
		}
	}
		if(message[0] == '1' ) puts ("elegiste pedir posicion");
		if(message[0] == '2' ) puts ("elegiste moverte");
		if(message[0] == '3' ) puts ("elegiste atrapar Pokemon");
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


	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP


	int j;
	for(j = 0 ; j< list_size(listaDeNiveles); j++){

		t_nivel* mapa = list_get(listaDeNiveles,j);
		leerConfiguracionMapa(mapa);

		getaddrinfo(mapa->ipMapa, mapa->socketMapa , &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion
		int serverSocket;		//	Obtiene un socket
		serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	    connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen); // Me conecto
		freeaddrinfo(serverInfo);	// No lo necesitamos mas

		enviarMensajeInicial(serverSocket);
		sendObjetivosMapa(serverSocket);

		log_info(logger, "Conectado al servidor");
		puts("conectado");

		int enviar = 1;
		char message[PACKAGESIZE];
		while(enviar){
			fgets(message, PACKAGESIZE, stdin);			// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
			if (!strcmp(message,"exit\n"))
				{
					enviar = 0;
					log_info(logger, "El usuario decidio salir");// Chequeo que el usuario no quiera salir
				}
			if (enviar)
				{
					send(serverSocket, message, strlen(message) + 1, 0);
					log_info(logger, "Se envio el mensaje");  // Solo envio si el usuario no quiere salir.
				}
			}

	}
	return EXIT_SUCCESS;
}