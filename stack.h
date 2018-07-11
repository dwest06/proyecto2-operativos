// C program for array implementation of stack
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


// A structure to represent a stack
typedef struct stack
{
    int top;
    unsigned capacity;
    char** array;
} stack;

stack* createStack(unsigned capacity);
int isFull(stack* stack);
int isEmpty(stack* stack);
void push(stack* stack, char* item);
char* pop(stack* stack);
