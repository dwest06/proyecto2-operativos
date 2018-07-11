#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "funciones.h"
#include "stack.h"

pid_t parent_pid;

void sigquit_handler (int sig) {
	assert (sig==SIGQUIT);
	pid_t self = getpid();
	if (parent_pid != self) _exit(0);
}

int hallar_posicion_proceso(pid_t proceso, pid_t arreglo[], int n) {
	for (int i = i; i < n; i++) {
		if (arreglo[i] == proceso) return i; 
	}
	return -1;
}

int main(int argc, char const *argv[]){

	// Verificaciones de parametros de entrada
	if (argc < 3){
		printf("Cantidad inválida de argumentos.\n\tUso:\t%s<numero de procesos> <archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}




	signal(SIGQUIT, sigquit_handler);
    parent_pid = getpid();
	// Variables necesarias
	// Cantiad de procesos que se deben crear
	int numero_procesos = atoi(argv[1]);
	//Cantidas de archivos leidos 
	int cantidad_archivos = 0;

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
	char *linea_leida = NULL;
	size_t longitud = 0;
	ssize_t leidos;
	char *token = NULL;

	// Arreglo de strings para guardar la cantidad de nombres de archivos
	char* palabra;

	stack* pila_palabras = createStack(BUFFER);

	// Obtenemos del archivo los nombres de los demas archivos y los guardamos 
	// en el arreglo 
	while ((leidos = getline(&linea_leida, &longitud, fdprincipal)) != NULL){
		if (linea_leida[strlen(linea_leida) - 1] == '\n') linea_leida[strlen(linea_leida) - 1] = '\0';
        if (linea_leida[strlen(linea_leida) - 1] == ' ') linea_leida[strlen(linea_leida) - 1] = '\0';

        palabra = calloc(BUFFER, sizeof(char));
        strcpy(palabra, ruta_directorio);
		strcat(palabra, linea_leida);
        strcat(palabra, ".csv");

        push(pila_palabras, palabra);
        cantidad_archivos++;
	}

	//  Creamos los arreglos de files descriptors para la comunicacion entre
	//  Procesos mediante pipes
	//  Files descriptors para la comunicacion padre a hijo
	int fdp[numero_procesos][2];
	// Files descriptors para la comunicacion hijo a padre
	int fdh[numero_procesos][2];

	// Para los forks y reconocer cual es el padre y cual es el hijo
	pid_t f[numero_procesos];
	int i;
	//Creacion de los procesor
	for (i = 0; i < numero_procesos; ++i)	{

		// Pipe para la comunicacion de padre a hijo
		pipe(fdp[i]);

		// Pide para la comunicaion de hijo a padre
		pipe(fdh[i]);

		// Fork para crear a los hijos
		f[i] = fork();

		//Si es el hijo, se detiene el loop para que no se cree mas procesos
		if (f[i] == 0) break;
	
	}



	// Proceso padre
	if (f[i] > 0){
		//Cerramos los pipes necesarios de cada hijo
		for (int i = 0; i < numero_procesos; ++i){
			// Cerramos el read del padre
			close(fdp[i][0]);

			// Cerramos el write del hijo
			close(fdh[i][1]);
		}
			
		char* archivo;
		
		if (cantidad_archivos <= numero_procesos) {
			for (int i = 0; i < cantidad_archivos; ++i){
				// Pasamos la informacion por el pipe
				archivo = pop(pila_palabras);
				write(fdp[i][1], archivo, strlen(archivo + 1));
			}

				// Esperamos a que algun hijo finalice los calculos
				wait(NULL);

			for (int i = 0; i < cantidad_archivos; ++i){
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
			}
		}
		else {
			for (int i = 0; i < numero_procesos; ++i){
				// Pasamos la informacion por el pipe
				archivo = pop(pila_palabras);
				write(fdp[i][1], archivo, strlen(archivo + 1));
				cantidad_archivos--;
			}
			while (cantidad_archivos != -1) {
				pid_t retornado = wait(NULL);

				informacion* a = calloc(1, sizeof(informacion));
				read(fdh[retornado][0], a, sizeof(informacion));

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

				archivo = pop(pila_palabras);
				write(fdp[i][1], archivo, strlen(archivo + 1));
				cantidad_archivos--;
			}

		kill(-parent_pid, SIGQUIT);

		}

		for (int i = 0; i < numero_procesos; ++i){
			//Cerramos los demas files descriptors de los pipes
			close(fdp[i][1]);
			close(fdh[i][0]);
		}
	}

	//Proceso hijo
	else {
		//Cerramos el write del padre
		close(fdp[i][1]);

		//Cerramos el read del hijo
		close(fdh[i][0]);


		//Definimos variables para generar la ruta
		char ruta[120];
		//inicializamos 
		memset(ruta, 0, 120);


		//Leemos el archivo del pipe
		read(fdp[i][0], ruta, 100);

		//Pasa la ruta a a funcion que abre el .csv y analiza los datos
		informacion *datos = leer_archivo(ruta);

		write(fdh[i][1], datos, sizeof(informacion));


		//Cerramos los demas files descriptor de los pipes
		close(fdp[i][0]);
		close(fdh[i][1]);
		exit(0);
	}

	fclose(fdprincipal);

	informacion* infofinal = unir_datos(info1, info2);
	printf("Cantidad total de hombres: %i\n", infofinal->hombres);
	printf("Cantidad total de mujeres: %i\n", infofinal->mujeres);

	printf("Promedio total de edad de hombres: %f\n", promedio(infofinal->proporcion_h));
	printf("Promedio total de edad de mujeres: %f\n", promedio(infofinal->proporcion_m));
	return 0;
}