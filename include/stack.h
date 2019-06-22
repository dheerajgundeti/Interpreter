#ifndef _STACK_H
#define _STACK_H

struct stac{
	int *data;
	int top;
};
typedef stac stack;

stack *creatstack();
void push(stack *s, int data);
int top(stack *s);
int pop(stack *s);
int empty(stack *s);

#endif