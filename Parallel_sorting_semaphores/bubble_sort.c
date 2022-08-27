/*
Topic - Semaphores, Thread
Description - given an array with size n, the program sorts the array using bubble sort with n-1 threads. For simplicity, the array is defined beforehand 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "semaphore.h"

// array declaration
int arrSize = 15;
int array[] = {56, 89, 5, 78, 65, 31, 74, 25, 16, 20, 98, 751, 62, 75, 8};

// semaphores declaration
sem_t master;
sem_t *slave; // of size thSize
sem_t *vector; // of size arrSize

// threads declaration
pthread_t mr, *sl;

// thread functions declaration
void *master_th(void *);
void *slave_th(void *);

// a function and a flag to check the sorted array
int checkSorted();
int sorted = 0;

int main(int argc, char **argv){
	int thSize = arrSize-1;
	
	// allocate the semaphores
	slave = (sem_t *) malloc(thSize * sizeof(sem_t));
	if(slave == NULL) {exit(0);}
	
	vector = (sem_t *) malloc(arrSize * sizeof(sem_t));
	if(vector == NULL) {exit(0);}
	
	// initialize the semaphores
	sem_init(&master, 0, thSize);
	
	for(int i = 0; i < thSize; i++){
		sem_init(&slave[i], 0, 0);
	}
	
	for(int i = 0; i < arrSize; i++){
		sem_init(&vector[i], 0, 1);
	}

	// allocate the slave threads
	sl = (pthread_t *) malloc(thSize * sizeof(pthread_t));
	if(sl == NULL) {exit(0);}
	
	// start threads
	if (pthread_create(&mr, NULL, &master_th, &thSize) == -1) {exit(0);}
	
	int *index;
	index = (int *) malloc(sizeof(int) * thSize);
	if (index == NULL) {exit(0);}
	
	for(int i = 0; i < thSize; i++){
		index[i] = i;
		if (pthread_create(&sl[i], NULL, &slave_th, &index[i]) == -1) {exit(0);}
	}
	
	// finish thread
	pthread_join(mr, NULL);
	for(int i = 0; i < thSize; i++){
		pthread_join(sl[i], NULL);
	}

	for(int i = 0; i < arrSize; i++){
		printf("%d ", array[i]);
	}
	printf("\n");
	
	return 1;
}

void *master_th(void *arg){
	int *numThP, numTh;
	numThP = (int *) arg;
	numTh = *numThP;
	
	while(1){
		for(int i=0; i<numTh; i++){
			sem_wait(&master);
		}

		if(checkSorted() == 1){
			sorted = 1;
		}
		
		for(int i=0; i<numTh; i++){
			sem_post(&slave[i]);
		}

		if(sorted == 1){
			break;
		}
	}

	return NULL;
}

void *slave_th(void *arg){
	int *indexP, index;
	indexP = (int *) arg;
	index = *indexP;

	while(1){
		sem_wait(&slave[index]);

		if(sorted == 1){
			break;
		}
		
		sem_wait(&vector[index]);
		sem_wait(&vector[index+1]);
		
		/*   critical section 	*/
		if(array[index] > array[index+1]){	
			int tmp = array[index];	
			array[index] = array[index+1];	
			array[index+1] = tmp;		
		}
		/*			*/
		sem_post(&vector[index]);
		sem_post(&vector[index+1]);	
		
		sem_post(&master);	
	}	
	
	return NULL;
}

int checkSorted(){
	for(int i = 0; i < arrSize-1; i++){
		if(array[i] > array[i+1]){
			return 0;
		}
	}
	return 1;
}
