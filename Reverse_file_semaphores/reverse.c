/*
Topic - Threads, Semaphores
Description - the program receives in the command line two strings. The first specifies the name of an input file. The second specifies the name of an output file. The input file contains unspecified number of lines with maximum of 100 charecters in each. It executes 3 threads:
The first thread reads lines from the input file (one at a time)
The second thread inverts the content of a line (the last character becomes the first and vice versa), and transforms all the literal characters in uppercase
The third thread saves each line transformed by the second thread in the output file (sequentially)
When the first thread identifies the end of the file, it must allow the second and third thread an ordered termination, and it terminates itself. 
Run the program with the following arguments to see it in use: ./reverse inFile.txt outFile.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

sem_t sR, sW, sRev;

FILE *fR, *fW;

void *readFile(void *);
void *revFile(void *);
void *saveFile(void *);

char line[MAX];
int breakRead = 0;
int eof = 0;

int main(int argc, char **argv){
	if (argc != 3) {exit (0);}

	fR = fopen(argv[1],  "r");	
	fW = fopen(argv[2],  "w");	
	if(fR == NULL || fW == NULL) {exit (0);}
	
	sem_init(&sR, 0, 1);
	sem_init(&sW, 0, 0);
	sem_init(&sRev, 0, 0);
	
	pthread_t thRead, thRev, thSave;
	if (pthread_create(&thRead, NULL, &readFile, NULL) == -1){exit (0);}
	if (pthread_create(&thRev, NULL, &revFile, NULL) == -1){exit (0);}
	if (pthread_create(&thSave, NULL, &saveFile, NULL) == -1){exit (0);}
	
	if (pthread_join(thRead, NULL) == -1) {exit (0);}
	if (pthread_join(thRev, NULL) == -1) {exit (0);}
	if (pthread_join(thSave, NULL) == -1) {exit (0);}

	return 1;
}

void *readFile(void *arg){
	while(1){				
		sem_wait(&sR);

		if(breakRead == 1){
			break;
		}
		if(fgets(line, MAX, fR) == NULL){
			eof = 1;
		}
		
		sem_post(&sRev);
	}
	
	return NULL;
}

void *revFile(void *arg){
	while(1){
		sem_wait(&sRev);

		if(eof == 1){
			breakRead = 1;
			sem_post(&sW);
			break;
		}
		for(int i=0; i<strlen(line)-1; i++){
			line[i] = toupper(line[i]);
		}
		for(int i=0; i<(strlen(line)-1)/2; i++){
			char c = line[i];
			line[i] = line[strlen(line)-1-1-i];
			line[strlen(line)-1-1-i] = c;
		}

		sem_post(&sW);
	}
	
	return NULL;
}

void *saveFile(void *arg){
	while(1){
		sem_wait(&sW);

		if(eof == 1){
			breakRead = 1;
			sem_post(&sR);
			break;
		}
		fputs(line, fW);		

		sem_post(&sR);
	}
	
	return NULL;
}

