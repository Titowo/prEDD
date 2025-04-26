#ifndef stopword_H_
#define stopword_H_

#include <stdbool.h> // Necesario para bool

// --- Prototipos de Funciones para el Manejo de Stop Words ---

/**
 * @brief Carga la lista de stop words desde un archivo de texto.
 * Lee el archivo especificado (ej. "stopwords_english.dat" [source: 45]), donde se espera
 * que cada línea contenga una única stop word. Almacena las stop words
 * internamente en memoria (probablemente en minúsculas para comparación
 * insensible al caso).
 * @param filename El nombre/ruta del archivo que contiene las stop words.
 * @return bool Devuelve true si la carga fue exitosa (archivo leído y memoria asignada),
 * false si ocurre algún error (ej. no se pudo abrir el archivo, error de memoria).
 */
bool cargar_stopwords(const char* filename);

/**
 * @brief Verifica si una palabra dada es una stop word.
 * Compara la palabra (probablemente convirtiéndola a minúsculas internamente)
 * con la lista de stop words cargada previamente en memoria.
 * @param word La palabra a verificar.
 * @return bool Devuelve true si la palabra se encuentra en la lista de stop words cargada,
 * false en caso contrario.
 */
bool is_stopword(const char* word);

/**
 * @brief Libera toda la memoria utilizada para almacenar la lista de stop words.
 * Debe llamarse al final del programa para evitar fugas de memoria.
 */
void free_stopwords();

#endif // stopword_H_ // Corregido el cierre del #ifndef