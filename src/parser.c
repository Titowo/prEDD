#include "includes/parser.h"
#include "includes/stopwords.h"
#include "includes/inverted_index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Función estática para convertir a minúsculas, la necesitamos aquí.
static void parser_convertir_a_minusculas(char *cadena) {
    if (!cadena) return;
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower((unsigned char)cadena[i]);
    }
}

// En tu parser.h los params son linea_original, url_salida, contenido_salida
bool parsear_linea(char* linea_original, char** url_salida, char** contenido_salida) {
    if (!linea_original || !url_salida || !contenido_salida) {
        // Este es más un safeguard, no debería pasar si el bucle de lectura está bien.
        // fprintf(stderr, "[PARSER_DEBUG] Argumentos nulos en parsear_linea.\n");
        return false;
    }

    *url_salida = NULL;
    *contenido_salida = NULL;

    char* ultimo_separador = NULL;
    char* puntero_busqueda = linea_original;
    char* ocurrencia_actual;

    while ((ocurrencia_actual = strstr(puntero_busqueda, "||")) != NULL) {
        ultimo_separador = ocurrencia_actual;
        puntero_busqueda = ocurrencia_actual + 2; // Avanzamos para buscar la siguiente ocurrencia.
    }

    if (ultimo_separador == NULL) {
        // La línea no tiene el formato "URL || Contenido" que esperamos.
        return false;
    }

    size_t largo_url = ultimo_separador - linea_original;
    *url_salida = (char*)malloc(largo_url + 1);
    if (!*url_salida) {
        perror("[PARSER] Fallo malloc para url_salida en parsear_linea");
        return false;
    }
    strncpy(*url_salida, linea_original, largo_url);
    (*url_salida)[largo_url] = '\0';

    char* inicio_contenido = ultimo_separador + 2; // Saltamos el "||"
    while (*inicio_contenido && isspace((unsigned char)*inicio_contenido)) {
        inicio_contenido++; // Saltamos espacios al inicio del contenido.
    }

    *contenido_salida = strdup(inicio_contenido);
    if (!*contenido_salida) {
        perror("[PARSER] Falló strdup para contenido_salida en parsear_linea");
        free(*url_salida);
        *url_salida = NULL;
        return false;
    }
    // Descomenta si quieres ver cada URL que se parsea (puede ser mucho)
    // printf("    [PARSER_info] URL Parseada: %.70s...\n", *url_salida);
    return true;
}

// En tu parser.h los params son contenido_const, documento_id, index
void tokenizar_e_indexar_contenido(const char* contenido_const, const char* documento_id, indiceInvertido* indice) {
    if (!contenido_const || !documento_id || !indice) {
        return; // Sin los ingredientes, no hay receta.
    }

    char* contenido_mutable = strdup(contenido_const);
    if (!contenido_mutable) {
        perror("[PARSER] Fallo strdup para contenido_mutable en tokenizar_e_indexar_contenido");
        return;
    }

    // printf("    [PARSER_info] Tokenizando para DocID: %.70s...\n", documento_id); //VERBOSE
    int terminos_indexados_este_doc = 0;
    const char* delimitadores = " \t\n\r\f\v,.;:!?()[]{}-\"\'“”‘’"; // Buena artillería de separadores.
    char* token = strtok(contenido_mutable, delimitadores);

    while (token != NULL) {
        parser_convertir_a_minusculas(token);
        if (strlen(token) > 0 && !es_stopword(token)) { // Ojo, es_stopword es de stopwords.h
            // Descomenta si quieres ver cada término que se intenta indexar (¡serán millones!)
            // printf("      [PARSER_info] Indexando término: '%s' en DocID: %s\n", token, documento_id);
            anadir_termino(indice, token, documento_id); // Esta es de inverted_index.h
            terminos_indexados_este_doc++;
        }
        token = strtok(NULL, delimitadores);
    }
    // Descomenta si quieres un resumen por documento
    // if (terminos_indexados_este_doc > 0) {
    //    printf("    [PARSER_info] DocID %s: %d términos útiles indexados.\n", documento_id, terminos_indexados_este_doc);
    // }
    free(contenido_mutable); // No olvidar la copia.
}

// En tu parser.h los params son nombre_archivo, index
bool procesar_archivo_documento(const char* nombre_archivo, indiceInvertido* index) {
    if (!nombre_archivo || !index) {
        fprintf(stderr, "[PARSER] Error: Nombre de archivo o índice nulos en procesar_archivo_documento.\n");
        return false;
    }

    FILE* archivo_docs = fopen(nombre_archivo, "r");
    if (!archivo_docs) {
        fprintf(stderr, "[PARSER] No se pudo abrir el archivo de documentos '%s'! Error: %s\n", nombre_archivo, strerror(errno));
        return false;
    }

    // El mensaje inicial ya lo pone el main.c
    // printf("[PARSER] Abierto '%s'. Empezando a leer línea por línea...\n", nombre_archivo);

    char buffer_linea[8192]; // Un buffer generoso, por si las líneas son kilométricas.
    long contador_lineas_leidas = 0;
    long lineas_parseadas_ok = 0;
    long lineas_con_formato_malo = 0;

    while (fgets(buffer_linea, sizeof(buffer_linea), archivo_docs) != NULL) {
        contador_lineas_leidas++;

        // Un reporte de cómo vamos, pa' no creer que se pegó.
        // Lo ajusté para que no reporte tan seguido si el archivo es muy grande.
        if (contador_lineas_leidas % 100000 == 0) { // Cada 100,000 líneas
            printf("[PARSER] ... procesando línea %ld ... (%ld parseadas OK, %ld con formato malo)\n",
                   contador_lineas_leidas, lineas_parseadas_ok, lineas_con_formato_malo);
        }

        buffer_linea[strcspn(buffer_linea, "\n")] = '\0'; // Sacamos el \n que deja fgets.

        char* url = NULL;
        char* contenido = NULL;

        // En tu parser.h los params de parsear_linea son linea_original, url_salida, contenido_salida
        if (parsear_linea(buffer_linea, &url, &contenido)) {
            // En tu parser.h los params de tokenizar son contenido_const, documento_id, index
            tokenizar_e_indexar_contenido(contenido, url, index);
            free(url); // Liberamos lo que parsear_linea nos dio.
            free(contenido);
            lineas_parseadas_ok++;
        } else {
            // Si la línea no tenía el formato "URL || Contenido", la contamos pero no la procesamos.
            lineas_con_formato_malo++;
            // Descomenta si quieres ver cada línea que no se pudo parsear (puede ser mucho)
            // fprintf(stderr, "[PARSER_warn] Línea %ld no tenía el formato esperado: %.70s...\n", contador_lineas_leidas, buffer_linea);
        }
    }

    if (ferror(archivo_docs)) { // ¿Pasó algo mientras leíamos?
        perror("[PARSER] Hubo un error de lectura en el archivo de documentos durante fgets");
    }

    fclose(archivo_docs);
    printf("\n[PARSER] Termino el procesamiento de '%s'!\n", nombre_archivo);
    printf("  Total de lineas leidas: %ld\n", contador_lineas_leidas);
    printf("  Lineas parseadas y procesadas correctamente: %ld\n", lineas_parseadas_ok);
    if (lineas_con_formato_malo > 0) {
        printf("  Lineas con formato 'URL || Contenido' no encontrado (saltadas): %ld\n", lineas_con_formato_malo);
    }
    return true;
}
