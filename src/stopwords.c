#include "./includes/stopwords.h"
#include "utils.c"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// Puntero al arreglo dinamico que tendra las palabras;
static char** stopwords_list = NULL;
// Numero actual de stopwords almacenadsa;
static size_t stopwords_cantidad = 0;
// Tamanio inicial y factor de crecimiento del arreglo dinamico
static size_t stopwords_capacidad = 0;

// Def del tamanio inicial y el crecimiento del arreglo
#define CAPACIDAD_INICIAL_STOPWORDS 100
#define FACTOR_CRECIMIENTO_STOPWORDS 2

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
    stopwords_capacidad = CAPACIDAD_INICIAL_STOPWORDS;
    stopwords_list = (char **)malloc(stopwords_capacidad * sizeof(char*));
    if (stopwords_list == NULL) {
        fprintf(stderr, "Error al asignar memoria para stopwords %s\n");
        return false;
    }

    stopwords_cantidad = 0; // Inicio del contador;
    char buffer_linea[256]; // Buffer para cada linea;

    while (fgets(buffer_linea, sizeof(buffer_linea), archivo) != NULL) {
        // Eliminar salto de linea si existe
        size_t len = strlen(buffer_linea);
        if(len > 0 && buffer_linea[len-1] == '\n') {
            buffer_linea[len-1] = '\0';
            len--;
        }

        // Ignora las lineas vacias;
        if(len == 0) {
            continue;
        }

        if (stopwords_cantidad >= stopwords_capacidad ) {
            size_t nueva_capacidad = stopwords_capacidad * FACTOR_CRECIMIENTO_STOPWORDS;
            char** temp = (char**)realloc(stopwords_list, nueva_capacidad * sizeof(char*));
            if (temp == NULL) {
                fprintf(stderr, "Error al realloc de stopwords %s\n");
                fclose(archivo);
                return false;
            }
            stopwords_list = temp;
            stopwords_capacidad = nueva_capacidad;
        }

        stopwords_list[stopwords_cantidad] = (char*)malloc(len + 1);

        if (stopwords_list[stopwords_cantidad] == NULL) {
            fprintf(stderr, "Error: Fallo de memoria al guardar stopword (linea %zu aprox).\n", stopwords_cantidad + 1); // Mensaje específico
            fclose(archivo);
            return false; 
        } else {    
            strcpy(stopwords_list[stopwords_cantidad], buffer_linea);
        }

        // Aumentar el contador ya que se asigno todo correctamente
        stopwords_cantidad++;
    }
    fclose(archivo);
    printf("[*] Se cargaron %zu stopwords.\n", stopwords_cantidad);
    return true;
}


void free_stopwords() {
    if (stopwords_list != NULL) {
        for (size_t i = 0; i<stopwords_cantidad; ++i) {
            free(stopwords_list[i]);
            stopwords_list[i] = NULL;
        }
        free(stopwords_list);
        stopwords_list = NULL;
    }

    // RESETEAR CONTADORES
    stopwords_cantidad = 0;
    stopwords_capacidad = 0;

    printf("[*] Stopwords liberadas...");
}