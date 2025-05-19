#include "includes/parser.h"         // Tus declaraciones
#include "includes/stopwords.h"      // Para es_stopword
#include "includes/inverted_index.h" // Para anadir_termino e IndiceInvertido
                                     // inverted_index.h debería incluir list.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>   // Para tolower, isspace
#include <errno.h>   // Para strerror

// --- Funciones Estáticas (Ayudantes Internos de este Módulo) ---

/**
 * @brief Pasa una cadena de caracteres a minúsculas. La modifica directamente.
 * Es 'static' porque solo la necesitamos aquí adentro.
 * @param cadena La cadena que vamos a "achicar".
 */
static void parser_convertir_a_minusculas(char *cadena) {
    if (!cadena) return;
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower((unsigned char)cadena[i]);
    }
}

// --- Implementación de Funciones Públicas (Declaradas en parser.h) ---

bool parsear_linea(char* linea_original, char** url_salida, char** contenido_salida) {
    if (!linea_original || !url_salida || !contenido_salida) {
        fprintf(stderr, "[PARSER] ¡Ojo! Argumentos nulos en parsear_linea.\n");
        return false;
    }

    *url_salida = NULL;
    *contenido_salida = NULL;

    char* ultimo_separador = NULL;
    char* puntero_busqueda = linea_original;
    char* ocurrencia_actual;

    // Buscamos la última ocurrencia de "||" de forma manual.
    while ((ocurrencia_actual = strstr(puntero_busqueda, "||")) != NULL) {
        ultimo_separador = ocurrencia_actual;
        puntero_busqueda = ocurrencia_actual + 2; 
    }

    if (ultimo_separador == NULL) {
        return false;
    }

    // Calculamos el largo de la URL (hasta el inicio del último separador "||")
    size_t largo_url = ultimo_separador - linea_original;

    *url_salida = (char*)malloc(largo_url + 1); // +1 para el '\0'
    if (!*url_salida) {
        perror("[PARSER] Falló malloc para url_salida");
        return false;
    }
    strncpy(*url_salida, linea_original, largo_url);
    (*url_salida)[largo_url] = '\0';

    // El contenido empieza justo después del último separador "||" (que tiene 2 caracteres)
    char* inicio_contenido = ultimo_separador + 2;
    // Vamos a saltarnos los espacios iniciales del contenido, si los hay.
    while (*inicio_contenido && isspace((unsigned char)*inicio_contenido)) {
        inicio_contenido++;
    }

    *contenido_salida = strdup(inicio_contenido);
    if (!*contenido_salida) {
        perror("[PARSER] Falló strdup para contenido_salida");
        free(*url_salida);
        *url_salida = NULL;
        return false;
    }
    return true;
}

void tokenizar_e_indexar_contenido(const char* contenido_const, const char* documento_id, indiceInvertido* indice) {
    if (!contenido_const || !documento_id || !indice) {
        return;
    }

    char* contenido_mutable = strdup(contenido_const);
    if (!contenido_mutable) {
        perror("[PARSER] Falló strdup para el contenido mutable en tokenizar_e_indexar_contenido");
        return;
    }

   const char* delimitadores = " \t\n\r\f\v,.;:!?()[]{}-\"\'“”‘’";

    char* token = strtok(contenido_mutable, delimitadores);
    while (token != NULL) {
        parser_convertir_a_minusculas(token);

        // Si la palabra tiene contenido y no es una de las que ignoramos (stopword)...
        if (strlen(token) > 0 && !es_stopword(token)) {
            anadir_termino(indice, token, documento_id);
        }
        token = strtok(NULL, delimitadores);
    }

    free(contenido_mutable);
}

bool procesar_archivo_documento(const char* nombre_archivo, indiceInvertido* index) {
    if (!nombre_archivo || !index) {
        fprintf(stderr, "[PARSER] Error: Nombre de archivo o índice nulos en procesar_archivo_documento.\n");
        return false;
    }

    FILE* archivo_docs = fopen(nombre_archivo, "r");
    if (!archivo_docs) {
        fprintf(stderr, "[PARSER] ¡No se pudo abrir el archivo de documentos '%s'! Error: %s\n", nombre_archivo, strerror(errno));
        return false;
    }

    printf("[PARSER] Procesando los documentos de '%s'. Esto puede tomar un rato, ármate de paciencia...\n", nombre_archivo);

    // Un buffer grandecito, porque las líneas en gov2_pages.dat pueden ser bien largas.
    char buffer_linea[8192];
    long contador_lineas_leidas = 0;
    long lineas_parseadas_ok = 0;
    long lineas_con_error_parseo = 0;


    while (fgets(buffer_linea, sizeof(buffer_linea), archivo_docs) != NULL) {
        contador_lineas_leidas++;
        if (contador_lineas_leidas % 50000 == 0) {
            printf("[PARSER] ... vamos por la línea %ld ... (%ld parseadas OK)\n", contador_lineas_leidas, lineas_parseadas_ok);
        }

        buffer_linea[strcspn(buffer_linea, "\n")] = '\0';

        char* url = NULL;
        char* contenido = NULL;

        if (parsear_linea(buffer_linea, &url, &contenido)) {

            tokenizar_e_indexar_contenido(contenido, url, index);
            free(url);
            free(contenido);
            lineas_parseadas_ok++;
        } else {
            lineas_con_error_parseo++;
        }
    }

    if (ferror(archivo_docs)) {
        perror("[PARSER] Hubo un error de lectura en el archivo de documentos durante el fgets");
    }

    fclose(archivo_docs);
    printf("[PARSER] ¡Terminó el procesamiento de '%s'!\n", nombre_archivo);
    printf("  Se leyeron %ld líneas en total.\n", contador_lineas_leidas);
    printf("  Se parsearon y procesaron %ld líneas correctamente.\n", lineas_parseadas_ok);
    if (lineas_con_error_parseo > 0) {
        printf("  No se pudieron parsear (formato URL||Contenido) %ld líneas.\n", lineas_con_error_parseo);
    }
    return true;
}
