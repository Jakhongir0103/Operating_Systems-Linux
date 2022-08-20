/*
Topic - Communication between processes through signals and pipes

Description - This is s C program that generates two children, a child 1 and a child 1. The child 2 reads from the keyboard some strings, and it transfers them to the child 1. The child 1 converts the strings into uppercase characters, and it visualizes the strings in standard output. Entering the stirng "end" stops the program. The transfer of the strings (and synchronization) takes place through pipes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h> 
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define SIZE 4096

static void child2 (pid_t, int*, int*);
static void child1 (int*, int*);
static void signalHandler(int);

int main (void) {
  pid_t pid1; // communication of text
  pid_t pid2; // communication of text size, and the PID of child2

  int fd1[2];
  int fd2[2];
  if(pipe(fd1) == -1){exit(0);}
  if(pipe(fd2) == -1){exit(0);}
  
  pid1 = fork ();
  if(pid1 == -1){
    printf("Error creating fork 1\n");
    exit(0);
  }
  if (pid1==0) {
    // Child 1
    sleep(1); 
    child1(fd1, fd2);
  } else {
    // Parent Running
    pid2 = fork ();
    if(pid2 == -1){
      printf("Error creating fork 2\n");
      exit(0);
    }
    if (pid2==0){
    	// Child 2
	child2(pid1, fd1, fd2); // pid1 = pid of child1
    } else {
        wait(NULL);
        wait(NULL);
    }
  }

  return (0);
}

static void child2(pid_t pid, int *fd1, int *fd2) {
  char text[SIZE];
  int info[2];
  
  signal(SIGUSR2, signalHandler);

  while (1) {   
    printf("Enter a text: ");
    scanf("%s", text);
    close(fd1[0]);
    if(write(fd1[1], text, sizeof(char)*strlen(text)) == -1){
        printf("Error writing text\n");
        exit(0);
    }

    info[0] = getpid(); // pid of child2
    info[1] = strlen(text); // length of string text
    close(fd2[0]);
    if(write(fd2[1], info, sizeof(int)*2) == -1){
        printf("Error writing info\n");
        exit(0);
    }
    
    kill(pid, SIGUSR1);
    
    if(!strcmp(text, "end")){
        exit(0);
    }
    
    pause ();
    //child2 woke up
  }
  return;
}

static void child1(int *fd1, int *fd2) {
  char *text;
  int info[2];
  
  signal(SIGUSR1, signalHandler);

  while (1) {
    pause();
    //child1 woke up
    
    close(fd2[1]);
    if(read(fd2[0], info, sizeof(int)*2) == -1){
        printf("Error read info\n");
        exit(0);
    }
    
    text = (char*) malloc(sizeof(char)*info[1]);
    close(fd1[1]);
    if(read(fd1[0], text, sizeof(char)*info[1]) == -1){
        printf("Error read text\n");
        exit(0);
    }

    if(!strcmp(text, "end")){
        exit(0);
    }

    int i=0;
    while (text[i]) { 
        text[i] = toupper(text[i]); 
        i++; 
    }

    printf("Transformed text: %s\n", text);

    kill(info[0], SIGUSR2);
  }
  return;
}

static void signalHandler(int sig){
  return;
}

