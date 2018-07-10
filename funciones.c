#include "funciones.h"

int es_archivo(char *ruta) {
    struct stat path_stat;
    stat(ruta, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char* obtener_ruta_absoluta(char *ruta_relativa) {
    if (ruta_relativa[0] == '/') return ruta_relativa;

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
            strcpy(token1, token2);
            token2 = strtok(NULL, "/");
        }
    }

    strcat(ruta, ruta_corregida);
    free(ruta_corregida);

    return ruta; 
}