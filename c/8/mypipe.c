#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

char message[] = "Hello from your parent!\n";

int main (int argc, char *argv[])
{
  int pipe_dsc[2];
  
  for (int i = 1; i < argc; i++) {
    
    printf("Program to %s \n", argv[0]);
    printf("Liczba argc %d\n", argc);
    
    if (pipe(pipe_dsc) == -1) syserr("Error in pipe\n");
    
    switch (fork()) {
      case -1:
        syserr("Error in fork\n");
        
      case 0:
      printf("AAA\n");
        if (close (pipe_dsc [0]) == -1)     syserr("Error in child, close (pipe_dsc [0])\n");
        if (close (pipe_dsc [1]) == -1)     syserr("Error in child, close (pipe_dsc [1])\n");
        argv++;
        argc--;
        
        break;      
      default:
        if (close (0) == -1)                syserr("Error in child, close (0)\n");
        if (dup (pipe_dsc [0]) != 0)        syserr("Error in child, dup (pipe_dsc [0])\n");
        if (close (pipe_dsc [0]) == -1)     syserr("Error in child, close (pipe_dsc [0])\n");
        
        if (i < argc - 1) { //Nadpisujemy wyjście
          if (close (1) == -1)              syserr("Error in child, close (1)\n");
          if (dup (pipe_dsc [1]) != 1)  syserr("Error in child, dup2 (pipe_dsc[1], 1)");
        }
        if (close (pipe_dsc [1]) == -1)     syserr("Error in child, close (pipe_dsc [1])\n");
        
        if (argc >= 2) {
          execvp(argv[1], argv + 1);
          syserr("Error in execvp\n");
        }  
    }
  }
  exit(0);
}
