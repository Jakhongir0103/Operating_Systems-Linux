/*
Topic - Thread
Description - This program receives the names of n (argv[1]) input files, sorts them concurrently using threads and prints out the result in the output files. Number of the files and names of the input/output files are given in the arguments.
Run the program with the following arguments to see it in use: "./prog 3 fileIn1.txt fileIn2.txt fileIn3.txt fileOut1.txt fileOut2.txt fileOut3.txt"
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define SIZE 64

static void *thread_function(void*);
static int *sort(int *, int);

struct files{
	char in[SIZE];
	char out[SIZE];
}files;

int main(int argc, char **argv){
	if(argc % 2 == 1){return 1;}
	
	int fN = atoi(argv[1]);
	char **fIn;
	char **fOut;
	
	fIn = malloc(fN * sizeof(char *)); if(fIn == NULL){return 2;}
	fOut = malloc(fN * sizeof(char *)); if(fOut == NULL){return 3;}
 	for(int i = 0; i < fN; i++){
    		fIn[i] = malloc(SIZE * sizeof(char)); if(fIn[i] == NULL){return 5;}
    		fOut[i] = malloc(SIZE * sizeof(char)); if(fOut[i] == NULL){return 6;}
 	}
	
	for(int i=0, j=2; i < fN; i++, j++){
		strcpy(fIn[i], argv[j]);
	}
	for(int i=0, j=2+fN; i < fN; i++, j++){
		strcpy(fOut[i], argv[j]);
	}
	
	struct files *fNames;
	fNames = malloc(fN * sizeof(files)); if(fNames == NULL){return 8;}
	
	pthread_t *th;
	th = malloc(fN * sizeof(th)); if(th == NULL){return 7;}
	for(int i=0; i < fN; i++){
		strcpy(fNames[i].out, fOut[i]);
		strcpy(fNames[i].in, fIn[i]);

		if(pthread_create(&th[i], NULL, thread_function, (void*) &fNames[i]) != 0){return 9;}
	}
	for(int i=0; i < fN; i++){
		if(pthread_join(th[i], NULL) != 0){return 10;}
	}
	
	return 0;
}

static void *thread_function(void *args){
	struct files *fNames;
	fNames = (struct files *) args;
	
	FILE *fIn;
	FILE *fOut;
	
	fIn = fopen(fNames->in, "r"); if(fIn == NULL){exit(0);}
	fOut = fopen(fNames->out, "w"); if(fOut == NULL){exit(0);}
	
	int size;
	fscanf(fIn,"%d", &size);
	
	int *array;
	array = malloc(size * sizeof(int)); if(array == NULL){exit(1);}
	
	for(int i=0; i<size; i++){
		fscanf(fIn,"%d", &array[i]);
	}
	
	int *sorted_array;
	sorted_array = sort(array, size);
	
	for(int i=0; i<size; i++){
		fprintf(fOut,"%d\n", sorted_array[i]);
	}
	
	fclose(fIn);
	fclose(fOut);
	return NULL;
}

static int *sort(int *array, int size){
	int temp;
	
	for(int i=0; i<size; i++){
    		for(int j=0; j<size-1-i; j++){
      			if(array[j] > array[j+1]){
       			temp = array[j];
        			array[j] = array[j+1];
        			array[j+1] = temp;
      			}
    		}
  	}
	return array;
}
