#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "funciones.h"

int main(int argc, char const *argv[]){
	// Verificaciones de parametros de entrada
	if (argc < 3){
		printf("Cantidad inválida de argumentos.\n\tUso:\t%s<numero de procesos> <archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// Variables necesarias
	// Cantiad de procesos que se deben crear
	int NumProcesos = atoi(argv[1]);

	informacion* info1 = NULL;
	informacion* info2 = NULL;

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
	char tokens[NumProcesos][100];

	// Obtenemos del archivo los nombres de los demas archivos y los guardamos 
	// en el arreglo 
	for (int i = 0; i < NumProcesos; ++i){
		readd = getline(&line, &len, fdprincipal);

		if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (line[strlen(line) - 1] == ' ') line[strlen(line) - 1] = '\0';

        strcpy(tokens[i], ruta_directorio);
		strcat(tokens[i], line);
        strcat(tokens[i], ".csv");
	}

	//  Creamos los arreglos de files descriptors para la comunicacion entre
	//  Procesos mediante pipes
	//  Files descriptors para la comunicacion padre a hijo
	int fdp[NumProcesos][2];
	// Files descriptors para la comunicacion hijo a padre
	int fdh[NumProcesos][2];

	int final = 0;

	// Para los forks y reconocer cual es el padre y cual es el hijo
	pid_t f;

	for (int i = 0; i < NumProcesos; ++i) {

		// Pipe para la comunicacion de padre a hijo
		pipe(fdp[i]);

		// Pide para la comunicaion de hijo a padre
		pipe(fdh[i]);

		// Fork para crear a los hijos
		f = fork();
		// Proceso padre
		if (f > 0) {
			// Cerramos el read del padre
			close(fdp[i][0]);

			// Cerramos el write del hijo
			close(fdh[i][1]);

			// Pasamos la informacion por el 
			write(fdp[i][1], tokens[i], strlen(tokens[i]) + 1);

			// Esperamos a que el hijo finalice los calculos
			wait(NULL);

			// Obtenemos los resultados por el pipe

			/////////////////////////////////
			// No se todavia que se recibe por el pipe
			/////////////////////////////////

			informacion* a = calloc(1, sizeof(informacion));
			read(fdh[i][0], a, sizeof(informacion));

			if (info1 == NULL) {
				info1 = a;
			}
			else if (info2 == NULL) {
				info2 = a;
			}
			else {
				info1 = unir_datos(info1, info2);
				info2 = a;
			}

			//Recibe todos la info de los hijos

			//Cerramos los demas files descriptors de los pipes
			close(fdp[i][1]);
			close(fdh[i][0]);
		}

		//Proceso hijo
		else{
			//Cerramos el write del padre
			close(fdp[i][1]);

			//Cerramos el read del hijo
			close(fdh[i][0]);

			//Definimos variables para generar la ruta
			char ruta[120];
			//inicializamos 
			memset(ruta, 0, 120);

			//Leemos el archivo del pipe
			read(fdp[i][0], ruta, 100 );

			//Pasa la ruta a a funcion que abre el .csv y analiza los datos
			informacion *datos = leer_archivo(ruta);

			//Pasamos por pipe al proceso padre el resultado de los archivos
			//Analizados

			write(fdh[i][1], datos, sizeof(informacion));

			//Cerramos los demas files descriptor de los pipes
			close(fdp[i][0]);
			close(fdh[i][1]);

			exit(0);
		}

    //Cerramos el archivo que contiene la lista de los nombre
	}
	fclose(fdprincipal);

	informacion* infofinal = unir_datos(info1, info2);
	printf("Cantidad total de hombres: %i\n", infofinal->hombres);
	printf("Cantidad total de mujeres: %i\n", infofinal->mujeres);

	printf("Promedio total de edad de hombres: %f\n", promedio(infofinal->proporcion_h));
	printf("Promedio total de edad de mujeres: %f\n", promedio(infofinal->proporcion_m));
	return 0;
}