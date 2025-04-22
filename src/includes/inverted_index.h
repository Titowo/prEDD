#ifndef inverted_index_H_
#define inverted_index_H_
#include "list.h"
#include <stddef.h>

typedef struct {
    char* word;
    nodePtr palabra_head;
} vocabularyEntry;

typedef struct {
    vocabularyEntry* entradas; 
    size_t cantidad;
    size_t capacidad;
} indiceInvertido;

indiceInvertido* crear_indice(size_t capacidad_inicial);
void destruir_indice(indiceInvertido* index);
void anniadir_termino(indiceInvertido* index, const char* termino, const char* documento);
nodePtr busca_termino_lista_destino(indiceInvertido* index, const char* termino);
nodePtr intersect_lista_destino(nodePtr list1, nodePtr list2);

#endif 