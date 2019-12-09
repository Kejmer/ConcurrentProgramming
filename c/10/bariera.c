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
#define N   10
#define K   5

int main ()
{
  
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
        sleep(sleep_time);  // wymagana drzemka
        
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

  while(i<N){
    printf("Czekam na dziecko\n");
    if (wait(0) == -1){
        syserr("wait");
    }else{
      printf("Odebrałem dziecko\n");
      ++i;
    }
  }

  printf("Rodzic: Kończę\n");
  
  return 0;  
}
/**********************************************************************/
