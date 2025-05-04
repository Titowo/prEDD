#include "./includes/list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

node* crear_nodo(const char* documento) {
    node* nuevo = malloc(sizeof(node));
    if (!nuevo) {
        perror("No se pudo crear nodo");
        return NULL;
    }

    nuevo->documento = strdup(documento);
    if (!nuevo->documento) {
        free(nuevo);
        return NULL;
    }

    nuevo->frecuencia = 1;
    nuevo->next = NULL;
    return nuevo;
}

bool insertar_o_sumar_node(nodePtr * lista, const char* documento) {
    node* actual = *lista;

    while (actual) {
        if (strcmp(actual->documento, documento) == 0) {
            actual->frecuencia++;
            return false;
        }
        actual = actual->next;
    }

    node* nuevo = crear_nodo(documento);
    if (!nuevo) return false;

    nuevo->next = *lista;
    *lista = nuevo;
    return true;
}

void free_list(nodePtr* head) {
    node* aux;
    while (*head) {
        aux = *head;
        *head = (*head)->next;
        free(aux->documento);
        free(aux);
    }
}

void print_list(const nodePtr head) {
    node* actual = head;
    while (actual) {
        printf("%s (freq: %d)\n", actual->documento, actual->frecuencia);
        actual = actual->next;
    }
}