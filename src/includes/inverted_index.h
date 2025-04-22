#ifndef inverted_index_H_
#define inverted_index_H_
#include "list.h"
#include <stddef.h>

/**
 * @brief Defina la entrada del vocabulario para mapear una termino (palabra)
 * a la cabeza de su lista de documentos (lista_documentos_cabeza).
**/
typedef struct {
    char* palabra;                  // El termino (palabra) del vocabulario.
    nodePtr list_documentos_cabeza; // Puntero a la cabeza de la lista de docs donde aparezca la palabra.
} vocabularyEntry;

/**
 * @brief Define la estructura principal del indice invertido que contiene un array dinamico
 * de entradas del vocabulario.
**/
typedef struct {
    vocabularyEntry* entradas; // Array dinamico de las entradas del vocabulario. 
    size_t cantidad;           // Numero actual de entradas (palabras unicas) en el indice.
    size_t capacidad;          // Capacidad actual del array "entradas".
} indiceInvertido;

// --- Prototipo de funciones de inidiceInvertido ---

/**
 * @brief Crea y inicializa una nueva estructura de indice invertido.
 * Asigna memoria para la estructura del indice y para el array inicial de entradas
 * del vocabulario segun la capacidad especifica.
**/
indiceInvertido* crear_indice(size_t capacidad_inicial);

/**
 * @brief Libera memoria a un indice asociado.
 * NO retorna nada porque avisa unicamente si se pudo lograr.
 */
void destruir_indice(indiceInvertido* index);

/**
 * @brief Anniade un termino a un doc especifico al indice invertido.
 * Si ya existe el termino unicamente suma el documento a su lista de documentos.
 * (o suma a la frecuencia si el documento ya existia en la lista).
 * Si el termino no existe lo suma al vocabulario, crea una nueva lista de documentos
 * para el y anniade el doc a la lista.
 * @param index puntero hacia el indice invertido que se modifica.
 * @param palabra el termino (palabra) que se encontro.
 * @param documento el identificado de documento en el que se encontro la palabra. 
**/
void anniadir_termino(indiceInvertido* index, const char* palabra, const char* documento);

/**
 * @brief busca un termino en el indice y devuelve el puntero de la cabeza de su lista de posteo.
 ** @param index es el puntero al indice invertido que hay que buscar.
 ** @param termino la palabra que se busca en el vocabulario del indice.
 * @return nodePtr puntero de la lista enlazada de documentos
 * devuelve NULL si el termino no se encuentra en el indice.
**/
nodePtr busca_termino_lista_destino(indiceInvertido* index, const char* termino);

/**
 * @brief Calcula la interseccion de dos lista de posteo y devuelve un puntero cabeza de su lista de posteo.
 * ! IMPORTANTE: esta funcion CREA y DEVUELVE una NUEVA LISTA. El que llama esta funcion debe de liberar bien
 * la memoria que se devuelve usan la funcion free_list() para no modificar las listas originales.
 ** @param list1 Puntero a la cabeza de la primera lista de posteo.
 * @param list2 Puntero a la cabeza de la segunda lista de posteo.
 * Devuelve NULL si la interseccion es vacia o si ocurre un error en memoria.
**/
nodePtr intersect_lista_destino(nodePtr list1, nodePtr list2);

#endif 