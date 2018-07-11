#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "funciones.h"
#include <math.h>
#include <sys/time.h>
#include <assert.h>
#include "stack.h"

pid_t parent_pid;

void sigquit_handler (int sig) {
	assert (sig==SIGQUIT);
	pid_t self = getpid();
	if (parent_pid != self) _exit(0);
}

int contar_lineas(FILE* descriptor) {
	int lineas = 0;
	int ch;

	while(!feof(descriptor))
	{
	  ch = fgetc(descriptor);
	  if(ch == '\n')
	  {
	    lineas++;
	  }
	}
	return lineas;
}

int hallar_posicion_proceso(pid_t proceso, pid_t arreglo[], int n) {
	for (int i = i; i < n; i++) {
		if (arreglo[i] == proceso) return i; 
	}
	return -1;
}

int main(int argc, char const *argv[]){

	int tiempo_i = Tomar_Tiempo();

	// Verificaciones de parametros de entrada
	if (argc < 3){
		printf("Cantidad inválida de argumentos.\n\tUso:\t%s<numero de procesos> <archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

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

	// Arreglo de strings para guardar la cantidad de nombres de archivos
	char* palabra;
	stack* pila_palabras = createStack(BUFFER);

	FILE* fd2 = fopen(argv[2], "r");
	int cant_lineas = contar_lineas(fd2);
	fclose(fd2);

	// Obtenemos del archivo los nombres de los demas archivos y los guardamos 
	// en el arreglo 
	//for (int i = 0; i < numero_procesos; ++i){
	while ((getline(&linea_leida, &longitud, fdprincipal)) != -1){

		if (linea_leida[strlen(linea_leida) - 1] == '\n') linea_leida[strlen(linea_leida) - 1] = '\0';
        if (linea_leida[strlen(linea_leida) - 1] == ' ') linea_leida[strlen(linea_leida) - 1] = '\0';

        palabra = calloc(BUFFER, sizeof(char));
        strcpy(palabra, ruta_directorio);
		strcat(palabra, linea_leida);
        strcat(palabra, ".csv");

        push(pila_palabras, palabra);

        //Contamos cuantos archivos se tiene que analizar
        cantidad_archivos++;
	}

	//  Creamos los arreglos de files descriptors para la comunicacion entre
	//  Procesos mediante pipes
	//  Files descriptors para la comunicacion padre a hijo
	int fdp[2];
	// Files descriptors para la comunicacion hijo a padre
	int fdh[2];

	int final = 0;

	// Para los forks y reconocer cual es el padre y cual es el hijo
	pid_t f[numero_procesos];
	pid_t fid;
	// Pipe para la comunicacion de padre a hijo
	pipe(fdp);
	// Pide para la comunicaion de hijo a padre
	pipe(fdh);

	//Creacion de los procesor
	for (int i = 0; i < numero_procesos; ++i)	{		
		// Fork para crear a los hijos
		fid = fork();
		f[i] = fid;

		//Si es el hijo, se detiene el loop para que no se cree mas procesos
		if (f[i] == 0) break;
	
	}

	// Proceso padre
	if (fid > 0){

		// Cerramos el read del padre
		close(fdp[0]);

		// Cerramos el write del hijo
		close(fdh[1]);

		for (int i = 0; i < cantidad_archivos; ++i){
			// Pasamos la informacion por el pipe
			palabra = pop(pila_palabras);
			write(fdp[1], palabra, strlen(palabra) + 1);
		}

		//Cerramos la entrada de escritura
		close(fdp[1]);

		//Se recibe la informacion de cada hijo y se totaliza
		for (int i = 0; i < cantidad_archivos; ++i){
			// Obtenemos los resultados por el pipe

			informacion* a;
			read(fdh[0], a, sizeof(a));
			printf("Recibio en el pipe:%d\n", a->hombres);

		}
		

		//Cerramos file descriptor del pipes
		close(fdh[0]);

		for (int i = 0; i < numero_procesos; ++i){
			kill(f[i], SIGKILL);
		}


	}

	//Proceso hijo
	else {
		//Cerramos el write del padre
		close(fdp[1]);

		//Cerramos el read del hijo
		close(fdh[0]);

		//Definimos variables para generar la ruta
		char ruta[BUFFER];
		//inicializamos 
		memset(ruta, 0, BUFFER);

		while(1){
			//Leemos el archivo del pipe
			read(fdp[0], ruta, 100 );
			
			//Pasa la ruta a a funcion que abre el .csv y analiza los datos
			informacion* resultado = leer_archivo(ruta);
			write(fdh[1], resultado, sizeof(informacion));	
		}
		

		//Cerramos los demas files descriptor de los pipes
		close(fdp[0]);
		close(fdh[1]);

		exit(0);
	}

	int tiempo_f = Tomar_Tiempo();
	printf("Tiempo:\t%ims\n", tiempo_f - tiempo_i);
	return 0;
}