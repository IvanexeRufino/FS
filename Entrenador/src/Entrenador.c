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
int main(void) {
	t_log* logger;
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
	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion
	int serverSocket;		//	Obtiene un socket
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen); // Me conecto
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	puts (infoEntrenador->simbolo);
	void* buffer = malloc(sizeof(int) + sizeof(char));
	int identificador = '0';
	memcpy(buffer, &identificador , sizeof(char));
	memcpy(buffer + sizeof(char), (infoEntrenador->simbolo), sizeof(char));
	send(serverSocket, buffer, (2 * sizeof(char)), 0);
	puts(buffer);

	log_info(logger, "Conectado al servidor");
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
	return EXIT_SUCCESS;
}
