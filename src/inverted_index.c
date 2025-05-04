#include "./includes/inverted_index.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

indiceInvertido* crear_indice(size_t capacidad_inicial) {
    indiceInvertido* idx = malloc(sizeof(indiceInvertido));
    if (!idx) return NULL;

    idx->cantidad = 0;
    idx->capacidad = capacidad_inicial;
    idx->entradas = malloc(sizeof(EntradaVocabulario) * capacidad_inicial);
    if (!idx->entradas) {
        free(idx);
        return NULL;
    }

    for (size_t i = 0; i < capacidad_inicial; i++) {
        idx->entradas[i].palabra = NULL;
        idx->entradas[i].list_documentos_cabeza = NULL;
    }

    return idx;
}

void destruir_indice(indiceInvertido* idx) {
    if (!idx) return;

    for (size_t i = 0; i < idx->cantidad; i++) {
        free(idx->entradas[i].palabra);
        lista_posteo_liberar(&idx->entradas[i].list_documentos_cabeza);
    }

    free(idx->entradas);
    free(idx);
}

static ssize_t buscar_pos_termino(const indiceInvertido* idx, const char* palabra) {
    for (size_t i = 0; i < idx->cantidad; i++) {
        if (strcmp(idx->entradas[i].palabra, palabra) == 0) {
            return (ssize_t)i;
        }
    }
    return -1;
}

static bool aumentar_capacidad(indiceInvertido* idx) {
    size_t nueva_capacidad = idx->capacidad * 2;
    EntradaVocabulario * nuevo_array = realloc(idx->entradas, sizeof(EntradaVocabulario) * nueva_capacidad);
    if (!nuevo_array) return false;

    for (size_t i = idx->capacidad; i < nueva_capacidad; i++) {
        nuevo_array[i].palabra = NULL;
        nuevo_array[i].list_documentos_cabeza = NULL;
    }

    idx->entradas = nuevo_array;
    idx->capacidad = nueva_capacidad;
    return true;
}

void indice_invertido_agregar_termino(indiceInvertido* idx, const char* palabra, const char* documento) {
    ssize_t pos = buscar_pos_termino(idx, palabra);
    
    if (pos < 0) {
        if (idx->cantidad == idx->capacidad && !aumentar_capacidad(idx)) {
            return;
        }

        pos = idx->cantidad++;
        idx->entradas[pos].palabra = strdup(palabra);
        idx->entradas[pos].list_documentos_cabeza = NULL;
    }

    lista_posteo_insertar_o_incrementar(&idx->entradas[pos].list_documentos_cabeza, documento);
}

ListaPosteo indice_invertido_obtener_posteo(const indiceInvertido* idx, const char* palabra) {
    ssize_t pos = buscar_pos_termino(idx, palabra);
    return (pos >= 0) ? idx->entradas[pos].list_documentos_cabeza : NULL;
}

ListaPosteo lista_posteo_intersectar(const nodePtr a, const nodePtr b) {
    ListaPosteo resultado = NULL;

    for (NodoPosteo* nodoA = a; nodoA != NULL; nodoA = nodoA->siguiente) {
        for (NodoPosteo* nodoB = b; nodoB != NULL; nodoB = nodoB->siguiente) {
            if (strcmp(nodoA->documento, nodoB->documento) == 0) {
                lista_posteo_insertar_o_incrementar(&resultado, nodoA->documento);
                break;
            }
        }
    }

    return resultado;
}
