/*
 * string.c
 *
 *  Created on: 9/10/2016
 *      Author: utnso
 */

#include "string.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

char **string_split(char *text, char *separator) {
	char **substrings = NULL;
	int size = 0;

	char *text_to_iterate = string_duplicate(text);
	char *token = NULL, *next = NULL;
	token = strtok_r(text_to_iterate, separator, &next);

	while (token != NULL) {
		size++;
		substrings = realloc(substrings, sizeof(char*) * size);
		substrings[size - 1] = string_duplicate(token);
		token = strtok_r(NULL, separator, &next);
	}

	size++;
	substrings = realloc(substrings, sizeof(char*) * size);
	substrings[size - 1] = NULL;

	free(text_to_iterate);
	return substrings;
}

