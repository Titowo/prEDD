#ifndef stopword_H_
#define stopword_H_

bool cargar_stopwords(const char* filename);
bool is_stopword(const char* word);
void free_stopwords();

#endif // !stopword_H_