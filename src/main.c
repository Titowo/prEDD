#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>   // Para tolower

// --- Nuestros Módulos ---
// Asumiendo que main.c está en prEDD/src/ y los .h en prEDD/src/includes/
#include "includes/stopwords.h"      // Declaraciones para stopwords
#include "includes/list.h"           // Declaraciones para listas (nodePtr, etc.)
#include "includes/inverted_index.h" // Declaraciones para el índice (indiceInvertido, etc.)
#include "includes/parser.h"         // Declaraciones para el parseo de documentos

#define MAX_LARGO_CONSULTA 256   // Máximo de caracteres para la consulta del usuario.
#define MAX_TERMINOS_CONSULTA 20 // Máximo de palabras "útiles" en una consulta.

// Función auxiliar local para pasar la consulta del usuario a minúsculas.
static void main_convertir_a_minusculas(char *cadena) {
    if (!cadena) return; // Por si las moscas.
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower((unsigned char)cadena[i]); // El cast es buena práctica con ctype.
    }
}

int main(void) { // Usar void si no vas a usar argc, argv. Es más limpio.

    // Nombres de archivos como los espera el PDF.
    // Ojo con las rutas si tu 'data' no está al mismo nivel que el ejecutable.
    // El PDF menciona "stopwords_english.dat". Tu archivo subido es "stopwords_english.dat.txt".
    // Ajusta esta línea si tu archivo se llama diferente.
    const char* archivo_stopwords_path    = "data/stopwords_english.dat.txt"; // Usando el nombre de tu archivo subido
    const char* archivo_documentos_path   = "data/gov2_pages.dat";

    printf("------------------------------------------------------------\n");
    printf("--- Mi Buscador Personalizado - Versión \"Hecho en Casa\" ---\n");
    printf("------------------------------------------------------------\n\n");

    printf("[MAIN] Cargando stopwords desde '%s'. Un poquito de paciencia...\n", archivo_stopwords_path);
    if (!cargar_stopwords(archivo_stopwords_path)) { // Esta es tu función de stopwords.h
        fprintf(stderr, "[MAIN] ¡Falló la carga de stopwords! Así no podemos seguir, el programa termina.\n");
        return EXIT_FAILURE;
    }
    // Tu cargar_stopwords ya imprime un mensaje de cuántas cargó.
    printf("[MAIN] ¡Stopwords listas y dispuestas para ser ignoradas!\n\n");

    printf("[MAIN] Creando el índice invertido...\n");
    // CORREGIDO: Usar 'indiceInvertido' (minúscula i) para que coincida con tu inverted_index.h
    indiceInvertido* mi_indice = crear_indice(2048); // Capacidad inicial para el vocabulario.
    if (!mi_indice) {
        fprintf(stderr, "[MAIN] ¡Falló la creación del índice invertido! Problemas de memoria quizás.\n");
        free_stopwords(); // Usando la función declarada en stopwords.h
        return EXIT_FAILURE;
    }
    printf("[MAIN] Índice invertido listo para recibir datos.\n\n");

    printf("[MAIN] Procesando documentos desde '%s' para llenar el índice...\n", archivo_documentos_path);
    // En tu parser.h el primer param se llama 'nombre_archivo' y el segundo 'index'
    // (En tu parser.c original, el segundo era 'idx', pero el .h manda)
    if (!procesar_archivo_documento(archivo_documentos_path, mi_indice)) {
        fprintf(stderr, "[MAIN] Hubo un problema procesando los documentos. El índice podría estar incompleto.\n");
    } else {
        // En tu inverted_index.h, el miembro es 'cantidad'
        printf("[MAIN] Documentos procesados. El índice tiene %zu palabras únicas.\n\n", mi_indice->cantidad);
    }

    char consulta_del_usuario[MAX_LARGO_CONSULTA];
    printf("\n------------------------------------------\n");
    printf("--- ¡YA PUEDES HACER TUS CONSULTAS! ---\n");
    printf("Escribe lo que buscas (o 'chao' para terminar la conversa):\n");

    while (true) {
        printf("\nTu Consulta > ");
        if (fgets(consulta_del_usuario, sizeof(consulta_del_usuario), stdin) == NULL) {
            printf("\n[MAIN] Fin de la entrada (EOF) o error leyendo. ¡Nos vemos!\n");
            break;
        }

        consulta_del_usuario[strcspn(consulta_del_usuario, "\n")] = '\0'; // Sacar el \n

        if (strcmp(consulta_del_usuario, "chao") == 0) {
            printf("[MAIN] ¡Vale, que te vaya bien!\n");
            break;
        }

        if (strlen(consulta_del_usuario) == 0) { continue; } // Si el usuario solo apretó Enter.

        printf("[MAIN] Procesando: \"%s\"\n", consulta_del_usuario);

        char copia_consulta[MAX_LARGO_CONSULTA]; // strtok modifica, así que trabajamos con una copia.
        strcpy(copia_consulta, consulta_del_usuario);
        main_convertir_a_minusculas(copia_consulta); // Nuestra función local.

        char* terminos_validos[MAX_TERMINOS_CONSULTA];
        int num_terminos_validos = 0;
        const char* delimitadores_consulta = " \t\n\r\f\v,.;:!?()[]{}-\"\'“”‘’"; // Hartos delimitadores.
        char* token = strtok(copia_consulta, delimitadores_consulta);

        while (token != NULL && num_terminos_validos < MAX_TERMINOS_CONSULTA) {
            // Ya está en minúsculas. Solo filtramos stopwords y vacíos.
            if (strlen(token) > 0 && !es_stopword(token)) { // es_stopword de stopwords.h
                terminos_validos[num_terminos_validos++] = token;
            }
            token = strtok(NULL, delimitadores_consulta);
        }

        if (num_terminos_validos == 0) {
            printf("  Mmm, tu consulta no tiene palabras que sirvan después de filtrar. Intenta de nuevo.\n");
            continue;
        }

        printf("  Buscando %d término(s) clave: ", num_terminos_validos);
        for(int i = 0; i < num_terminos_validos; ++i) printf("'%s' ", terminos_validos[i]);
        printf("\n");

        // En tu list.h, el tipo es nodePtr
        nodePtr lista_resultado_final = NULL;
        bool es_primera_lista_valida = true;

        for (int i = 0; i < num_terminos_validos; ++i) {
            // En tu inverted_index.h el segundo param es 'palabra'.
            nodePtr lista_del_termino_actual = buscar_lista_posteo_termino(mi_indice, terminos_validos[i]);

            if (!lista_del_termino_actual) {
                printf("  El término '%s' no lo tenemos registrado.\n", terminos_validos[i]);
                free_list(&lista_resultado_final); // Usando free_list de tu list.h
                lista_resultado_final = NULL;
                break; // No tiene caso seguir con esta consulta.
            }

            if (es_primera_lista_valida) {
                nodePtr nodo_aux_copia = lista_del_termino_actual;
                while(nodo_aux_copia) {
                    // En tu list.h el primer param de insertar_o_sumar_node es 'head' y el segundo 'documento'.
                    insertar_o_sumar_node(&lista_resultado_final, nodo_aux_copia->documento);
                    nodo_aux_copia = nodo_aux_copia->next;
                }
                es_primera_lista_valida = false;
                if (!lista_resultado_final && num_terminos_validos > 0) { // Si la copia falló o la lista original era vacía
                     printf("  Problemas al armar la lista inicial con '%s'.\n", terminos_validos[i]);
                     break; // No podemos seguir.
                }
            } else { // Si ya tenemos una lista parcial de resultados, hacemos la intersección.
                // En tu inverted_index.h los params de intersectar_listas_posteo son lista1, lista2
                nodePtr lista_intermedia = intersectar_listas_posteo(lista_resultado_final, lista_del_termino_actual);
                free_list(&lista_resultado_final); // Liberamos la lista parcial anterior.
                lista_resultado_final = lista_intermedia;   // La nueva intersección es nuestro resultado parcial.

                if (lista_resultado_final == NULL) {
                    // Si la intersección nos dejó sin nada, ya no hay match.
                    printf("  Parece que '%s' no tiene documentos en común con los términos anteriores.\n", terminos_validos[i]);
                    break;
                }
            }
        }

        if (lista_resultado_final != NULL) {
            printf("--- ¡Resultados! Documentos que contienen todos los términos que buscaste: ---\n");
            // En tu list.h el param de print_list es 'head'.
            // La definición en tu list.c es 'const nodePtr head', lo cual es compatible.
            print_list(lista_resultado_final); // Usando print_list de tu list.h
            free_list(&lista_resultado_final); // ¡Importante! Liberar la memoria de esta lista.
        } else {
            printf("Pucha, no encontramos documentos que tengan todos esos términos juntos.\n");
        }
    }

    printf("\n[MAIN] Limpiando y liberando toda la memoria...\n");
    if (mi_indice) {
        // En tu inverted_index.h el param de destruir_indice es 'indice'.
        destruir_indice(mi_indice);
        printf("[MAIN] Índice invertido liberado.\n");
    }
    free_stopwords(); // Usando la función de stopwords.h (ya imprime su mensaje)

    printf("\n[MAIN] ¡Misión cumplida! Programa terminado. Que estés bien.\n");
    return EXIT_SUCCESS;
}
