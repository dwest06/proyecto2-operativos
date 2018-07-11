#include "stack.h"
  
// function to create a stack of given capacity. It initializes size of
// stack as 0
stack* createStack(unsigned capacity)
{
    stack* stack = malloc(sizeof(stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (char**) malloc(stack->capacity * sizeof(char*));
    return stack;
}
 
// Stack is full when top is equal to the last index
int isFull(stack* stack)
{   return stack->top == stack->capacity - 1; }
 
// Stack is empty when top is equal to -1
int isEmpty(stack* stack)
{   return stack->top == -1;  }
 
// Function to add an item to stack.  It increases top by 1
void push(stack* stack, char* item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}
 
// Function to remove an item from stack.  It decreases top by 1
char* pop(stack* stack)
{
    if (isEmpty(stack))
        return NULL;
    return stack->array[stack->top--];
}