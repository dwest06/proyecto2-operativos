#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>


int leer_archivo(char *dir ){
	//Abrimos el archivo

	printf("%s\n", dir);
	FILE * fd = fopen(dir, "r");

	if (fd == NULL){
		perror("Error en leer_archivo");
		exit(EXIT_FAILURE);
	}

	//Declaramos l variables para leer el archivo
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *token = NULL;

	//La primera linea la descartamos ya que no se necesita
	read = getline(&line, &len, fd);

	while ((read = getline(&line, &len, fd)) != -1) {				
		if (read == 1) break;
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';
        //printf("%s\n", line);
        //El primer token de la linea tampoco se necesita
        token = strtok(line, ",");
        printf("%s\n",token );

        while ((token = strtok(NULL,",")) != NULL){
        	printf("%s\n",token );
        }
    }

	//lo Cerramos
	fclose(fd);
	return 0;
}


int main(int argc, char const *argv[]){
	
	//Verificaciones de parametros de entrada

	if (argc < 2){
		printf("Error.\nUso: %s <NumProcesos> <ArchivoLista>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	//Variables necesarias

	//Cantiad de procesos que se deben crear
	int NumProcesos = atoi(argv[1]);
	//Leemos el archivo que contiene el nombre de los .csv que se deben analizar
	FILE* fdprincipal = fopen(argv[2], "r");

	//Verificamos si fue exitoso el open del archivo
	if (fdprincipal == NULL){
		perror("Archivo no encontrado");
		exit(EXIT_FAILURE);
	}

	//Guardamos un arreglo con la cantidad de 
	char *line = NULL;
	size_t len = 0;
	ssize_t readd;
	char *token = NULL;
	char tokens[NumProcesos][100];
	for (int i = 0; i < NumProcesos; ++i){
		readd = getline(&line, &len, fdprincipal);

		if (line[readd - 1] == '\n')
			line[readd - 1] = '\0';

		printf("%s\n",line );
		strcpy(tokens[i],line);
	}

	//Creamos los arreglos de files descriptors para la comunicacion entre
	//Procesos mediante pipes
	//Files descriptors para la comunicacion padre a hijo
	int fdp[NumProcesos][2];
	//Files descriptors para la comunicacion hijo a padre
	int fdh[NumProcesos][2];

	int final = 0;

	pid_t f;

	for (int i = 0; i < NumProcesos; ++i)	{

		//Pipe para la comunicacion de padre a hijo
		pipe(fdp[i]);

		//Pide para la comunicaion de hijo a padre
		pipe(fdh[i]);

		//Fork para crear a los hijos
		f = fork();
		//Proceso padre
		if (f > 0){
			printf("Soy el padre\n");

			//Cerramos el read del padre
			close(fdp[i][0]);

			//Cerramos el write del hijo
			close(fdh[i][1]);

			//Pasamos la informacion por el 
			write(fdp[i][1], tokens[i], strlen(tokens[i]) + 1);

			wait(NULL);

			int a;
			read(fdh[i][0], &a, sizeof(int));
			printf("Recibio en el pipe:%d\n", a);
			final += a;

			//Recibe todos la info de los hijos
			printf("SALIIIIIIIIII\n" );

			//Cerramos los demas files descriptors de los pipes
			close(fdp[i][1]);
			close(fdh[i][0]);


		}
		//Proceso hijo
		else{
			printf("Soy el hijo\n");

			//Cerramos el write del padre
			close(fdp[i][1]);

			//Cerramos el read del hijo
			close(fdh[i][0]);

			//Definimos variables para generar la ruta
			char nom[100];
			char ruta[120];
			//inicializamos 
			memset(ruta, 0, 120);

			//Leemos el archivo del pipe
			read(fdp[i][0], nom, 100 );

			if(nom[strlen(nom) - 1] == ' ')
				nom[strlen(nom) - 1] = '\0';

			//concatenamos el .csv para que pueda abrir el archivo correctamente
			strcat(nom, ".csv");
			strcat(ruta, "6d/");
			strcat(ruta, nom);

			printf("Esto es lo que se paso por el pipe:%s\n",ruta );

			//Pasa la ruta a a funcion que abre el .csv y analiza los datos

			leer_archivo(ruta);
			//write(fdh[i][1], &aux, sizeof(aux) );
			
			//Abre el archivo que se paso por el pipe

			

			//Cerramos los demas files descriptor de los pipes
			close(fdp[i][0]);
			close(fdh[i][1]);

			exit(0);


		}


    //Cerramos el archivo que contiene la lista de los nombres
	
	}
	fclose(fdprincipal);
	printf("Y el final fue: %d\n", final);
	return 0;
}