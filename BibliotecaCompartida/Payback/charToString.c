#include "Configuracion.h"

char* charToString(char element) {
	char* new = malloc(2);
	*new = element;
	*(new + 1) = '\0';
	return new;
}
