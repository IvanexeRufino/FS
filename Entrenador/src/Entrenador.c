#include "Entrenador.h"

#define PRESENTACION 1

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
		datos->puertoMapa = config_get_int_value(config, "Puerto");
		datos->ipMapa = config_get_string_value(config, "IP");
			return 1;
	}
	else		return -1;
}

void enviarMensajeInicial(int serverSocket){
	//void* buffer = malloc(sizeof(int) + sizeof(char) );
	//char identificador = '0';
	//memcpy(buffer, &identificador , sizeof(char));
	//memcpy(buffer + sizeof(char), (infoEntrenador->simbolo), sizeof(char));

	char* buffer = malloc(sizeof(int) + sizeof(char) );
	char* identificador = "0";
	strcpy(buffer,identificador);
	strcat(buffer,infoEntrenador->simbolo);
	send(serverSocket, buffer, strlen(buffer)+1, 0);
	puts("Se ha enviado: ");
	puts(buffer);
	free(buffer);

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

int conectarConServer(char *ipServer, int puertoServer)
{
	struct sockaddr_in socket_info;
	int nuevoSocket;
	// Se carga informacion del socket
	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = inet_addr(ipServer);
	socket_info.sin_port = htons(puertoServer);

	// Crear un socket:
	// AF_INET, SOCK_STREM, 0
	nuevoSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (nuevoSocket < 0)
		return -1;
	// Conectar el socket con la direccion 'socketInfo'.
	int conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
	int mostrarEsperaAconectar=0;
	while (conecto != 0){
		mostrarEsperaAconectar++;
		if (mostrarEsperaAconectar == 1){
			printf("Esperando que el mapa se levante\n");
		}
		conecto = connect (nuevoSocket,(struct sockaddr *)&socket_info,sizeof (socket_info));
	}

	return nuevoSocket;
}

int crearSocketCliente(char ip[], int puerto) {
	int socketCliente;
	struct sockaddr_in servaddr;

	if ((socketCliente = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Problem creando el Socket.");
		exit(2);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(puerto);

	if (connect(socketCliente, (struct sockaddr *) &servaddr, sizeof(servaddr))
			< 0) {
		perror("Problema al intentar la conexión con el Servidor");
		exit(3);
	}
	return socketCliente;
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

while(1)
	{
	int j;
		for(j = 0 ; j< list_size(listaDeNiveles); j++)
		{
			t_nivel* mapa = list_get(listaDeNiveles,j);
			leerConfiguracionMapa(mapa);				      					   //Busco en los archivos de config la ip y el socket
			int socketServidor = conectarConServer(mapa->ipMapa, mapa->puertoMapa); //Me conecto con el Mapa
			mapa->socketMapa = socketServidor;
			log_info(logger, "Conectado al servidor");							   // Lo reflejo en el log
			enviarMensajeInicial(mapa->puertoMapa);								   //Le envio el simbolo al Mapa - HEADER ID es el 0
			int k=0;															   //La utilizo para moverme entre objetivos de pokemones
			for(k = 0 ; k< list_size(mapa->objetivos); k++)
					{
					list_get(mapa->objetivos,k);
					char* buffer = malloc(1);
					recv(socketServidor, buffer, sizeof(char), 0);
					if(buffer == '0')											// Es la señal que me dice que es mi turno
						{
						//infoEntrenador->posicionEnX = solicitarPosicionEnX();		//Le envio en el header el ID 1 --Falta implementar
						//infoEntrenador->posicionEnY = solicitarPosicionEnY();		//Le envio en el header el ID 2 --Falta implementar
							while(infoEntrenador->posicionEnX != mapa->pokemonActualPosicionEnX && infoEntrenador->posicionEnY != mapa->pokemonActualPosicionEnY)
							{
								//en Desarrollo

								//solicitarAvanzar();									//Le envio en el header el ID 3
							}
						//int atrapado=atraparPokemon();  								//Le envio en el header el ID 4
						//if (atrapado == 1)
						//	{
						//	printf("Felicitaciones, capturaste el pokemon nro %d \n",k);
						//	}
						}
					}
			printf("Felicitaciones, terminaste de capturar todos los pokemons del mapa nro %d \n",j);
		}
	}
	return EXIT_SUCCESS;
}
