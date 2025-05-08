#include "stopwords.c"
// #include "list.c"
// #include "inverted_index.h"

int main(int argc, char const *argv[])
{
    char* filename = "./data/stopwords_english.dat.txt";
    cargar_stopwords(filename);
    free_stopwords();
    return 0;
}
