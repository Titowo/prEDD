#ifndef parser_H_
#define parser_H_

#include "inverted_index.h"
#include <stdbool.h>       

// --- Prototipos de Funciones para el Parseo de Documentos ---

/**
 * @brief Procesa un archivo completo de documentos para construir/llenar el índice invertido.
 * Lee el archivo línea por línea, donde cada línea representa un documento.
 * Para cada línea, extrae la URL (ID del documento) y el contenido de texto.
 * Luego, tokeniza el contenido y añade los términos válidos (no stopwords) al índice.
 * Se espera que el archivo tenga el formato especificado: URL || Contenido
 * @param filename El nombre/ruta del archivo de documentos a procesar
 * @param index Puntero al índice invertido que se llenará con los términos y documentos.
 * @return bool Devuelve true si el archivo se procesó completamente sin errores fatales 
 * de lo contrario devuelve false.
 */
bool procesar_archivo_documento(const char* nombre_archivo, indiceInvertido* index);

/**
 * @brief Parsea una única línea del archivo de documentos para separar la URL del contenido.
 * Busca el último separador "||" en la línea para distinguir la URL del contenido.
 * Asigna memoria dinámicamente para almacenar la URL y el contenido como cadenas separadas.
 * @param line La línea de texto completa leída del archivo.
 * @param url_out Puntero a un char* donde se almacenará el puntero a la cadena de la URL asignada.
 * @param contenido_out Puntero a un char* donde se almacenará el puntero a la cadena del contenido asignada.
 * @return bool Devuelve true si el parseo fue exitoso y se asignó memoria, false si la línea
 * no tiene el formato esperado o si falla la asignación de memoria.
 */
bool parsear_linea(char* linea_original, char** url_salida, char** contenido_salida);

/**
 * @brief Tokeniza el contenido textual de un documento y añade los términos válidos al índice.
 * Recorre la cadena 'contenido', la divide en palabras (tokens) usando espacios y/o
 * signos de puntuación como delimitadores.
 * Para cada token (palabra): lo convierte a minúsculas, verifica si es una stopword (implícito,
 * la función anadir_termino debería manejarlo o se verifica aquí antes de llamar) y, si es
 * un término válido, lo añade al índice asociado al 'documento' dado usando la función
 * anadir_termino del módulo inverted_index.
 * @param contenido La cadena de texto con el contenido del documento.
 * @param documento El identificador (URL) del documento al que pertenece el contenido.
 * @param index Puntero al índice invertido donde se añadirán los términos.
 */
// Nombre cambiado
void tokenizar_e_indexar_contenido(const char* contenido_const, const char* documento_id, indiceInvertido* indice);

#endif // parser_H_