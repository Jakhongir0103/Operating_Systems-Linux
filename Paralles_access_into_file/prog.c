/*
Topic - semaphores, paralles accessing into a file
Description - A file, of undefined length contains a list of integer numbers. The program, after receiving a value k (integer) and a string from command line, generates k threads and waits them. Each thread reads concurrently the file, and performs the sum of the read integer numbers. When the end of file is reached, it prints sum of the integer numbers it has read and terminates. After all threads terminate, the main thread prints the total sum.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

sem_t mutex;
FILE *fp;
pthread_t *th;
int *sums;
int *indexes;

void *readFile(void *args);

int main(int argc, char **argv){
	fp = fopen(argv[2], "r");
	
	sem_init(&mutex, 0, 1);
	
	th = (pthread_t *) malloc(atoi(argv[1]) * sizeof(pthread_t));
	sums = (int *) malloc(atoi(argv[1]) * sizeof(int));
	indexes = (int *) malloc(atoi(argv[1]) * sizeof(int));

	for(int i=0; i<atoi(argv[1]); i++){
		sums[i]=0;
		indexes[i] = i;
		pthread_create(&th[i], NULL, &readFile, &indexes[i]);
	}

	for(int i=0; i<atoi(argv[1]); i++){
		pthread_join(th[i], NULL);
	}
	
	int sum = 0;
	for(int i=0; i<atoi(argv[1]); i++){
		sum = sum + sums[i];
	}
	printf("Total: %d\n", sum);
	
	sem_destroy(&mutex);

	return(1);
}

void *readFile(void *args){
	int *val = args;
	int index = *val;
	int d, retVal;
		
	while(1){
		sem_wait(&mutex);
		retVal = fscanf(fp, "%d", &d);
		sem_post(&mutex);
		if(retVal == EOF){
			break;
		}
		sums[index] = sums[index] + d;
	}
		printf("Thread# %d: Sum=%d\n", index, sums[index]);
		pthread_exit((void *)1);
}
