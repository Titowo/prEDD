main: ./src/main.c
	gcc -Wall -g -o buscador src/main.c src/list.c src/stopwords.c src/inverted_index.c src/parser.c

	cls
	./buscador data/stopwords_english.dat.txt data/gov2_pages.dat
