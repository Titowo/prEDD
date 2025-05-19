main: ./src/main.c
	gcc -Wall -g -o buscador src/main.c src/list.c src/stopwords.c src/inverted_index.c src/parser.c