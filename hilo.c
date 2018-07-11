#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "funciones.h"

//Variables Globales

long cantHombres = 0, cantMujeres = 0, promedioHombre = 0, promedioMujeres= 0;

int counter = 0;
int NumHilos;
//Cantidas de archivos leidos 
int countArchivos = 0;

char tokens[20][100];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#include "funciones.h"

/***************************************************************************
 * Definicion de la funcion para tomar los tiempos en Solaris o Linux.
 * Retorna el tiempo en microsegundos
 ***************************************************************************/
int Tomar_Tiempo()
{
    struct timeval t;         /* usado para tomar los tiempos */
    int dt;
    gettimeofday ( &t, (struct timezone*)0 );
    dt = (t.tv_sec)*1000000 + t.tv_usec;
    return dt;
}

//Funcion que realiza un hilo
void *leer_archivo(void *arg){
	char *dir;
	while (counter < countArchivos){
		
		pthread_mutex_lock(&mutex);
		//Buscamos el archivo
		dir = tokens[counter];
		counter++;
		printf("%s\n", dir);
		pthread_mutex_unlock(&mutex);
		/////////////////////
		//Analizamos lo que este en dir
		/////////////////////

		//printf("%lu, counter: %d, dir: %s\n",pthread_self(), counter, dir );

		
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
    //char *ruta_directorio = obtener_ruta_absoluta(nombre_archivo_contenido);
    char *ruta_directorio = "5departamentos/";

  
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
		err = pthread_create( &(threads[i%NumHilos]) , NULL , (void*)leer_archivo, NULL );
	}

	for (int i = 0; i < NumHilos; ++i){
        pthread_join(threads[i], NULL);
	}

	Total_Time_End = Tomar_Tiempo();

	printf("Tiempo tomasdo: %d\n",Total_Time_End - Total_Time_Start );

	return 0;
}