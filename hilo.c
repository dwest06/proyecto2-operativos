#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "funciones.h"

//Variables Globales

int cantHombres = 0, cantMujeres = 0;
int proporcion_h[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int proporcion_m[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int counter = 0;
int NumHilos;
//Cantidas de archivos leidos 
int countArchivos = 0;

char tokens[20][100];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Funcion que realiza un hilo
void *leer_archivo_hilos(void *arg){
	char *dir;
	while (counter < countArchivos){
		
		pthread_mutex_lock(&mutex);
		//Buscamos el archivo
		dir = tokens[counter];
		counter++;
		pthread_mutex_unlock(&mutex);

		// Verificamos si el se abrio correctamente
		FILE * fd = fopen(dir, "r");
	    if (fd == NULL){
	        perror("Error en leer_archivo");
	        exit(EXIT_FAILURE);
	    }

	    char *lineh = NULL;
	    size_t lenh = 0;
	    ssize_t readh;
	    char *tokenh = NULL;

	    int hombres[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	    int cant_hombres;
	    int mujeres[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	    int cant_mujeres;

	    readh = getline(&lineh, &lenh, fd);

	    while ((readh = getline(&lineh, &lenh, fd)) != -1) {
	        // O leemos hasta que la linea que sea solo contenga un salto de linea
	        if (readh == 1) break;

	        // Quitamos el salto de linea al final
	        if (lineh[readh - 1] == '\n')
	            lineh[readh - 1] = '\0';

	        // La primera linea de cada segmento de informacion tampoco se necesita
	        // A partir de la segunda linea de cada segmento es que esta la info
	        // necesaria para calcular el promedio
	        // Esta linea para los hombres
	        readh = getline(&lineh, &lenh, fd);

	        // El primer token de la linea tampoco se necesita
	        tokenh = strtok(lineh, ",");

	        // El segundo tampoco
	        tokenh = strtok(NULL, ",");

	        cant_hombres = 0;
	        // Leemos todos los datos 
	        while ((tokenh = strtok(NULL,",")) != NULL){
	            if (tokenh[0] == ' ') memmove(tokenh, tokenh+1, strlen(tokenh));
	            hombres[cant_hombres] += atoi(tokenh);
	            cant_hombres += 1; 
	        }
	        
	        // Para las mujeres
	        readh = getline(&lineh, &lenh, fd);

	        // El primer tokenh de la linea tampoco se necesita
	        tokenh = strtok(lineh, ",");
	        // El segundo tampoco
	        tokenh = strtok(NULL, ",");

	        cant_mujeres = 0;
	        // Leemos todos los datos 
	        while ((tokenh = strtok(NULL,",")) != NULL){
	            if (tokenh[0] == ' ') memmove(tokenh, tokenh+1, strlen(tokenh));
	            mujeres[cant_mujeres] += atoi(tokenh);
	            cant_mujeres += 1; 
	        }
	    }
		cantHombres += sumatoria(hombres);
		cantMujeres += sumatoria(mujeres);

		for (int i = 0; i < 10; i++) {
	        proporcion_h[i] = hombres[i];
	        proporcion_m[i] = mujeres[i];
	    }
	}

	pthread_exit(NULL);
}


int main(int argc, char const *argv[]){
	
	int Total_Time_Start, Total_Time_End;

	Total_Time_Start = Tomar_Tiempo();
	// Verificaciones de parametros de entrada
	if (argc < 3){
		printf("Cantidad inválida de argumentos.\n\tUso:\t%s<numero de procesos> <archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Variables necesarias
	// Cantiad de procesos que se deben crear
	NumHilos = atoi(argv[1]);
	// Leemos el archivo que contiene el nombre de los .csv que se deben analizar
	FILE* fdprincipal = fopen(argv[2], "r");

	// Verificamos si fue exitoso el open del archivo
	if (fdprincipal == NULL){
		perror("Archivo ingresado no encontrado");
		exit(EXIT_FAILURE);
	}
	char *nombre_archivo_contenido = calloc(BUFFER, sizeof(char));
    strcpy(nombre_archivo_contenido, argv[2]);
    char *ruta_directorio = obtener_ruta_absoluta(nombre_archivo_contenido);
    //char *ruta_directorio = "5departamentos/";

  
    // Guardamos un arreglo con la cantidad de 
	char *line = NULL;
	size_t len = 0;
	ssize_t readd;
	char *token = NULL;

	// Arreglo de strings para guardar la cantidad de nombres de archivos
	

	// Obtenemos del archivo los nombres de los demas archivos y los guardamos 
	// en el arreglo 
	while ((readd = getline(&line, &len, fdprincipal)) != -1){


		if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (line[strlen(line) - 1] == ' ') line[strlen(line) - 1] = '\0';

      
        strcpy(tokens[countArchivos], ruta_directorio);
		strcat(tokens[countArchivos], line);
        strcat(tokens[countArchivos], ".csv");
      
      
        //Contamos cuantos archivos se tiene que analizar
        countArchivos++;
	}

	//////////////////////////
	//Creamos los hilos
	//////////////////////////

	//declaracion de los hilos
	pthread_t threads[NumHilos];

	int err;

	for (int i = 0; i < NumHilos; ++i){
		err = pthread_create( &(threads[i%NumHilos]) , NULL , (void*)leer_archivo_hilos, NULL );
	}

	for (int i = 0; i < NumHilos; ++i){
        pthread_join(threads[i], NULL);
	}

	Total_Time_End = Tomar_Tiempo();
	printf("Cantidad total de hombres: %i\n", cantHombres);
	printf("Cantidad total de mujeres: %i\n", cantMujeres);

	printf("Promedio total de edad de hombres: %f\n", promedio(proporcion_h));
	printf("Promedio total de edad de mujeres: %f\n", promedio(proporcion_m));

	printf("Tiempo: %dms\n",Total_Time_End - Total_Time_Start );

	return 0;
}