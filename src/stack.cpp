#include<malloc.h>

#include "stack.h"

#ifndef MAX_STACK_SIZE
#define MAX_STACK_SIZE  100
#endif

stack *creatstack(){
	stack *s;
	s = (stack *)malloc(sizeof(stack));
	s->data = (int *)calloc(MAX_STACK_SIZE, sizeof(int));
	s->top = -1;
	return s;
}
void push(stack *s, int data){
	s->top++;
	s->data[s->top] = data;
}
int top(stack *s){
	return s->top;
}
int pop(stack *s){
	if (top(s) == -1) return -1;
	int k = s->data[s->top];
	s->top--;
	return k;
}

int empty(stack *s){
	if (s->top == -1) return 1;
	return 0;
}