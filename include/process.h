#ifndef _PROCESS_H
#define _PROCESS_H

struct data{
	int size;
	int lines;
};
typedef struct data symboltable_data;

int compilefile(char *input_file, char *output_file);

#endif