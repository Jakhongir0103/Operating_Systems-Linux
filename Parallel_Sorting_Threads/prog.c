/*
Topic - Parallel shoritng using Thread
Description - This program receives the names of n (argv[1]) input files, sorts them concurrently using threads and merges them into the output file. Number of the files and names of the input/output files are given in the arguments.
Run the program with the following arguments to see it in use: "./prog 3 fileIn1.txt fileIn2.txt fileIn3.txt fileOutAll.txt"
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 64
#define ARRAY_SIZE 8
#define MIN -10000000

static void *thread_function(void*);
static void sort(int, int);
static void mergeSort(int, int);
static void writeOutput(char*, int);

static int **matrix;
static int *sortedArray;

struct files{
	char in[SIZE];
	int index;
};

int main(int argc, char **argv){
	if(argc % 2 == 1){return 1;}
	int fN = atoi(argv[1]);
	
	char **fIn;
	matrix = malloc(fN * sizeof(int *)); if(matrix == NULL){return 0;}
	sortedArray = malloc(ARRAY_SIZE * fN * sizeof(int)); if(sortedArray == NULL){return 0;}
	
 	for(int i = 0; i < ARRAY_SIZE * fN; i++){
    		sortedArray[i] = MIN;
 	}
	
	fIn = malloc(fN * sizeof(char *)); if(fIn == NULL){return 2;}
 	for(int i = 0; i < fN; i++){
    		fIn[i] = malloc(SIZE * sizeof(char)); if(fIn[i] == NULL){return 5;}
 	}
	
	for(int i=0, j=2; i < fN; i++, j++){
		strcpy(fIn[i], argv[j]);
	}
	
	struct files *fStr;
	fStr = malloc(fN * sizeof(struct files)); if(fStr == NULL){return 8;}
	
	pthread_t *th;
	th = malloc(fN * sizeof(th)); if(th == NULL){return 7;}
	for(int i=0; i < fN; i++){
		fStr[i].index = i;
		strcpy(fStr[i].in, fIn[i]);

		if(pthread_create(&th[i], NULL, thread_function, (void*) &fStr[i]) != 0){return 9;}
	}
	for(int i=0; i < fN; i++){
		if(pthread_join(th[i], NULL) != 0){return 10;}
	}

	for(int i=0; i < fN; i++){
		mergeSort(i, ARRAY_SIZE*(i+1));
	}	
	
	writeOutput(argv[fN+2], ARRAY_SIZE * fN);
	
	return 0;
}

static void *thread_function(void *args){
	struct files *fInfo;
	fInfo = (struct files *) args;
	
	FILE *fIn;
	
	fIn = fopen(fInfo->in, "r"); if(fIn == NULL){exit(0);}
	
	int size;
	fscanf(fIn,"%d", &size);
	
	matrix[fInfo->index] = malloc(size * sizeof(int)); if(matrix[fInfo->index] == NULL){exit(1);}
	
	for(int i=0; i<size; i++){
		fscanf(fIn,"%d", &matrix[fInfo->index][i]);
	}
	
	sort(fInfo->index, size);
	
	fclose(fIn);
	return NULL;
}

static void sort(int index, int size){
	int temp;
	
	for(int i=0; i<size; i++){
    		for(int j=0; j<size-1-i; j++){
      			if(matrix[index][j] > matrix[index][j+1]){
       			temp = matrix[index][j];
        			matrix[index][j] = matrix[index][j+1];
        			matrix[index][j+1] = temp;
      			}
    		}
  	}

	return;
}

static void mergeSort(int index, int len){
	int i = 0, j = 0, k = 0;
	int size = 0;

	for(int m=0; m < len; m++){
		if(sortedArray[m] != MIN){size++;}
	}
	
	int *tempArray;
	tempArray = malloc((size+ARRAY_SIZE) * sizeof(int)); if(tempArray == NULL){exit(0);}
	
    	while (i < size && j < ARRAY_SIZE)
    	{
        	if (sortedArray[i] < matrix[index][j]){
            		tempArray[k++] = sortedArray[i++];    
        	}else{
            		tempArray[k++] = matrix[index][j++];
            	}
    	}
  
    	while (i < size){tempArray[k++] = sortedArray[i++];}
  
    	while (j < ARRAY_SIZE){tempArray[k++] = matrix[index][j++];}
    	
    	for(int m=0; m<size+ARRAY_SIZE; m++){
    		sortedArray[m] = tempArray[m];
    	}
	
	return;
}

static void writeOutput(char *fName, int len){
	FILE *fOut;
	
	fOut = fopen(fName, "w"); if(fOut == NULL){exit(0);}

	for(int i=0; i < len; i++){
		fprintf(fOut,"%d ", sortedArray[i]);
	}
	fprintf(fOut,"\n");
	fclose(fOut);
	
	return;
}
