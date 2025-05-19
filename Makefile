# --- Compilador y Flags ---
CC = gcc
# Flags para el compilador:
CFLAGS = -Wall -g

# --- Archivos Fuente ---
# Directorio donde están tus archivos .c
SRCDIR = src
# Lista de tus archivos .c
C_SOURCES = main.c list.c stopwords.c inverted_index.c parser.c
SRCS = $(addprefix $(SRCDIR)/, $(C_SOURCES))

# --- Nombre del Ejecutable ---
TARGET_BASE = buscador

# --- Configuración Específica del Sistema Operativo ---
RM = rm -f             # Comando para borrar archivos
CLEAR_SCREEN = clear   # Comando para limpiar la pantalla
TARGET_SUFFIX =

ifeq ($(OS),Windows_NT)
    RM = del /Q /F         # Comando de Windows para borra
    CLEAR_SCREEN = cls     # Comando de Windows para limpiar pantalla
    TARGET_SUFFIX = .exe   # En Windows, los ejecutables llevan .exe
endif

# Nombre final del ejecutable, con su extensión si aplica
TARGET = $(TARGET_BASE)$(TARGET_SUFFIX)

# --- Reglas del Makefile ---

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	@$(CLEAR_SCREEN) # Limpiamos la pantalla antes de mostrar los mensajes de compilacion
	@echo "------------------------------------------------------------"
	@echo "Compilando el proyecto Buscador: $(TARGET)"
	@echo "Fuentes: $(SRCS)"
	@echo "------------------------------------------------------------"
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)
	@echo ""
	@echo "------------------------------------------------------------"
	@echo "Compilacion exitosa!"
	@echo "Ejecutable creado en: ./$(TARGET)"
	@echo "------------------------------------------------------------"
	@echo "Para ejecutar (desde la carpeta del Makefile):"
	@echo "  En Linux/macOS: ./$(TARGET_BASE)"
	@echo "  En Windows    : $(TARGET_BASE).exe  (o .\\$(TARGET_BASE).exe)"
	@echo "------------------------------------------------------------"
	@echo "Recuerda que puedes pasar los paths a los archivos como argumentos:"
	@echo "  ./$(TARGET_BASE) ruta/a/stopwords.dat ruta/a/documentos.dat"
	@echo "Si no pasas argumentos, usara los defaults (dataset pequenio)."
	@echo "------------------------------------------------------------"


.PHONY: clean
clean:
	@echo "------------------------------------------------------------"
	@echo "Limpiando archivos generados del proyecto Buscador..."
	@echo "Eliminando: $(TARGET)"
	$(RM) $(TARGET)
	# Si en el futuro compilaras a archivos objeto (.o) primero,
	# también los borrarías aquí, ej: $(RM) $(SRCDIR)/*.o
	@echo "Limpieza completada."
	@echo "------------------------------------------------------------"

.PHONY: help
help:
	@echo "Makefile para el Proyecto Buscador"
	@echo "---------------------------------"
	@echo "Comandos disponibles:"
	@echo "  make        o make all    : Compila el proyecto."
	@echo "  make clean  : Elimina el ejecutable generado."
	@echo "  make help   : Muestra esta ayuda."
	@echo ""
	@echo "Para compilar con un compilador diferente (ej. clang):"
	@echo "  make CC=clang"
	@echo "------------------------------------------------------------"

