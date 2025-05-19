#include "./includes/parser.h"
#include "./includes/stopwords.h"
#include "./includes/inverted_index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static void convertir_a_minuscula(char *cadena) {
    if(!cadena) return; // Si pasa NULL no hacer nada;
    for (int i=0; cadena[i]; i++) {
        cadena[i] = tolower((unsigned char)cadena[i]);
    }

}


bool parsaer_linea(char* linea_original, char** url_salida, char** contenido_salida) {
    if(!linea_original || !url_salida || !contenido_salida) {
        fprintf(stderr, "[*PARSER*] Argumentos nulos en parsear_linea.\n");
        return false;
    }

    *url_salida = NULL;
    *contenido_salida = NULL;

    char* separador = strrstr(linea_original, "||");
    
    if (separador == NULL) {
        fprintf(stderr, "[*PARSER*] Linea sin separador '||': %s\n", linea_original);
        return false;
    }

    // Calcular el largo de la url
    size_t largo_url = separador - linea_original;

    *url_salida = (char*)malloc(largo_url + 1); // sin el +1 no funciona LOL
    if (!*url_salida) {
        perror("[*PARSER*] Fallo malloc para url_salida.\n");
        return false;
    }
    strncpy(*url_salida, linea_original, largo_url);
    (*url_salida)[largo_url] = '\0';

    // El contenido de la url comienza desde "||" que son 2 caracteres
    char* inicio_contenido = separador + 2;
    // Quitar los espacios entre el separador y el comienzo del string
    while(*inicio_contenido && isspace((unsigned char)*inicio_contenido)) {
        inicio_contenido++;
    }

    *contenido_salida = strdup(inicio_contenido);
    if (!*contenido_salida) {
        perror("[*PARSER*] Fallo strdup para contenido_out\n");
        free(*url_salida);
        *url_salida = NULL;
        return false;
    }

    return true;
} 


void tokenizar_e_indexar_contenido(const char* contenido_const, const char* docuemnto_id, indiceInvertido* indice) {
    if (!contenido_const || !docuemnto_id || !indice) {
        return;
    }

    char* contenido_mutable = strdup(contenido_const);
    if (!contenido_mutable) {
        perror("[*PARSER*] Fallo el strdup para el contenido mutable a tokenizar\n");
        return;
    }

    const char* delimitador = " \t\n\r\f\v,.;:!?()[]{}-\"\'“”‘’";
    char* token = strtok(contenido_mutable, delimitador);
    
    while (token != NULL) {
        convertir_a_minuscula(token);
        
        if (strlen(token) > 0 && !es_stopword(token)) {
            anadir_termino(indice, token, docuemnto_id);
        }
        token = strtok(NULL, delimitador);
    }
    free(contenido_mutable);
}

bool procesar_archivo_documento(const char* nombre_archivo, indiceInvertido* indice) {
    if (!nombre_archivo || !indice) {
        fprintf(stderr, "[*PARSER*] Nombre del archivo o indice nulos para procesar_archivo_documento.\n");
        return false;
    }

    FILE *archivo = fopen(nombre_archivo, "r");
    if(!archivo) {
        fprintf(stderr, "[*PARSER*] No se pudo abrir el archivo de documentos '%s': '%s'\n", nombre_archivo, strerror(errno));
        return false;
    }

    printf("[*PARSER*] Procesando archivo de documentos: '%s'\n", nombre_archivo);

    char buffer_linea[4096];
    long contador_lineas = 0;
    long lineas_procesadas_ok = 0;

    while(fgets(buffer_linea, sizeof(buffer_linea), archivo) != NULL) {
        contador_lineas++;
        if (contador_lineas % 5000 == 0) {
            printf("Procesadno linea %ld...\n", contador_lineas);
        }

        char *url= NULL;
        char *contenido= NULL;

        buffer_linea[strcspn(buffer_linea, "\n")] = '\0';

        if (parsear_linea(buffer_linea, &url, &contenido)) {
            tokenizar_e_indexar_contenido(contenido, url, indice);

            free(url);
            free(contenido);
            lineas_procesadas_ok++;
        } else {
            fprintf(stderr, "[*PARSER*] Advertencia: Linea %ld no pudo ser parseada: %s\n", contador_lineas, buffer_linea);
        }
    }

    if (ferror(archivo)) {
        perror("[*PARSER*] Ocurrio un error en la lectura del archivo...\n");
    }

    fclose(archivo);
    printf("[*PARSER*] Fin del procesamiento de documentos. %ld lineas leidas, %ld procesadas correctamente.\n", contador_lineas, lineas_procesadas_ok);
    return true;
}