#include "./includes/inverted_index.h"
#include "./includes/list.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

// --- Funciones Estáticas (auxiliares, internas a este archivo .c) ---

/**
 * @brief Busca un término en el array de entradas del índice.
 * Realiza una búsqueda lineal.
 * @param indice Puntero al índice invertido (const, no se modifica).
 * @param palabra El término a buscar.
 * @return ssize_t La posición (índice) del término en el array de entradas si se encuentra,
 * o -1 si el término no se encuentra.
 */
static ssize_t buscar_pos_termino(const indiceInvertido* indice, const char* palabra) {
    if (!indice || !palabra) { // Comprobación de parámetros nulos
        return -1;
    }
    for (size_t i = 0; i < indice->cantidad; i++) {
        // Asumimos que indice->entradas[i].palabra no será NULL si está dentro de indice->cantidad
        if (indice->entradas[i].palabra && strcmp(indice->entradas[i].palabra, palabra) == 0) {
            return (ssize_t)i;
        }
    }
    return -1; // No encontrado
}

/**
 * @brief Aumenta la capacidad del array de entradas del índice (usualmente duplicándola).
 * @param indice Puntero al índice invertido cuya capacidad se aumentará.
 * @return bool true si la capacidad se aumentó con éxito, false si falló (ej. realloc falló).
 */
static bool aumentar_capacidad(indiceInvertido* indice) {
    if (!indice) return false;

    size_t nueva_capacidad = (indice->capacidad == 0) ? 1 : indice->capacidad * 2; // Manejar capacidad inicial 0
    EntradaVocabulario* nuevo_array = (EntradaVocabulario*)realloc(indice->entradas, sizeof(EntradaVocabulario) * nueva_capacidad);
    
    if (!nuevo_array) {
        fprintf(stderr, "Error: Fallo al reasignar memoria para aumentar capacidad del indice.\n");
        return false;
    }

    // Inicializar la nueva porción del array
    for (size_t i = indice->capacidad; i < nueva_capacidad; i++) {
        nuevo_array[i].palabra = NULL;
        nuevo_array[i].list_documentos_cabeza = NULL;
    }

    indice->entradas = nuevo_array;
    indice->capacidad = nueva_capacidad;
    return true;
}

// --- Implementación de Funciones Públicas (declaradas en inverted_index.h) ---

indiceInvertido* crear_indice(size_t capacidad_inicial) {
    if (capacidad_inicial == 0) { // Es buena idea tener una capacidad mínima
        capacidad_inicial = 16; // O algún otro valor por defecto
    }

    indiceInvertido* idx = (indiceInvertido*)malloc(sizeof(indiceInvertido));
    if (!idx) {
        fprintf(stderr, "Error: Fallo al asignar memoria para la estructura del indice.\n");
        return NULL;
    }

    idx->cantidad = 0;
    idx->capacidad = capacidad_inicial;
    idx->entradas = (EntradaVocabulario*)malloc(sizeof(EntradaVocabulario) * capacidad_inicial);
    if (!idx->entradas) {
        fprintf(stderr, "Error: Fallo al asignar memoria para las entradas iniciales del indice.\n");
        free(idx); // Liberar la estructura del índice si falla la asignación de entradas
        return NULL;
    }

    // Inicializar todas las entradas iniciales
    for (size_t i = 0; i < capacidad_inicial; i++) {
        idx->entradas[i].palabra = NULL;
        idx->entradas[i].list_documentos_cabeza = NULL;
    }

    return idx;
}

void destruir_indice(indiceInvertido* indice) {
    if (!indice) {
        return;
    }

    for (size_t i = 0; i < indice->cantidad; i++) {
        free(indice->entradas[i].palabra); // Liberar la cadena de la palabra
        // Asumiendo que free_list está definida en list.h y funciona correctamente
        free_list(&(indice->entradas[i].list_documentos_cabeza)); // Liberar la lista enlazada de documentos
    }

    free(indice->entradas); // Liberar el array de entradas
    free(indice);           // Liberar la estructura del índice
}

// La firma en el .h es: void anadir_termino(indiceInvertido* indice, const char* palabra, const char* documento);
void anadir_termino(indiceInvertido* indice, const char* palabra, const char* documento) {
    if (!indice || !palabra || !documento) {
        // fprintf(stderr, "Advertencia: Argumento nulo pasado a anadir_termino.\n"); // Opcional: log de advertencia
        return;
    }

    ssize_t pos = buscar_pos_termino(indice, palabra);
    
    if (pos < 0) { // Término no encontrado, necesita ser añadido al vocabulario
        if (indice->cantidad >= indice->capacidad) { // Usar >= para estar seguro
            if (!aumentar_capacidad(indice)) {
                fprintf(stderr, "Error: No se pudo aumentar capacidad del indice. Termino '%s' no anadido.\n", palabra);
                return; // No se pudo aumentar la capacidad
            }
        }

        pos = indice->cantidad; // La nueva posición será al final

        // strdup asigna memoria y copia la cadena. Puede devolver NULL si falla malloc.
        indice->entradas[pos].palabra = strdup(palabra);
        if (indice->entradas[pos].palabra == NULL) {
            fprintf(stderr, "Error: Fallo de memoria al duplicar el termino '%s'. No anadido.\n", palabra);
            // No incrementamos 'cantidad' porque el término no se añadió exitosamente
            return; 
        }
        indice->entradas[pos].list_documentos_cabeza = NULL; // Nueva lista de posteo, inicialmente vacía
        indice->cantidad++; // Incrementar cantidad solo si se añadió un nuevo término al vocabulario con éxito
    }

    // Añadir/actualizar el documento en la lista de posteo para el término encontrado o recién añadido
    // Usar la función de list.h (asumiendo que se llama insertar_o_sumar_node)
    if (!insertar_o_sumar_node(&(indice->entradas[pos].list_documentos_cabeza), documento)) {
        // Esta función (insertar_o_sumar_node) debería manejar sus propios errores de memoria.
        // Si devuelve false y eso indica un error crítico, podríamos querer propagarlo.
        // fprintf(stderr, "Advertencia: No se pudo insertar/sumar documento '%s' para termino '%s'.\n", documento, palabra);
    }
}

// La firma en el .h es: nodePtr buscar_lista_posteo_termino(indiceInvertido* indice, const char* palabra);
// Es buena práctica que sea const si no modifica el índice.
nodePtr buscar_lista_posteo_termino(const indiceInvertido* indice, const char* palabra) {
    if (!indice || !palabra) {
        return NULL;
    }
    ssize_t pos = buscar_pos_termino(indice, palabra);
    return (pos >= 0) ? indice->entradas[pos].list_documentos_cabeza : NULL;
}

// La firma en el .h es: nodePtr intersectar_listas_posteo(nodePtr lista1, nodePtr lista2);
nodePtr intersectar_listas_posteo(nodePtr lista1, nodePtr lista2) {
    nodePtr resultado_interseccion = NULL;
    node* nodo_actual_lista1 = lista1;

    while (nodo_actual_lista1 != NULL) {
        node* nodo_actual_lista2 = lista2;
        while (nodo_actual_lista2 != NULL) {
            // Asumimos que nodo_actual_listaX->documento no es NULL
            if (strcmp(nodo_actual_lista1->documento, nodo_actual_lista2->documento) == 0) {
                // Documento encontrado en ambas listas, añadirlo al resultado
                // Usar la función de list.h
                insertar_o_sumar_node(&resultado_interseccion, nodo_actual_lista1->documento);
                // Si los IDs de documento son únicos dentro de una lista de posteo, podemos hacer break.
                break; 
            }
            nodo_actual_lista2 = nodo_actual_lista2->next;
        }
        nodo_actual_lista1 = nodo_actual_lista1->next;
    }
    return resultado_interseccion;
}