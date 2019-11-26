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
    
    
    if (pipe(pipe_dsc) == -1) syserr("Error in pipe\n");
    
    switch (fork()) {
      case -1:
        syserr("Error in fork\n");
        
      case 0:
        if (close (0) == -1)                syserr("Error in child, close (0)\n");
        if (dup (pipe_dsc [0]) != 0)        syserr("Error in child, dup (pipe_dsc [0])\n");
        if (close (pipe_dsc [0]) == -1)     syserr("Error in child, close (pipe_dsc [0])\n");
        if (close (pipe_dsc [1]) == -1)     syserr("Error in child, close (pipe_dsc [1])\n");
        
        break;      
      default:
        if (close (pipe_dsc [0]) == -1)     syserr("Error in child, close (pipe_dsc [0])\n");
        if (i < argc - 1) { //Nadpisujemy wyjście
          if (close (1) == -1)              syserr("Error in child, close (1)\n");
          if (dup (pipe_dsc [1]) != 1)  syserr("Error in child, dup2 (pipe_dsc[1], 1)");
        }
        if (close (pipe_dsc [1]) == -1)     syserr("Error in child, close (pipe_dsc [1])\n");
        
        if (argc >= 2) {
          char **tab = malloc(sizeof(char*));
          tab[0] = argv[1];
          execvp(argv[i], tab);
          // execl("ls", "ls", NULL);
          syserr("Error in execl\n");
        }  
    }
  }
  exit(0);
}
