#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 1024
#define RESERVAR_PALABRA calloc(BUFFER, sizeof(char))

int es_archivo(char *ruta);
char* obtener_ruta_absoluta(char *ruta_relativa);