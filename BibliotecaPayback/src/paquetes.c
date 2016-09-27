/*
 * paquetes.c
 *
 *  Created on: 26/9/2016
 *      Author: utnso
 */
#include <paquetes.h>

void* memoria(int cantidad)
{
	void* puntero = NULL;
	if (cantidad !=0)
	{
		while (puntero == NULL)
		{
			puntero= malloc(cantidad);
		}
	}
	return puntero;
}

char* acoplador(t_paquete* paquete) /*transforma una estructura de tipo t_paquete en un stream*/
{
	char* paqueteSalida = memoria(size_header + paquete->tamanioDatos);
	memcpy(paqueteSalida, paquete, size_header);
	memcpy(paqueteSalida + size_header, paquete->datos, paquete->tamanioDatos);
	return paqueteSalida;
}

void desacoplador(char* buffer,int sizeBuffer, t_list* paquetes)/*transforma multiples streams en estructuras de t_paquete y los agrega a una lista*/
{
	int desacoplado = 0;
	t_paquete* paquete;
	while (sizeBuffer!= desacoplado)
	{
		paquete= memoria(sizeof(t_paquete));
		paquete->codOp= (uint16_t)*(buffer + desacoplado);
		paquete->tamanioDatos= (uint16_t)* (buffer + sizeof(uint16_t) + desacoplado);
		paquete->datos= memoria(paquete->tamanioDatos);
		memcpy(paquete->datos, buffer + desacoplado + size_header, paquete->tamanioDatos);
		if(paquete->codOp==0)
		{
			list_clean(paquetes);
			list_add(paquetes,paquete);
			return;
		}
		else
		{
			list_add(paquetes, paquete);
		}

		desacoplado += (size_header + paquete->tamanioDatos);
	}
	return;
}

int main(void)
{
	return EXIT_SUCCESS;
}
