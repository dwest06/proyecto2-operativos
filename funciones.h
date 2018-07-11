#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 1024
#define RESERVAR_PALABRA calloc(BUFFER, sizeof(char))

typedef struct informacion {
    int hombres;
    int mujeres;
    int proporcion_h[10];
    int proporcion_m[10];
} informacion;


int sumatoria(int elementos[]);

int es_archivo(char *ruta);
char* obtener_ruta_absoluta(char *ruta_relativa);
int* unir_arreglos(int tam, int arr1[], int arr2[]);
informacion* unir_datos(informacion *info1, informacion *info2);

double promedio(int cantidad[]);

informacion* leer_archivo(char *dir);