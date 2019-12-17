#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include "err.h"


//some consts
#define MAX_REINDEERS 7
#define MAX_ELFS 6
#define MAX_GIFTS 5

#define NO_REINDEERS 3
#define NO_ELFS 4
#define NO_GIFTS 5

#define BUFFSIZE 3
#define LOOPS 5

/**************************************************************************************************/
//storage compartment
struct storage{

};
/**************************************************************************************************/
//popular names
char *elfs_names[MAX_ELFS] = {"Mirek","Zuzia","Gienia", "Macius", "Ela", "Stasia"};
char *reindeers_names[MAX_REINDEERS] = {"Janek", "Zosia", "Franek", "Jozek", "Asia", "Olek", "Ruda"};
char *gifts[MAX_GIFTS] = {"lalka", "klocki", "ciuchcia", "rozga", "rower"};

/**************************************************************************************************/
//toymaker
int produce(){
  sleep(rand() % 3);
  return rand()%NO_GIFTS;
}

//sent to santa
void deliver(int i){
  sleep(rand() % 3);
}

void nap(int i){
  sleep(i);
}
/**************************************************************************************************/
//life of an elf
void elf(int id, struct storage* s){

  int i,g;
  printf("Hej! Jestem elfem o imieniu %s, zaczynam!\n", elfs_names[id]);
  for(i = 0; i< LOOPS; ++i){

    g = produce();
    printf("Hej! Jestem elfem o imieniu %s, wyprodukowalem/am prezent: %s\n", elfs_names[id], gifts[g]);

    printf("Hej! Jestem elfem o imieniu %s, wstawilem/am prezent: %s\n", elfs_names[id], gifts[g]);
  }
}

/**************************************************************************************************/
//life of a reindeer
void reindeer(int id, struct storage* s){
  
  int end = 0;
  int g;
  
  printf("Hej! Jestem reniferem o imieniu %s, zaczynam!\n", reindeers_names[id]);
  while(!end){

  
    printf("Hej! Jestem reniferem o imieniu %s, odebralem/am prezent: %s\n", reindeers_names[id], gifts[g]);

    deliver(g);
    printf("Hej! Jestem reniferem o imieniu %s dostarczylem/am prezent: %s\n", reindeers_names[id], gifts[g]);
    
  }
}
/**************************************************************************************************/
/**************************************************************************************************/
int main(){
    
  int i;
  pid_t pid;
  struct storage *mapped_mem;
  
  int seed = time(0);
  srand(seed);

  printf("Tworze pracownikow.\nElfy: %d; Renifery: %d\n", NO_ELFS, NO_REINDEERS);
  
  for(i = 0; i < NO_ELFS + NO_REINDEERS; i++){
    
    rand();//some randomness
    switch(pid = fork()){
    case -1:
      syserr("fork");
    case 0:
      srand(rand());
      if (i < NO_ELFS){
        printf("Elf %d!\n", i);
        elf(i, mapped_mem);
        
      }else{
        printf("Renifer %d!\n", i);
        reindeer(i-NO_ELFS,mapped_mem);
      }
      return 0;
    default:
      nap(1);
      printf("Kolejny pracownik!\n");
      break;
    }
  }

  for(i = 0; i< NO_ELFS+NO_REINDEERS; ++i) wait(0);
  return 0;
  }
