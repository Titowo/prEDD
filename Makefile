main: ./src/main.c
	cls
	gcc -Wall -g -o buscador src/main.c src/list.c src/stopwords.c src/inverted_index.c src/parser.c
	@echo "Archivo './buscador' a sido creado con exito"
