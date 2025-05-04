#include "list.c"
// #include "inverted_index.h"

int main(int argc, char const *argv[])
{
    char* s1 = "Hello World";
    nodePtr a = crear_nodo(s1);
    print_list(a);
    insertar_o_sumar_node(a, s1);
    free_list(a);

    return 0;
}
