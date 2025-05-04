#include "./includes/stopwords.h"
#include "utils.c"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Puntero al arreglo dinamico que tendra las palabras;
static char** stopwords_list = NULL;
// Numero actual de stopwords almacenadsa;
static size_t stopwords_count = 0;
// Tamanio inicial y factor de crecimiento del arreglo dinamico
static size_t stopwords_capacity = 0;

// Def del tamanio inicial y el crecimiento del arreglo
#define CAPACIDAD_INICIAL_STOPWORDS = 100
#define FACTOR_CRECIMIENTO_STOPWORDS = 2

bool cargar_stopwords(const char* filename) {
    FILE *archivo = fopen(filename, "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error no se pudo abrir el archivo de STOPWORDS %s\n");
        return false;
    }

    // liberar memoria si ya existe una lista de stopwords
    if (stopwords_list != NULL) {
        free_stopwords();
    }

    // COMIENZO ARREGLO DINAMICO
    stopwords_capacity = CAPACIDAD_INICIAL_STOPWORDS;
}