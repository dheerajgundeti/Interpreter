#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>

#include "helpers.h"
#include "process.h"
#include "stack.h"

#ifndef MAX_LINE_COUNT
#define MAX_LINE_COUNT 100
#endif


symboltable_data databloc_size(char file[]){
	symboltable_data dat;
	dat.lines = 0;
	dat.size = 0;
	FILE *fp;
	char * line;
	line = (char *)calloc(MAX_LINE_COUNT, sizeof(char));
	fp = fopen(file, "r");
	if (fp == NULL){
		printf("CANNOT OPEN FILE\n");
		return dat;
	}
	int ok = 0,lines=0;
	while (fgets(line,MAX_LINE_COUNT, fp) != NULL){
		if (strcmp(line, "START:\n") == 0) break;
		ok = ok + strlen(line);
		lines++;
	}
	fclose(fp);
	dat.lines = lines;
	dat.size = ok;
	return dat;
}

int get_opcode_symbols(char *s,symboldata *symboltable,symboltable_data dat){
	int i;
	for (i = 0; i < dat.lines; i++){
		if (strcmp(symboltable[i].name, s) == 0){
			return symboltable[i].address;
		}
	}
	return -1;
}

int get_opcode_labels(char *s, labeldata *labeltable, int *label_index){
	int i,j=0,check=0;
	
	char *temp;
	temp = (char *)calloc(MAX_NAME_LEN, sizeof(char));
	for (j = 0; s[j] != '[' && s[j] != '\0'; j++) temp[j] = s[j];
	temp[j] = '\0';
	
	if (s[j] == '[') check = 1;
	j++;
	for (i = 0; i < label_index[0]; i++){
		if (strcmp(temp, labeltable[i].name) == 0){
			int k = 0;
			if (check == 1){
				while (s[j] != ']'){
					temp[k] = s[j];
					k++;
					j++;
				}
				temp[k] = '\0';
				return labeltable[i].address + atoi(temp);
			}
			return labeltable[i].address;
		}
	}
	return -1;
}
int get_opcode(char s[], symboldata *symboltable, symboltable_data dat, labeldata *labeltable, int *label_index){
	int ok;
	if (strcmp("AX", s) == 0)  return 0;
	else if (strcmp("BX", s) == 0)  return 1;
	else if (strcmp("CX", s) == 0)  return 2;
	else if (strcmp("ADD", s) == 0 || strcmp("DX", s) == 0) return 3;
	else if (strcmp("SUB", s) == 0 || strcmp("EX", s) == 0) return 4;
	else if (strcmp("MUL", s) == 0 || strcmp("FX", s) == 0) return 5;
	else if (strcmp("JMP", s) == 0 || strcmp("GX", s) == 0) return 6;
	else if (strcmp("IF", s) == 0 || strcmp("HX", s) == 0) return 7;
	else if (strcmp("EQ", s) == 0) return 8;
	else if (strcmp("LT", s) == 0) return 9;
	else if (strcmp("GT", s) == 0) return 10;
	else if (strcmp("LTEQ", s) == 0) return 11;
	else if (strcmp("GTEQ", s) == 0) return 12;
	else if (strcmp("PRINT", s) == 0) return 13;
	else if (strcmp("READ", s) == 0) return 14;
	else{
		ok=get_opcode_symbols(s,symboltable, dat);
	}
	if (ok == -1){
		ok = get_opcode_labels(s, labeltable, label_index);
	}
	return ok;
}

void replace(char *command, char *operation, symboldata *symboltable, symboltable_data dat, intermediatedata *out_data, int *in_no, labeldata *labeltable, int *label_index){
	int opcode = get_opcode(command,symboltable, dat,labeltable,label_index);
	if (opcode == -1){
		update_labeltable(command,in_no[0]+1,labeltable,label_index);
		return;
	}
	out_data[in_no[0]].no = in_no[0]+1;
	out_data[in_no[0]].opcode = opcode;
	char *token = strtok(operation,",");
	int parameters = 0;
	while (token!= NULL){
		out_data[in_no[0]].parameters[parameters] = get_opcode(token, symboltable, dat,labeltable,label_index);
		token = strtok(NULL, ",");
		parameters++;
	}
	in_no[0]++;
}

void fill_left_opcodes(stack *opcode_stack, intermediatedata *out_data, int sent){
	int ok = 0;
	while (opcode_stack->top != -1){
		ok = pop(opcode_stack);
		if (out_data[ok].opcode == 7)
			out_data[ok].parameters[3] = sent + 1;
		else
			out_data[ok].parameters[0] = sent + 1;
		sent = ok;
		if (out_data[sent].opcode == 7) break;
	}
}

void start_process(char *file, symboldata *symboltable, symboltable_data dat, intermediatedata *out_data,int *in_no,labeldata *labeltable,int *label_index){
	FILE *fp;

	stack *opcode_stack;
	opcode_stack = creatstack();

	char * line;
	line = (char *)calloc(MAX_LINE_COUNT, sizeof(char));
	fp = fopen(file, "r");
	if (fp == NULL){
		printf("CANNOT OPEN FILE\n");
		return ;
	}
	while (fgets(line, MAX_LINE_COUNT, fp) != NULL){
		if (strcmp(line, "START:\n") == 0) break;
	}
	while (fgets(line, MAX_LINE_COUNT, fp) != NULL){
		purify(line);
		char *command = strtok(line, " ");
		char *operation = strtok(NULL, "\n");
		if (strcmp("ENDIF",line) == 0){
			fill_left_opcodes(opcode_stack, out_data, in_no[0]);
		}
		else if (strcmp("ELSE",line) == 0){
			push(opcode_stack, in_no[0]);
			out_data[in_no[0]].no = in_no[0] + 1;
			out_data[in_no[0]].opcode = 6;
			in_no[0]++;
		}
		else if (strcmp("IF", line) == 0){
			push(opcode_stack, in_no[0]);
			out_data[in_no[0]].no = in_no[0] + 1;
			out_data[in_no[0]].opcode = 7;
			char *token = strtok(operation, " ");
			int parameters = 0;
			while (token != NULL){
				out_data[in_no[0]].parameters[parameters] = get_opcode(token, symboltable, dat,labeltable,label_index);
				token = strtok(NULL, " ");
				parameters++;
			}
			in_no[0]++;
		}
		else if (strcmp("MOV", command) == 0){
			out_data[in_no[0]].no = in_no[0] + 1;
			char *token = strtok(operation, ",");
			int parameters = 0,check=2;
			while (token != NULL){
				if (parameters == 0 && check_for_register(token) == 1) check = 1;
				out_data[in_no[0]].parameters[parameters] = get_opcode(token, symboltable, dat, labeltable, label_index);
				token = strtok(NULL, ",");
				parameters++;
			}
			out_data[in_no[0]].opcode = check;
			in_no[0]++;
		}
		else if (strcmp("END", line) != 0){
			replace(command, operation, symboltable, dat, out_data, in_no,labeltable,label_index);
		}
	}
	fclose(fp);
}

int compilefile(char *input_file,char *output_file){
	symboltable_data dat = databloc_size(input_file);
	char *s;
	s = (char *)calloc(dat.size, sizeof(char));
	FILE *fp;
	fp = fopen(input_file, "r");
	if (fp == NULL){
		return -1;
	}
	int i = 0;
	char c;
	while (i<dat.size){
		c = fgetc(fp);
		s[i] = c;
		i++;
	}
	fclose(fp);
	int *memory_index;
	memory_index = (int *)malloc(sizeof(int));
	memory_index[0] = 8;
	symboldata *symboltable = fill_symbol_table(s, dat.lines,memory_index);
	
	
	
	int *in_no;
	in_no = (int *)malloc(sizeof(int));
	in_no[0] = 0;
	int *label_index;
	label_index = (int *)malloc(sizeof(int));
	label_index[0] = 0;
	int lines = count_lines(input_file);
	
	intermediatedata *out_data;
	out_data = (intermediatedata *)calloc(lines, sizeof(intermediatedata));
	init_outdata(out_data, lines);
	labeldata *labeltable;
	labeltable = (labeldata *)calloc(lines, sizeof(labeldata));


	start_process(input_file, symboltable, dat,out_data,in_no,labeltable,label_index);


	int j;
	fp = fopen(output_file, "w");
	if (fp == NULL){
		return -1;
	}
	for (i = 0; i < in_no[0]; i++){
		fprintf(fp, "%d %d", out_data[i].no, out_data[i].opcode);
		for (j = 0; j < 4; j++){
			if (out_data[i].parameters[j] == -1) break;
			fprintf(fp, " %d", out_data[i].parameters[j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 1;
}
