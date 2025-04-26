#ifndef list_H
#define list_H

#include <stdbool.h> // Necesario para el tipo bool

/**
 * @brief Define la estructura de un nodo para la lista enlazada de documentos (lista de posteo).
 * Cada nodo representa un documento donde aparece un término específico del índice.
 */
typedef struct node {
    /** @brief Identificador del documento (ej. URL). Cadena asignada dinámicamente. */
    char* documento;
    /** @brief Frecuencia de aparición del término en este documento. */
    int frecuencia;
    /** @brief Puntero al siguiente nodo en la lista, o NULL si es el último. */
    struct node * next;
} node;

/** @brief Define un alias para un puntero a la estructura 'node' para conveniencia. */
typedef node* nodePtr;

// ---- Prototipos de Funciones de la lista enlazada de posteo ----

/**
 * @brief Crea un nuevo nodo para la lista de posteo.
 * Asigna memoria para el nodo y para una copia del ID del documento.
 * Inicializa la frecuencia a 1 y el puntero 'next' a NULL.
 * @param documento El identificador del documento para este nodo. Se creará una copia interna.
 * @return nodePtr Puntero al nodo recién creado, o NULL si falla la asignación de memoria.
 */
nodePtr crear_nodo(const char* documento);

/**
 * @brief Inserta un nuevo documento en la lista de posteo o incrementa la frecuencia si ya existe.
 * Busca el 'documento' en la lista referenciada por 'head'.
 * Si lo encuentra, incrementa el campo 'frecuencia' de ese nodo y retorna false.
 * Si no lo encuentra, crea un nuevo nodo usando crear_nodo() y lo inserta
 * al principio de la lista (modificando *head), retornando true.
 * @param head Puntero al puntero de la cabeza de la lista (nodePtr*). Se necesita para modificar
 * la cabeza de la lista si se inserta un nuevo nodo al principio.
 * @param documento El identificador del documento a insertar o cuya frecuencia incrementar.
 * @return bool Devuelve true si se insertó un nuevo nodo, false si solo se incrementó
 * la frecuencia o si ocurrió un error en crear_nodo().
 */
bool insertar_o_sumar_node(nodePtr* head, const char* documento);

/**
 * @brief Libera toda la memoria asociada a una lista de posteo.
 * Recorre la lista desde la cabeza (*head), liberando la memoria de la cadena 'documento'
 * y la memoria del nodo mismo para cada elemento de la lista.
 * Finalmente, establece *head a NULL.
 * @param head Puntero al puntero de la cabeza de la lista (nodePtr*) a liberar.
 * Después de la llamada, *head será NULL.
 */
void free_list(nodePtr* head);

/**
 * @brief Imprime el contenido de una lista de posteo (para depuración).
 * Recorre la lista e imprime el ID del documento y la frecuencia de cada nodo.
 * @param head Puntero a la cabeza de la lista a imprimir.
 */
void print_list(nodePtr head);

#endif // list_H