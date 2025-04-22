#ifndef parser_H_
#define parser_H_
#include "inverted_index.h"

bool procesar_archivo_documento(const char* filename, indiceInvertido* index);
bool parser_line(char* line, char** url_out, char** contenido_out);
void tokenizacion_y_index_contenido(const char* contenido, const char *documento, indiceInvertido* index);

#endif // !parser_H_