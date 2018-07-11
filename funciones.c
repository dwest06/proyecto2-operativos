#include "funciones.h"

int sumatoria(int elementos[]) {
    int res = 0;

    for (int i = 0; i < 10; i++)  {
        res += elementos[i];
    } 

    return res;
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

    if (ruta[strlen(ruta)] != '/') strcat(ruta, "/");
    free(ruta_corregida);

    return ruta; 
}

double calcular_promedio() {
    return 1.0;
}