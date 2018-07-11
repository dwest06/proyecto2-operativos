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

#include "funciones.h"
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

	int Total_Time_Start, Total_Time_End;

	Total_Time_Start = Tomar_Tiempo();
	
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
	//for (int i = 0; i < NumProcesos; ++i){
	while ((readd = getline(&line, &len, fdprincipal)) != NULL){

		if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (line[strlen(line) - 1] == ' ') line[strlen(line) - 1] = '\0';

        strcpy(tokens[i], ruta_directorio);
		strcat(tokens[i], line);
        strcat(tokens[i], ".csv");

        //Contamos cuantos archivos se tiene que analizar
        countArchivos++;
	}

	//  Creamos los arreglos de files descriptors para la comunicacion entre
	//  Procesos mediante pipes
	//  Files descriptors para la comunicacion padre a hijo
	int fdp[2];
	// Files descriptors para la comunicacion hijo a padre
	int fdh[2];

	int final = 0;

	// Para los forks y reconocer cual es el padre y cual es el hijo
	pid_t f[NumProcesos];
	pid_t fid;
	// Pipe para la comunicacion de padre a hijo
	pipe(fdp);
	// Pide para la comunicaion de hijo a padre
	pipe(fdh);

	//Creacion de los procesor
	for (int i = 0; i < NumProcesos; ++i)	{		
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

		for (int i = 0; i < countArchivos; ++i){
			// Pasamos la informacion por el pipe
			write(fdp[1], tokens[i], strlen(tokens[i]) + 1);
		}

		//Cerramos la entrada de escritura
		close(fdp[1]);

		//Se recibe la informacion de cada hijo y se totaliza
		for (int i = 0; i < countArchivos; ++i){
			// Obtenemos los resultados por el pipe

			/////////////////////////////////
			// Recibe la struct aqui, besitos
			/////////////////////////////////
			read(fdh[0], , );
			printf("Recibio en el pipe:%d\n", a);

		}
		

		//Cerramos file descriptor del pipes
		close(fdh[0]);

		for (int i = 0; i < NumProcesos; ++i){
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
			leer_archivo(ruta);


			//////////////////////////////////
			// Aqui envia el struct al padre
			//////////////////////////////////

			write(fdh[1], , );	
		}
		

		//Cerramos los demas files descriptor de los pipes
		close(fdp[0]);
		close(fdh[1]);

		exit(0);
	}


		}

	Total_Time_End = Tomar_Tiempo();
	fclose(fdprincipal);
	printf("Tiempo tomasdo: %d\n",Total_Time_End - Total_Time_Start );
	return 0;
}