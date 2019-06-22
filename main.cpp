#include<stdio.h>
#include<malloc.h>
#include<string.h>

#include "helpers.h"
#include "process.h"



int main(){

	int ok=compilefile("input.txt","output.txt");
	if (ok == 1){
		printf("\nCOMPILED!!");
	}
	else{
		printf("ERROR : NOT COMPILED");
	}
	return 0;
}