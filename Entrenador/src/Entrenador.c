#include "Entrenador.h"

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

int leerConfiguracionEntrenador(entrenador_datos *datos)
{
	char nombre[10];
	printf("%s", "Nombre del Entrenador?\n");
	scanf("%s",nombre);
	char pathconfigMetadata[100] ="/home/utnso/git/tp-2016-2c-SO-II-The-Payback/Entrenador/Entrenadores/";
	strcat(pathconfigMetadata,nombre);
	strcat(pathconfigMetadata,"/metadata");
	t_config* config = config_create(pathconfigMetadata);
		// Verifico que los parametros tengan sus valores OK
	if ( config_has_property(config, "nombre") && config_has_property(config, "simbolo")&& config_has_property(config, "vidas"))
		{
			datos->nombre= config_get_string_value(config,"nombre");
			datos->simbolo = config_get_string_value(config,"simbolo");
			datos->vidas = config_get_int_value(config, "vidas");

			datos->hojaDeViaje=dictionary_create();
			int k = 0;
			//Recorre la hoja de viaje, ciudad por ciudad
				while (&(*config_get_array_value(config, "hojaDeViaje")[k])!= NULL) {
					char *palabraAAgregar =config_get_array_value(config, "hojaDeViaje")[k];
					if(config_has_property(config,objetivosDelMapa(palabraAAgregar))){
						int i=0;
						//Recorre cada objetivo del mapa, pokemon a pokemon
						while(&(*config_get_array_value(config, objetivosDelMapa(palabraAAgregar))[i])!= NULL){
							char* valor = malloc(sizeof(char));
							valor = config_get_array_value(config, objetivosDelMapa(palabraAAgregar))[i];
							dictionary_put(datos->hojaDeViaje,
							&(*config_get_array_value(config, "hojaDeViaje")[k]),(void*) valor);
						i++;
						}
					}
				k++;
				}
			void imprimirClaveYValor(char* key, void* data) {
			char* pokemon = (char *) data;
			printf("Variable: %s  Valor: %s \n", key, pokemon);
			}

			printf("el nombre del entrenador es: %s\n su simbolo es: %s\n sus vidas son:%d\n"
						,datos->nombre,
						datos->simbolo,
						datos->vidas);
			dictionary_iterator(datos->hojaDeViaje, imprimirClaveYValor);
			return 1;
			}
			else
		    {
			return -1;
		    }
	}

	entrenador_datos* infoEntrenador;
int main(){

	/* Inicializacion y registro inicial de ejecucion */
		t_log* logger;
		logger = log_create(LOG_FILE, PROGRAM_NAME, IS_ACTIVE_CONSOLE, T_LOG_LEVEL);
		log_info(logger, PROGRAM_DESCRIPTION);

		infoEntrenador = malloc(sizeof(entrenador_datos));
		if ( leerConfiguracionEntrenador(infoEntrenador) == 1 )
			log_info(logger, "Archivo de configuracion leido correctamente");
		else
			log_error(logger,"Error la leer archivo de configuracion");

	/*
	 *  Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	 */
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion

	/*
	 * 	Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.
	 */
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	//int buf;
	//int i;
	//ssize_t nbytes;

	// Me conecto

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	/*
	 *	Enviar datos!
	 */
	int enviar = 1;
	char message[PACKAGESIZE];
	char* buffer = malloc(1);
	memcpy(buffer,infoEntrenador->simbolo, 1);
	printf("sending char: %c\n",buffer[0]);
	send(serverSocket, buffer[0], 1,0);

	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");
	log_info(logger, "Conectado al servidor");
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

	/*
	 *	Asique ahora solo me queda cerrar la conexion con un close();
	 */

	close(serverSocket);
	return 0;
}
