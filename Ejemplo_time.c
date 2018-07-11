/*****************************************************************************
 * ARCHIVO : Ejemplo_Tiempo.c    
 *
 * CONTENIDO : implementacion de la funcion para el calculo de la combinatoria
 *                         de dos numeros con toma de tiempos en Solaris o Linux
 *
 * AUTOR :    Yudith Cardinale
 *
 * LIBRERIAS UTILIZADAS: stdio.h, stdlib.h sys/time.h math.h
 *
 *****************************************************************************/

#include <stdlib.h> 
#include <math.h>
#include <stdio.h>
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

/*****************************************************************************
 * FUNCION : factorial.
 *
 * DESCRIPCION : dado un numero entero calcula su factorial recursivamete.
 *
 * PARAMETROS : (->)y entero,
 *                            (<-)entero.
 *
 * VARIABLES GLOBALES : ninguna.
 *
 *****************************************************************************/


int factorial (int y)
{
    int i,x;

    x=1;
    for (i=1;i<=y;i++){
        x=x* i;                            
    }
    return x;
}            

/*****************************************************************************
 * FUNCION : combinatoria.
 *
 * DESCRIPCION : dado dos numeros enteros calcula la combinatoria del primero
 *                             en el segundo.
 *
 * PARAMETROS : (->)m entero,n entero
 *                            (<-)entero.
 *
 * VARIABLES GLOBALES : ninguna.
 *
 *****************************************************************************/
int comb (int m, int n)
{
    int x;

    x=factorial (m)/(factorial (n)*factorial (m-n));
    return x;
}
                                                             

/************************************************************************/
/*                                Ejecucion del procedimiento principal                                 */
/************************************************************************/
/*******************************************************************
 *
 *    FUNCION: implementacion del programa principal para el calculo
 *                     de un numero combinatorio.
 *
 *    MACROS O FUNCIONES UTILIZADAS :    Tomar_Tiempo, factorial, comb.
 *
 *    DESCRIPCION: toma 3 argumentos, y calcula la combinatoria de
 *                             los dos ultimos. Ejemplo de corrida: "Ejemplo 5 4" 
 *
 *******************************************************************/

void main (int argc, char *argv[])
{
    int m,n,x;
    int Total_Time_Start, Total_Time_End, Comb_Time_Start, Comb_Time_End;
    
    if (argc!=3) {
        printf("Usage: %s numero_1 numero_2\n",argv[0]);
        exit(1);
    }
    
    m=atoi (argv[1]);
    n=atoi (argv[2]);
    
    Total_Time_Start=Tomar_Tiempo(); /* comienza a tomar tiempo total de ejecuci'on */         
    
    if ( (m>=0) && (m>=n) && (n>=0) ) {

        Comb_Time_Start= Tomar_Tiempo(); /* toma el tiempo de inicio de la funci'on comb */
        x=comb (m,n);     
        Comb_Time_End = Tomar_Tiempo();     /* toma el tiempo de finalizacion de la funci'on comb */        

        printf ("La combinatoria de %d en %d es: %d\n",m,n,x);
        printf("Tiempo de la funcion comb:    %8d\n", Comb_Time_End - Comb_Time_Start);
 
        Total_Time_End = Tomar_Tiempo();/*toma el tiempo de finalizaci'on de la ejecucion completa
*/
        printf("Tiempo total: %8d\n", Total_Time_End - Total_Time_Start);
    } 
    else {
        printf ("Opcion invalida!!!\n");
    }
}