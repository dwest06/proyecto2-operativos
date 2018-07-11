# Sistemas de Operación 1 (CI-3825)
# Andrés Ignacio Torres (14-11082)
# David Rodríguez (14-10930)

# Variables referentes a ejecutables
COMPILADOR = gcc -c -g
LINKEADOR = gcc

# Nombre del directorio para los objetos
DIR_LIBS = libs

# Regla para crear el directorio de los objetos
$(DIR_LIBS):
	mkdir $(DIR_LIBS)

# Nombre de las librerías/objetos que se requieren
LIBRERIAS = $(DIR_LIBS)/funciones.o $(DIR_LIBS)/main.o

# Nombre del ejecutable final
PROGRAMA = main

# Algunos atajos que permitimos hacer con make
.PHONY: all clean default
.DEFAULT_GOAL := main

# Compila todo el programa
all: $(PROGRAMA)
default: main

# Elimina todo lo compilado en ejecuciones anteriores
clean:
	rm $(PROGRAMA)
	rm $(LIBRERIAS)
	rmdir $(DIR_LIBS)

# Por último, reglas de creación de las librerías
$(PROGRAMA): $(LIBRERIAS)
	$(LINKEADOR) $(LIBRERIAS) -o $(PROGRAMA)

$(DIR_LIBS)/main.o: main.c $(DIR_LIBS) funciones.h
	$(COMPILADOR) main.c -o $(DIR_LIBS)/main.o

$(DIR_LIBS)/funciones.o: funciones.c $(DIR_LIBS) 
	$(COMPILADOR) funciones.c -o $(DIR_LIBS)/funciones.o
