#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "err.h"


/*Liczba dzieci*/
#define N   12
# define K   5
#define SIGDRZEMKA (SIGRTMIN + 1)

int signals;
int pipe_dsc[2];
extern int errno;

void catch (int sig) { 
  signals++;
  printf("Dostałem sygnał >>%s<<\n", strsignal(sig)); 
  if (signals == K) {
    printf("Zamykam\n");
    if (close(pipe_dsc[1]) == -1) syserr("Error in close(pipe_dsc[1])\n");
  }
}

int main ()
{
  struct sigaction action;
  sigset_t block_mask;

  sigemptyset (&block_mask);
  sigaddset(&block_mask, SIGDRZEMKA);
  
  action.sa_handler = catch;
  action.sa_mask = block_mask;
  action.sa_flags = 0;
  
  if (sigaction (SIGDRZEMKA, &action, 0) == -1)              /*Nowa obługa SIGRTMIN+1*/
    syserr("sigaction");
    
  if (pipe(pipe_dsc) == -1) syserr("Error in pipe\n");
   
  
  pid_t parent_pid, child_pid[N];
  int i;
  srand((unsigned) time(0));  // losowość

  parent_pid = getpid();

  for(i=0; i< N; ++i){    //tworzę N dzieci
    int sleep_time = rand()%10 + 1;   // pseudolosowość wymaga losowania w rodzicu
    switch (child_pid[i] = fork()){
      
      case -1:
        syserr("Error in fork\n");
      case 0:
        /*child*/
        printf("Dziecko %d: Zaczynam\n\n", getpid());
        if (close(pipe_dsc[1]) == -1) syserr("Error in close(pipe_dsc[1])\n");
        
        sleep(sleep_time);  // wymagana drzemka
        char buf[2];
        if (kill(parent_pid, SIGDRZEMKA) == -1)
          syserr("kill");
        
        printf("Czekam %d\n", getpid());
        
        if(read(pipe_dsc[0], buf, 1) == -1)
          syserr("Error in read");
        
        printf("Dziecko %d: Kończę\n\n", getpid());
        return 0;
      default:
#ifdef D
        printf("Parent %d\n", i);
#endif
        break;
    }
  }
  /*parent*/
  i=0;
  while(i<N){
    printf("Czekam na dziecko\n");
    if (wait(0) == -1){
      if (errno != EINTR)
        syserr("wait");
    }else{
      printf("Odebrałem dziecko\n");
      ++i;
    }
  }

  if (close(pipe_dsc[0]) == -1) syserr("Error in close(pipe_dsc[0])\n");
  printf("Rodzic: Kończę\n");
  
  return 0;  
}
/**********************************************************************/
