#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "funciones.h"

double sumatoria(int n, int elementos[]) {
	double res = 0;

	for (int i = 0; i < n; i++)  {
		res += elementos[i];
	} 

	return res;
}

double promedio(int n, int cantidad[]) {
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

	for (int i = 0; i < n; i++) {
		prom += ((rangos[i][0] + rangos[i][1])/2)*cantidad[i];
	}

	prom /= sumatoria(n, cantidad);

	return prom;

}

/*
 *	Funcion que se le pasa una direccion de un archivo .csv, lee el contenido
 *	y calcula el promedio de hombre y promedio de mujeres
 *
 */
double* leer_archivo(char *dir){
	double* resultado = calloc(4, sizeof(double));

	// Declaramos ciertas variables utiles para contar hombres y mujeres
	int count_h = 0;
	int count_m = 0;

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
	int cant_pueblos = 0;
	while ((read = getline(&line, &len, fd)) != -1) {
		// O leemos hasta que la linea que sea solo contenga un salto de linea
		if (read == 1) break;

		cant_pueblos += 1;

		// Quitamos el salto de linea al final
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';
		printf("%s\n", line);

		// La primera linea de cada segmento de informacion tampoco se necesita
		// A partir de la segunda linea de cada segmento es que esta la info
		// necesaria para calcular el promedio
		// Esta linea para los hombres
		read = getline(&line, &len, fd);

        // El primer token de la linea tampoco se necesita
        token = strtok(line, ",");

        // El segundo tampoco
        token = strtok(NULL, ",");

        int hombres[10];
        int x = 0;
        // Leemos todos los datos 
        while ((token = strtok(NULL,",")) != NULL){
        	if (token[0] == ' ') memmove(token, token+1, strlen(token));
        	hombres[x] = atoi(token);
        	x += 1; 
        }

        printf("Promedio de hombres: %f\n", promedio(x, hombres));
       	
        // Para las mujeres
        read = getline(&line, &len, fd);

        // El primer token de la linea tampoco se necesita
        token = strtok(line, ",");
        // El segundo tampoco
        token = strtok(NULL, ",");

        int mujeres[10];
       	int y = 0;
        // Leemos todos los datos 
        while ((token = strtok(NULL,",")) != NULL){
        	if (token[0] == ' ') memmove(token, token+1, strlen(token));
        	mujeres[y] = atoi(token);
        	y += 1; 
        }

        resultado[0] += sumatoria(x, hombres);
        resultado[1] += sumatoria(y, mujeres);
        resultado[2] += promedio(x, hombres);
        resultado[3] += promedio(y, mujeres);
    }

    resultado[2] /= cant_pueblos;
    resultado[3] /= cant_pueblos;

	// lo Cerramos
	fclose(fd);
	for (int i = 0; i < 4; i++) {
		printf("%f\n", resultado[i]);
	}
	return resultado;
}

int main(int argc, char const *argv[]){
	// Verificaciones de parametros de entrada
	if (argc < 3){
		printf("Cantidad inválida de argumentos.\n\tUso:\t%s<numero de procesos> <archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// Variables necesarias
	// Cantiad de procesos que se deben crear
	int NumProcesos = atoi(argv[1]);

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

			double* a;
			read(fdh[i][0], a, sizeof(double*));
			printf("Recibio en el pipe:%f\n", a[0]);
			//final += a;

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
			double *datos = leer_archivo(ruta);

			//Pasamos por pipe al proceso padre el resultado de los archivos
			//Analizados

			//////////////////////////////////
			// Tambien falta esto, no se que parametros enviar
			//////////////////////////////////

			write(fdh[i][1], datos, sizeof(datos));

			//Cerramos los demas files descriptor de los pipes
			close(fdp[i][0]);
			close(fdh[i][1]);

			exit(0);
		}

    //Cerramos el archivo que contiene la lista de los nombre
	}
	fclose(fdprincipal);
	printf("Y el final fue: %d\n", final);
	return 0;
}