#include "includes/inverted_index.h"
#include "includes/list.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h> 
#include <stdbool.h>


#ifndef ssize_t
#define ssize_t ptrdiff_t
#endif

// --- Funciones Estáticas ---
static ssize_t buscar_pos_termino(const indiceInvertido* indice, const char* palabra) {
    if (!indice || !palabra) {
        return -1;
    }
    for (size_t i = 0; i < indice->cantidad; i++) {
        if (indice->entradas[i].palabra && strcmp(indice->entradas[i].palabra, palabra) == 0) {
            return (ssize_t)i;
        }
    }
    return -1;
}

static bool aumentar_capacidad(indiceInvertido* indice) {
    if (!indice) return false;
    size_t nueva_capacidad = (indice->capacidad == 0) ? 16 : indice->capacidad * 2; // Empezar con algo si es 0
    EntradaVocabulario* nuevo_array = (EntradaVocabulario*)realloc(indice->entradas, sizeof(EntradaVocabulario) * nueva_capacidad);
    if (!nuevo_array) {
        fprintf(stderr, "[INDEX] Error: Fallo al reasignar memoria para aumentar capacidad del indice.\n");
        return false;
    }
    for (size_t i = indice->capacidad; i < nueva_capacidad; i++) {
        nuevo_array[i].palabra = NULL;
        nuevo_array[i].list_documentos_cabeza = NULL;
    }
    indice->entradas = nuevo_array;
    indice->capacidad = nueva_capacidad;
    // Descomenta para ver cuándo crece el vocabulario
    // printf("    [INDEX_info] Capacidad del vocabulario aumentada a %zu entradas.\n", nueva_capacidad);
    return true;
}

// --- Implementación de Funciones Públicas (declaradas en inverted_index.h) ---

indiceInvertido* crear_indice(size_t capacidad_inicial) {
    if (capacidad_inicial == 0) {
        capacidad_inicial = 256; // Una capacidad inicial un poco más generosa.
    }
    indiceInvertido* idx = (indiceInvertido*)malloc(sizeof(indiceInvertido));
    if (!idx) {
        perror("[INDEX] Fallo malloc para la estructura del indice");
        return NULL;
    }
    idx->cantidad = 0;
    idx->capacidad = capacidad_inicial;
    idx->entradas = (EntradaVocabulario*)malloc(sizeof(EntradaVocabulario) * capacidad_inicial);
    if (!idx->entradas) {
        perror("[INDEX] Fallo malloc para las entradas iniciales del indice");
        free(idx);
        return NULL;
    }
    for (size_t i = 0; i < capacidad_inicial; i++) {
        idx->entradas[i].palabra = NULL;
        idx->entradas[i].list_documentos_cabeza = NULL;
    }
    printf("[INDEX_info] Indice creado con capacidad inicial para %zu palabras.\n", capacidad_inicial);
    return idx;
}

void destruir_indice(indiceInvertido* indice) {
    if (!indice) return;
    printf("[INDEX_info] Destruyendo indice. Liberando %zu entradas del vocabulario...\n", indice->cantidad);
    for (size_t i = 0; i < indice->cantidad; i++) {
        free(indice->entradas[i].palabra);
        // free_list es de list.h
        free_list(&(indice->entradas[i].list_documentos_cabeza));
    }
    free(indice->entradas);
    free(indice);
    printf("[INDEX_info] Indice destruido completamente.\n");
}


void anadir_termino(indiceInvertido* indice, const char* palabra, const char* documento) {
    if (!indice || !palabra || !documento || strlen(palabra) == 0) { // Añadí strlen(palabra) == 0
        return;
    }

    ssize_t pos = buscar_pos_termino(indice, palabra);
    
    if (pos < 0) {
        if (indice->cantidad >= indice->capacidad) {
            if (!aumentar_capacidad(indice)) {
                fprintf(stderr, "[INDEX] Error: No se pudo aumentar capacidad. Termino '%s' para doc '%s' no añadido.\n", palabra, documento);
                return;
            }
        }
        pos = indice->cantidad;
        indice->entradas[pos].palabra = strdup(palabra);
        if (indice->entradas[pos].palabra == NULL) {
            perror("[INDEX] Fallo strdup para nueva palabra en vocabulario");
            return; 
        }
        indice->entradas[pos].list_documentos_cabeza = NULL;
        indice->cantidad++;

        if (indice->cantidad % 5000 == 0 || indice->cantidad <= 10) {
             printf("    [INDEX_info] Palabra nueva en vocabulario: '%s' (Total vocabulario: %zu)\n", palabra, indice->cantidad);
        }
    }


    if (!insertar_o_sumar_node(&(indice->entradas[pos].list_documentos_cabeza), documento)) {
        // fprintf(stderr, "[INDEX_warn] No se pudo añadir/actualizar doc '%s' en lista de posteo para '%s'.\n", documento, palabra);
    }
}


nodePtr buscar_lista_posteo_termino(const indiceInvertido* indice, const char* palabra) {
    if (!indice || !palabra) {
        return NULL;
    }
    ssize_t pos = buscar_pos_termino(indice, palabra);
    return (pos >= 0) ? indice->entradas[pos].list_documentos_cabeza : NULL;
}


nodePtr intersectar_listas_posteo(nodePtr lista1, nodePtr lista2) {
    nodePtr resultado_interseccion = NULL;
    node* nodo_actual_lista1 = lista1;

    while (nodo_actual_lista1 != NULL) {
        node* nodo_actual_lista2 = lista2;
        while (nodo_actual_lista2 != NULL) {
            if (strcmp(nodo_actual_lista1->documento, nodo_actual_lista2->documento) == 0) {
                insertar_o_sumar_node(&resultado_interseccion, nodo_actual_lista1->documento);
                break; 
            }
            nodo_actual_lista2 = nodo_actual_lista2->next;
        }
        nodo_actual_lista1 = nodo_actual_lista1->next;
    }
    return resultado_interseccion;
}
