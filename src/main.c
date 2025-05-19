#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// --- Nuestros Modulos ---
#include "includes/stopwords.h"
#include "includes/list.h"
#include "includes/inverted_index.h"
#include "includes/parser.h"

#define MAX_LARGO_CONSULTA 256   // Maximo de caracteres para la consulta del usuario.
#define MAX_TERMINOS_CONSULTA 20 // Maximo de palabras "utiles" en una consulta.

// Funcion auxiliar local para pasar la consulta del usuario a minusculas.
static void main_convertir_a_minusculas(char *cadena) {
    if (!cadena) return; // Por si las moscas.
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower((unsigned char)cadena[i]);
    }
}

void imprimir_uso(const char* nombre_programa) {
    printf("Uso: %s [<ruta_archivo_stopwords> <ruta_archivo_documentos>]\n", nombre_programa);
    printf("  Si no se especifican rutas, se usarán los valores por defecto:\n");
    printf("    Archivo de Stopwords: data/stopwords_english.dat.txt\n");
    printf("    Archivo de Documentos: data/gov2_pages.dat\n");
}


int main(int argc, char* argv[]) {

    const char* archivo_stopwords_path;
    const char* archivo_documentos_path;

    if (argc == 1) {
        printf("[MAIN_info] No se especificaron las rutas de archivos, usando valores por defecto.");
        archivo_stopwords_path    = "data/stopwords_english.dat.txt";
        archivo_documentos_path   = "data/gov2_pages.dat";
    } else if (argc == 3) {
        archivo_stopwords_path = argv[1];
        archivo_documentos_path = argv[2];
        printf("[MAIN_INFO] Usando ruta de stopwords: %s\n", archivo_stopwords_path);
        printf("[MAIN_INFO] Usando ruta de documentos: %s\n", archivo_documentos_path);
    } else {
        fprintf(stderr, "[MAIN_ERROR] Número incorrecto de argumentos.\n");
        imprimir_uso(argv[0]);
        return EXIT_FAILURE;
    }

    printf("------------------------------------------------------------\n");
    printf("--- Mi Buscador Personalizado - Version 1.0 ---\n");
    printf("------------------------------------------------------------\n\n");

    printf("[MAIN] Cargando stopwords desde '%s'. Un poquito de paciencia...\n", archivo_stopwords_path);
    if (!cargar_stopwords(archivo_stopwords_path)) {
        fprintf(stderr, "[MAIN] Fallo la carga de stopwords! Asi no podemos seguir, el programa termina.\n");
        return EXIT_FAILURE;
    }

    printf("[MAIN] Stopwords listas y dispuestas para ser ignoradas!\n\n");

    printf("[MAIN] Creando el indice invertido...\n");

    indiceInvertido* mi_indice = crear_indice(2048);
    if (!mi_indice) {
        fprintf(stderr, "[MAIN] Fallo la creacion del indice invertido! Problemas de memoria quizas.\n");
        free_stopwords();
        return EXIT_FAILURE;
    }
    printf("[MAIN] Indice invertido listo para recibir datos.\n\n");

    printf("[MAIN] Procesando documentos desde '%s' para llenar el indice...\n", archivo_documentos_path);

    if (!procesar_archivo_documento(archivo_documentos_path, mi_indice)) {
        fprintf(stderr, "[MAIN] Hubo un problema procesando los documentos. El indice podria estar incompleto.\n");
    } else {
        printf("[MAIN] Documentos procesados. El indice tiene %zu palabras unicas.\n\n", mi_indice->cantidad);
    }

    char consulta_del_usuario[MAX_LARGO_CONSULTA];
    printf("\n------------------------------------------\n");
    printf("--- YA PUEDES HACER TUS CONSULTAS! ---\n");
    printf("Escribe lo que buscas (o 'chao' para terminar la conversa):\n");

    while (true) {
        printf("\nTu Consulta > ");
        if (fgets(consulta_del_usuario, sizeof(consulta_del_usuario), stdin) == NULL) {
            printf("\n[MAIN] Fin de la entrada (EOF) o error leyendo. ¡Nos vemos!\n");
            break;
        }

        consulta_del_usuario[strcspn(consulta_del_usuario, "\n")] = '\0';

        if (strcmp(consulta_del_usuario, "chao") == 0) {
            printf("[MAIN] ¡Vale, que te vaya bien!\n");
            break;
        }

        if (strlen(consulta_del_usuario) == 0) { continue; }

        printf("[MAIN] Procesando: \"%s\"\n", consulta_del_usuario);

        char copia_consulta[MAX_LARGO_CONSULTA];
        strcpy(copia_consulta, consulta_del_usuario);
        main_convertir_a_minusculas(copia_consulta);

        char* terminos_validos[MAX_TERMINOS_CONSULTA];
        int num_terminos_validos = 0;
        const char* delimitadores_consulta = " \t\n\r\f\v,.;:!?()[]{}-\"\'“”‘’"; // Hartos delimitadores.
        char* token = strtok(copia_consulta, delimitadores_consulta);

        while (token != NULL && num_terminos_validos < MAX_TERMINOS_CONSULTA) {
            if (strlen(token) > 0 && !es_stopword(token)) { 
                terminos_validos[num_terminos_validos++] = token;
            }
            token = strtok(NULL, delimitadores_consulta);
        }

        if (num_terminos_validos == 0) {
            printf("  Mmm, tu consulta no tiene palabras que sirvan despues de filtrar. Intenta de nuevo.\n");
            continue;
        }

        printf("  Buscando %d termino(s) clave: ", num_terminos_validos);
        for(int i = 0; i < num_terminos_validos; ++i) printf("'%s' ", terminos_validos[i]);
        printf("\n");

        nodePtr lista_resultado_final = NULL;
        bool es_primera_lista_valida = true;

        for (int i = 0; i < num_terminos_validos; ++i) {
            nodePtr lista_del_termino_actual = buscar_lista_posteo_termino(mi_indice, terminos_validos[i]);

            if (!lista_del_termino_actual) {
                printf("  El termino '%s' no lo tenemos registrado.\n", terminos_validos[i]);
                free_list(&lista_resultado_final);
                lista_resultado_final = NULL;
                break;
            }

            if (es_primera_lista_valida) {
                nodePtr nodo_aux_copia = lista_del_termino_actual;
                while(nodo_aux_copia) {
                    insertar_o_sumar_node(&lista_resultado_final, nodo_aux_copia->documento);
                    nodo_aux_copia = nodo_aux_copia->next;
                }
                es_primera_lista_valida = false;
                if (!lista_resultado_final && num_terminos_validos > 0) {
                     printf("  Problemas al armar la lista inicial con '%s'.\n", terminos_validos[i]);
                     break;
                }
            } else {

                nodePtr lista_intermedia = intersectar_listas_posteo(lista_resultado_final, lista_del_termino_actual);
                free_list(&lista_resultado_final);
                lista_resultado_final = lista_intermedia;
                if (lista_resultado_final == NULL) {

                    printf("  Parece que '%s' no tiene documentos en comun con los terminos anteriores.\n", terminos_validos[i]);
                    break;
                }
            }
        }

        if (lista_resultado_final != NULL) {
            printf("--- Resultados! Documentos que contienen todos los terminos que buscaste: ---\n");
            print_list(lista_resultado_final);
            free_list(&lista_resultado_final);
        } else {
            printf("Pucha, no encontramos documentos que tengan todos esos terminos juntos.\n");
        }
    }

    printf("\n[MAIN] Limpiando y liberando toda la memoria...\n");
    if (mi_indice) {
        destruir_indice(mi_indice);
        printf("[MAIN] Indice invertido liberado.\n");
    }
    free_stopwords();

    printf("\n[MAIN] Mision cumplida! Programa terminado. Que estes bien.\n");
    return EXIT_SUCCESS;
}
