#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

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
void *leer_archivo(){

}


int main(int argc, char const *argv[]){
	
	// Verificaciones de parametros de entrada
	if (argc < 3){
		printf("Cantidad inválida de argumentos.\n\tUso:\t%s<numero de procesos> <archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Variables necesarias
	// Cantiad de procesos que se deben crear
	int NumHilos = atoi(argv[1]);
	//Cantidas de archivos leidos 
	int countArchivos = 0;

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

    // Guardamos un arreglo con la cantidad de 
	char *line = NULL;
	size_t len = 0;
	ssize_t readd;
	char *token = NULL;

	// Arreglo de strings para guardar la cantidad de nombres de archivos
	char tokens[NumHilos][100];

	// Obtenemos del archivo los nombres de los demas archivos y los guardamos 
	// en el arreglo 
	while ((readd = getline(&line, &len, fdprincipal)) != NULL){

		if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (line[strlen(line) - 1] == ' ') line[strlen(line) - 1] = '\0';

        strcpy(tokens[i], ruta_directorio);
		strcat(tokens[i], line);
        strcat(tokens[i], ".csv");

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
		err = pthread_create( &(threads[i]) , NULL , &leer_archivo, NULL );

	}

	return 0;
}