#include "funciones.h"

int sumatoria(int elementos[]) {
    int res = 0;

    for (int i = 0; i < 10; i++)  {
        res += elementos[i];
    } 

    return res;
}

double promedio(int cantidad[]) {
    int rangos[10][2] = {
        {0, 9}, 
        {10, 19},
        {20, 29},
        {30, 39},
        {40, 49},
        {50, 59},
        {60, 69},
        {70, 79},
        {80, 89},
        {90, 99}
    };

    double prom = 0;

    for (int i = 0; i < 10; i++) {
        prom += ((rangos[i][0] + rangos[i][1])/2)*cantidad[i];
    }

    prom /= sumatoria(cantidad);

    return prom;
}

informacion* unir_datos(informacion *info1, informacion *info2) {
    informacion *resultado = calloc(1, sizeof(informacion));

    if (info1 == NULL) return NULL;
    if (info2 == NULL) return info1;

    resultado->hombres = info1->hombres + info2->hombres;
    resultado->mujeres = info1->mujeres + info2->mujeres;

    for (int i = 0; i < 10; i++) {
        resultado->proporcion_h[i] = info1->proporcion_h[i] + info2->proporcion_h[i];
        resultado->proporcion_m[i] = info1->proporcion_m[i] + info2->proporcion_m[i];
    }

    return resultado;   
}

int es_archivo(char *ruta) {
    struct stat path_stat;
    stat(ruta, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char* obtener_ruta_absoluta(char *ruta_relativa) {
    if (ruta_relativa[0] == '/' || ruta_relativa[0] == '~') return ruta_relativa;

    char cwd[1024];
    char *ruta_corregida, *ruta;
    char *tmp;
    tmp = RESERVAR_PALABRA;
    ruta_corregida = RESERVAR_PALABRA;
    ruta = RESERVAR_PALABRA;
    getcwd(cwd, sizeof(cwd));
    strcpy(ruta, cwd);
    strcat(ruta, "/");
    strcpy(ruta_corregida, ruta_relativa);
    
    if (es_archivo(ruta_corregida)) {
        strcpy(tmp, ruta_corregida);
        char *token1 = strtok(tmp, "/");
        char* token2 = strtok(NULL, "/");

        strcpy(ruta_corregida, "");
        while (token2 != NULL) {
            strcat(ruta_corregida, token1);
            strcat(ruta_corregida, "/");
            strcpy(token1, token2);
            token2 = strtok(NULL, "/");
        }
    }

    strcat(ruta, ruta_corregida);

    if (ruta[strlen(ruta)-1] != '/') strcat(ruta, "/");
    free(ruta_corregida);

    return ruta; 
}

double calcular_promedio() {
    return 1.0;
}

/*
 *  Funcion que se le pasa una direccion de un archivo .csv, lee el contenido
 *  y calcula el promedio de hombre y promedio de mujeres
 *
 */
informacion* leer_archivo(char *dir) {
    informacion* resultado = calloc(1, sizeof(informacion));

    // Abrimos el archivo
    FILE * fd = fopen(dir, "r");

    // Verificamos si el se abrio correctamente
    if (fd == NULL){
        perror("Error en leer_archivo");
        exit(EXIT_FAILURE);
    }

    // Declaramos l variables para leer el archivo
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *token = NULL;

    // La primera linea la descartamos ya que no se necesita
    read = getline(&line, &len, fd);

    // Leemos por pueblo hasta que ya no puedo leer mas
    int hombres[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int cant_hombres;
    int mujeres[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int cant_mujeres;
    int posicion;

    while ((read = getline(&line, &len, fd)) != -1) {
        // O leemos hasta que la linea que sea solo contenga un salto de linea
        if (read == 1) break;

        // Quitamos el salto de linea al final
        if (line[read - 1] == '\n')
            line[read - 1] = '\0';

        // La primera linea de cada segmento de informacion tampoco se necesita
        // A partir de la segunda linea de cada segmento es que esta la info
        // necesaria para calcular el promedio
        // Esta linea para los hombres
        read = getline(&line, &len, fd);

        // El primer token de la linea tampoco se necesita
        token = strtok(line, ",");

        // El segundo tampoco
        token = strtok(NULL, ",");

        cant_hombres = 0;
        // Leemos todos los datos 
        while ((token = strtok(NULL,",")) != NULL){
            if (token[0] == ' ') memmove(token, token+1, strlen(token));
            hombres[cant_hombres] += atoi(token);
            cant_hombres += 1; 
        }
        
        // Para las mujeres
        read = getline(&line, &len, fd);

        // El primer token de la linea tampoco se necesita
        token = strtok(line, ",");
        // El segundo tampoco
        token = strtok(NULL, ",");

        cant_mujeres = 0;
        // Leemos todos los datos 
        while ((token = strtok(NULL,",")) != NULL){
            if (token[0] == ' ') memmove(token, token+1, strlen(token));
            mujeres[cant_mujeres] += atoi(token);
            cant_mujeres += 1; 
        }
    }

    resultado->hombres += sumatoria(hombres);
    resultado->mujeres += sumatoria(mujeres);

    for (int i = 0; i < 10; i++) {
        resultado->proporcion_h[i] = hombres[i];
        resultado->proporcion_m[i] = mujeres[i];
    }

    // lo Cerramos
    fclose(fd);
    return resultado;
}

int Tomar_Tiempo()
{
  struct timeval t;     /* usado para tomar los tiempos */
  int dt;
  gettimeofday ( &t, (struct timezone*)0 );
  dt = (t.tv_sec)*1000000 + t.tv_usec;
  return dt;
}