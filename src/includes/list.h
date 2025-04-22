#ifndef list_H
#define list_H

#include <stdbool.h>

// Definicion nodos para este caso
typedef struct node {
    char* documento;
    int frecuencia;
    struct node * next;
} node;

// Definicion nodo puntero como tipo
typedef node* nodePtr;

// Funciones asociadas a los nodos
nodePtr crear_nodo(const char* documento);
bool insertar_o_sumar_node(nodePtr* head, const char* documento);
void free_list(nodePtr* head);
void print_list(nodePtr head);

#endif