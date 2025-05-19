#include "includes/stopwords.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// --- Variables Estáticas (internas a este módulo) ---
static char** g_stopwords_list = NULL;
static size_t g_stopwords_cantidad = 0;
static size_t g_stopwords_capacidad = 0;

// Def del tamanio inicial y el crecimiento del arreglo
#define CAPACIDAD_INICIAL_STOPWORDS 100
#define FACTOR_CRECIMIENTO_STOPWORDS 2

// --- Funciones Estáticas (Ayudantes Internos) ---

/**
 * @brief Convierte una cadena a minúsculas, in-place.
 * Es 'static' porque solo la vamos a usar dentro de este archivo stopwords.c.
 * @param cadena La cadena a modificar.
 */
static void stopwords_convertir_a_minusculas(char *cadena) {
    if (!cadena) return;
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower((unsigned char)cadena[i]);
    }
}

// --- Implementación de Funciones Públicas ---

bool cargar_stopwords(const char* filename) {
    FILE *archivo = fopen(filename, "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error: no se pudo abrir el archivo de STOPWORDS '%s': %s\n", filename, strerror(errno));
        return false;
    }

    if (g_stopwords_list != NULL) {
        free_stopwords(); 
    }

    // COMIENZO ARREGLO DINAMICO
    g_stopwords_capacidad = CAPACIDAD_INICIAL_STOPWORDS;
    g_stopwords_list = (char **)malloc(g_stopwords_capacidad * sizeof(char*));
    if (g_stopwords_list == NULL) {
        fprintf(stderr, "Error al asignar memoria para la lista de stopwords: %s\n", strerror(errno));
        fclose(archivo);
        return false;
    }

    g_stopwords_cantidad = 0;
    char buffer_linea[256];

    while (fgets(buffer_linea, sizeof(buffer_linea), archivo) != NULL) {
        size_t len = strlen(buffer_linea);
        if (len > 0 && buffer_linea[len - 1] == '\n') {
            buffer_linea[len - 1] = '\0';
            len--;
        }

        if (len == 0) { 
            continue;
        }

        stopwords_convertir_a_minusculas(buffer_linea);

        if (g_stopwords_cantidad >= g_stopwords_capacidad) {
            size_t nueva_capacidad = (g_stopwords_capacidad == 0) ? CAPACIDAD_INICIAL_STOPWORDS : g_stopwords_capacidad * FACTOR_CRECIMIENTO_STOPWORDS;
            char** temp = (char**)realloc(g_stopwords_list, nueva_capacidad * sizeof(char*));
            if (temp == NULL) {
                fprintf(stderr, "Error al redimensionar (realloc) la memoria para stopwords: %s\n", strerror(errno));
                fclose(archivo);

                return false;
            }
            g_stopwords_list = temp;
            g_stopwords_capacidad = nueva_capacidad;
        }

        g_stopwords_list[g_stopwords_cantidad] = strdup(buffer_linea);
        if (g_stopwords_list[g_stopwords_cantidad] == NULL) {
            fprintf(stderr, "Error: Fallo de memoria al guardar stopword '%s' (linea %zu aprox.): %s\n", buffer_linea, g_stopwords_cantidad + 1, strerror(errno));
            fclose(archivo);

            return false;
        }
        g_stopwords_cantidad++;
    }

    fclose(archivo);

    printf("[STOPWORDS] Se cargaron %zu stopwords desde '%s'.\n", g_stopwords_cantidad, filename);
    return true;
}

void free_stopwords() {
    if (g_stopwords_list != NULL) {
        for (size_t i = 0; i < g_stopwords_cantidad; ++i) {
            free(g_stopwords_list[i]);
            g_stopwords_list[i] = NULL;
        }
        free(g_stopwords_list);
        g_stopwords_list = NULL;
    }
    // RESETEAR CONTADORES
    g_stopwords_cantidad = 0;
    g_stopwords_capacidad = 0;
    printf("[STOPWORDS] Memoria de stopwords liberada.\n");
}


bool es_stopword(const char* word) {
    if (!word || !g_stopwords_list || g_stopwords_cantidad == 0) {
        return false;
    }

    char buffer_palabra_minuscula[256];
    strncpy(buffer_palabra_minuscula, word, sizeof(buffer_palabra_minuscula) - 1);
    buffer_palabra_minuscula[sizeof(buffer_palabra_minuscula) - 1] = '\0';
    stopwords_convertir_a_minusculas(buffer_palabra_minuscula);

    for (size_t i = 0; i < g_stopwords_cantidad; ++i) {

        if (strcmp(buffer_palabra_minuscula, g_stopwords_list[i]) == 0) {
            return true;
        }
    }
    return false;
}
