#ifndef list_H
#define list_H

#include <stdbool.h>

/**
 * @brief Define la estructura de un nodo para la lista enlazada especifica de documentos.
 * Cada nodo representa la aparicion de una palabra en el documento. 
 */
typedef struct node {
    char* documento;    // Identificador del documento.
    int frecuencia;     // Frecuencia de aparicion de este documento.
    struct node * next; // Puntero al siguiente nodo de la lista o NULL si es el ultimo.
} node;

/**
 * @brief Definicion de alias para la estructura "node" para su mejor lectura.
 */
typedef node* nodePtr;

// ---- Prototipos de Funciones de los nodos ----

/**
 * @brief Crea un nuevo nodo para la lista de documentos.
 * Asigna memoria para el nodo.
 * Inicializa la frecuencia a 1 y setea el puntero "next" a NULL.
 * @param documento Es el identificador del documento para el nodo.
 * @return nodePtr Puntero al nodo recien creado, o NULL si ocurre un error o no se logra asignar memoria.
 */
nodePtr crear_nodo(const char* documento);


/**
 * @brief 
 */
bool insertar_o_sumar_node(nodePtr* head, const char* documento);
void free_list(nodePtr* head);
void print_list(nodePtr head);

#endif