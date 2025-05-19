#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// --- Nuestros Módulos ---
// Asegúrate que estas rutas sean correctas según tu estructura.
#include "includes/stopwords.h"
#include "includes/list.h"
#include "includes/inverted_index.h"
#include "includes/parser.h"

// --- Archivos de Datos para Pruebas ---
const char* TEST_STOPWORDS_FILE = "test_stopwords.dat";
const char* TEST_DOCS_FILE = "test_docs.dat";

// --- Funciones Auxiliares para las Pruebas ---

void imprimir_titulo_test(const char* titulo) {
    printf("\n--- INICIO TEST: %s ---\n", titulo);
}

void imprimir_fin_test(const char* titulo) {
    printf("--- FIN TEST: %s ---\n", titulo);
}

void crear_archivo_test_stopwords() {
    FILE* f = fopen(TEST_STOPWORDS_FILE, "w");
    if (f) {
        fprintf(f, "de\n");
        fprintf(f, "la\n");
        fprintf(f, "el\n");
        fprintf(f, "y\n");
        fprintf(f, "a\n");
        fclose(f);
        printf("  [TEST_INFO] Archivo '%s' creado para pruebas.\n", TEST_STOPWORDS_FILE);
    } else {
        perror("  [TEST_ERROR] No se pudo crear el archivo de stopwords para test");
    }
}

void crear_archivo_test_documentos() {
    FILE* f = fopen(TEST_DOCS_FILE, "w");
    if (f) {
        fprintf(f, "doc1.com||este es el contenido del primer documento de prueba\n");
        fprintf(f, "doc2.org||la casa es bonita y el perro tambien\n");
        fprintf(f, "doc3.net||otro documento mas para probar el sistema de indexado\n");
        fprintf(f, "doc4.io||prueba con stopword el la de y\n");
        fprintf(f, "sin_separador_esta_linea\n"); // Línea con error de formato
        fprintf(f, "doc5.ai||\n"); // Contenido vacío
        fclose(f);
        printf("  [TEST_INFO] Archivo '%s' creado para pruebas.\n", TEST_DOCS_FILE);
    } else {
        perror("  [TEST_ERROR] No se pudo crear el archivo de documentos para test");
    }
}

// --- Tests para el Módulo STOPWORDS ---
void test_modulo_stopwords() {
    imprimir_titulo_test("Modulo Stopwords");
    crear_archivo_test_stopwords();

    printf("  Intentando cargar stopwords desde '%s'...\n", TEST_STOPWORDS_FILE);
    if (cargar_stopwords(TEST_STOPWORDS_FILE)) {
        printf("  Stopwords cargadas OK.\n");

        printf("  Probando es_stopword('de'): %s\n", es_stopword("de") ? "true (CORRECTO)" : "false (ERROR)");
        printf("  Probando es_stopword('La'): %s\n", es_stopword("La") ? "true (CORRECTO)" : "false (ERROR)"); // Asume que es_stopword maneja minúsculas
        printf("  Probando es_stopword('casa'): %s\n", !es_stopword("casa") ? "false (CORRECTO)" : "true (ERROR)");
        printf("  Probando es_stopword('COMPUTADOR'): %s\n", !es_stopword("COMPUTADOR") ? "false (CORRECTO)" : "true (ERROR)");
        printf("  Probando es_stopword(NULL): %s\n", !es_stopword(NULL) ? "false (CORRECTO)" : "true (ERROR)");
        printf("  Probando es_stopword(\"\"): %s\n", !es_stopword("") ? "false (CORRECTO)" : "true (ERROR)");

        printf("  Liberando stopwords...\n");
        free_stopwords();
        printf("  Stopwords liberadas.\n");
        // Probamos si es_stopword devuelve false después de liberar (debería, ya que la lista estaría vacía)
        printf("  Probando es_stopword('de') despues de liberar: %s\n", !es_stopword("de") ? "false (CORRECTO)" : "true (ERROR)");

    } else {
        fprintf(stderr, "  ERROR: cargar_stopwords falló.\n");
    }
    remove(TEST_STOPWORDS_FILE); // Limpiamos el archivo de prueba
    imprimir_fin_test("Modulo Stopwords");
}

// --- Tests para el Módulo LIST ---
void test_modulo_list() {
    imprimir_titulo_test("Modulo List (Listas Enlazadas)");
    nodePtr cabeza = NULL;

    printf("  Insertando 'doc1.txt'...\n");
    insertar_o_sumar_node(&cabeza, "doc1.txt"); // Frecuencia 1
    printf("  Insertando 'doc2.pdf'...\n");
    insertar_o_sumar_node(&cabeza, "doc2.pdf"); // Frecuencia 1
    printf("  Insertando 'doc1.txt' de nuevo...\n");
    insertar_o_sumar_node(&cabeza, "doc1.txt"); // Frecuencia 2 para doc1.txt
    printf("  Insertando 'doc3.html'...\n");
    insertar_o_sumar_node(&cabeza, "doc3.html"); // Frecuencia 1

    printf("  Contenido de la lista despues de inserciones:\n");
    print_list(cabeza);

    // Test crear_nodo individualmente (aunque insertar_o_sumar_node ya lo usa)
    printf("  Creando nodo individual 'doc_test_individual.txt'...\n");
    nodePtr nodo_suelto = crear_nodo("doc_test_individual.txt");
    if (nodo_suelto) {
        printf("    Nodo suelto creado: %s, Freq: %d\n", nodo_suelto->documento, nodo_suelto->frecuencia);
        // Liberamos este nodo suelto (su documento y el nodo)
        free(nodo_suelto->documento);
        free(nodo_suelto);
        printf("    Nodo suelto liberado.\n");
    } else {
        fprintf(stderr, "  ERROR: crear_nodo fallo para 'doc_test_individual.txt'.\n");
    }


    printf("  Liberando la lista completa...\n");
    free_list(&cabeza);
    if (cabeza == NULL) {
        printf("  Lista liberada correctamente (cabeza es NULL).\n");
    } else {
        fprintf(stderr, "  ERROR: La cabeza de la lista no es NULL después de free_list.\n");
    }
    // Imprimimos de nuevo para verificar que esté vacía (no debería imprimir nada)
    printf("  Contenido de la lista despues de liberar (deberia estar vacia):\n");
    print_list(cabeza);

    imprimir_fin_test("Modulo List (Listas Enlazadas)");
}

// --- Tests para el Módulo INVERTED_INDEX ---
void test_modulo_inverted_index() {
    imprimir_titulo_test("Modulo Inverted Index");
    indiceInvertido* idx = crear_indice(4); // Capacidad inicial pequeña para probar crecimiento.
    if (!idx) {
        fprintf(stderr, "  ERROR: crear_indice fallo. No se pueden continuar los tests del indice.\n");
        return;
    }
    printf("  Indice creado con capacidad inicial: %zu\n", idx->capacidad);

    printf("  Añadiendo términos...\n");
    anadir_termino(idx, "hola", "doc1");
    anadir_termino(idx, "mundo", "doc1");
    anadir_termino(idx, "hola", "doc2");
    anadir_termino(idx, "test", "doc1");
    anadir_termino(idx, "prueba", "doc3");
    anadir_termino(idx, "hola", "doc1"); // Debería incrementar frecuencia en ("hola", "doc1")

    printf("  Indice despues de añadir terminos (Cantidad: %zu, Capacidad: %zu):\n", idx->cantidad, idx->capacidad);
    // Para ver el contenido, buscamos algunos términos
    nodePtr lista_hola = buscar_lista_posteo_termino(idx, "hola");
    printf("    Documentos para 'hola':\n    ");
    print_list(lista_hola); // No liberar lista_hola, es parte del índice.

    nodePtr lista_mundo = buscar_lista_posteo_termino(idx, "mundo");
    printf("    Documentos para 'mundo':\n    ");
    print_list(lista_mundo);

    nodePtr lista_inexistente = buscar_lista_posteo_termino(idx, "chao");
    printf("    Documentos para 'chao' (deberia ser NULL o lista vacia):\n    ");
    if (lista_inexistente == NULL) printf("NULL (CORRECTO)\n"); else print_list(lista_inexistente);


    // Test de intersección
    printf("  Probando interseccion de 'hola' y 'test' (ambos en doc1)...\n");
    // buscar_lista_posteo_termino devuelve punteros a listas internas, no debemos liberarlas.
    // intersectar_listas_posteo devuelve una NUEVA lista que SÍ debemos liberar.
    nodePtr interseccion1 = intersectar_listas_posteo(lista_hola, lista_mundo); // hola (d1,d2), mundo (d1) -> d1
    printf("    Intersección ('hola' y 'mundo'):\n    ");
    print_list(interseccion1);
    free_list(&interseccion1); // Liberamos la lista resultado de la intersección

    printf("  Probando intersección de 'hola' y 'prueba' (ninguno en común)...\n");
    nodePtr lista_prueba = buscar_lista_posteo_termino(idx, "prueba"); // prueba (d3)
    nodePtr interseccion2 = intersectar_listas_posteo(lista_hola, lista_prueba); // hola (d1,d2), prueba (d3) -> NULL
    printf("    Intersección ('hola' y 'prueba'):\n    ");
    if (interseccion2 == NULL) printf("NULL (CORRECTO)\n"); else print_list(interseccion2);
    free_list(&interseccion2);


    printf("  Destruyendo el índice...\n");
    destruir_indice(idx);
    printf("  Índice destruido.\n");

    imprimir_fin_test("Modulo Inverted Index");
}

// --- Tests para el Módulo PARSER ---
void test_modulo_parser() {
    imprimir_titulo_test("Modulo Parser");

    // Test para parsear_linea
    printf("  Testeando parsear_linea...\n");
    char linea1_test[] = "http://ejemplo.com/pagina1||Este es el contenido de la pagina uno";
    char* url1 = NULL, *contenido1 = NULL;
    if (parsear_linea(linea1_test, &url1, &contenido1)) {
        printf("    Parseo OK: URL='%s', Contenido='%.30s...'\n", url1, contenido1);
        free(url1); free(contenido1);
    } else { fprintf(stderr, "    ERROR parseando linea 1 valida.\n"); }

    char linea2_test[] = "http://ejemplo.com/pagina2||"; // Contenido vacío
    char* url2 = NULL, *contenido2 = NULL;
    if (parsear_linea(linea2_test, &url2, &contenido2)) {
        printf("    Parseo OK (contenido vacio): URL='%s', Contenido='%s'\n", url2, contenido2);
        free(url2); free(contenido2);
    } else { fprintf(stderr, "    ERROR parseando linea 2 con contenido vacio.\n"); }

    char linea3_test[] = "solo_url_sin_separador";
    char* url3 = NULL, *contenido3 = NULL;
    if (!parsear_linea(linea3_test, &url3, &contenido3)) {
        printf("    Parseo Fallo (sin separador): CORRECTO, no se pudo parsear.\n");
    } else { fprintf(stderr, "    ERROR: parsear_linea parseo incorrectamente una linea sin separador.\n"); free(url3); free(contenido3); }

    // Test para tokenizar_e_indexar_contenido y procesar_archivo_documento (juntos)
    printf("\n  Testeando procesar_archivo_documento (que usa tokenizar_e_indexar_contenido)...\n");
    crear_archivo_test_stopwords(); // Necesitamos stopwords cargadas
    cargar_stopwords(TEST_STOPWORDS_FILE);

    crear_archivo_test_documentos(); // Creamos un archivo de docs pequeño

    indiceInvertido* idx_parser = crear_indice(10);
    if (!idx_parser) {
        fprintf(stderr, "    ERROR: No se pudo crear indice para test de parser.\n");
        free_stopwords();
        remove(TEST_STOPWORDS_FILE);
        remove(TEST_DOCS_FILE);
        return;
    }

    if (procesar_archivo_documento(TEST_DOCS_FILE, idx_parser)) {
        printf("    procesar_archivo_documento finalizado.\n");
        printf("    Índice despues de procesar (Cantidad: %zu, Capacidad: %zu):\n", idx_parser->cantidad, idx_parser->capacidad);

        nodePtr lista_contenido = buscar_lista_posteo_termino(idx_parser, "contenido");
        printf("      Docs para 'contenido': "); print_list(lista_contenido);

        nodePtr lista_casa = buscar_lista_posteo_termino(idx_parser, "casa");
        printf("      Docs para 'casa': "); print_list(lista_casa);

        nodePtr lista_perro = buscar_lista_posteo_termino(idx_parser, "perro");
        printf("      Docs para 'perro': "); print_list(lista_perro);
        
        nodePtr lista_indexado = buscar_lista_posteo_termino(idx_parser, "indexado");
        printf("      Docs para 'indexado': "); print_list(lista_indexado);

        nodePtr lista_prueba = buscar_lista_posteo_termino(idx_parser, "prueba");
        printf("      Docs para 'prueba': "); print_list(lista_prueba);

    } else {
        fprintf(stderr, "    ERROR: procesar_archivo_documento fallo.\n");
    }

    destruir_indice(idx_parser);
    free_stopwords();
    remove(TEST_STOPWORDS_FILE);
    remove(TEST_DOCS_FILE);

    imprimir_fin_test("Modulo Parser");
}


// --- Main para las Pruebas ---
int main(void) {
    printf("=============================================\n");
    printf("====== INICIO DE PRUEBAS INDIVIDUALES ======\n");
    printf("=============================================\n");

    test_modulo_stopwords();
    test_modulo_list();
    test_modulo_inverted_index();
    test_modulo_parser();

    printf("\n=============================================\n");
    printf("====== FIN DE TODAS LAS PRUEBAS       ======\n");
    printf("=============================================\n");

    return EXIT_SUCCESS;
}

