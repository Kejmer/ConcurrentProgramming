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
  int produced_gifts;
  int received_gifts;
  int gifts[MAX_GIFTS];
  sem_t warehouse_in;
  sem_t warehouse_out;
  sem_t mutex;
};
/**************************************************************************************************/
//popular names
char *elfs_names[MAX_ELFS] = {"Mirek","Zuzia","Gienia", "Macius", "Ela", "Stasia"};
char *reindeers_names[MAX_REINDEERS] = {"Janek", "Zosia", "Franek", "Jozek", "Asia", "Olek", "Ruda"};
char *gifts[MAX_GIFTS] = {"lalka", "klocki", "ciuchcia", "rozga", "rower"};

/**************************************************************************************************/
//helper functions
int getNextGift(struct storage* s) {
  int g;
  if (sem_wait(&s->warehouse_out)) syserr("warehouse_out wait");
  if (sem_wait(&s->mutex)) syserr("mutex wait");
  
  g = s->gifts[s->received_gifts % MAX_GIFTS];
  s->received_gifts++;
  
  if (sem_post(&s->mutex)) syserr("mutex post");
  if (sem_post(&s->warehouse_in)) syserr("warehouse_in post");
  return g;
}

void addNewGift(struct storage* s, int g) {
  if (sem_wait(&s->warehouse_in)) syserr("warehouse_in wait");
  if (sem_wait(&s->mutex)) syserr("mutex wait");
  
  s->gifts[s->produced_gifts % MAX_GIFTS] = g;
  s->produced_gifts++;
  
  if (sem_post(&s->mutex)) syserr("mutex post");
  if (sem_post(&s->warehouse_out)) syserr("warehouse_out post");
   
}

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
    addNewGift(s, g);

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

    g = getNextGift(s);  
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
  
  int flags, prot;
  int fd_memory = -1; /* deskryptor dla pamięci*/
  prot = PROT_READ | PROT_WRITE;
  flags = MAP_SHARED | MAP_ANONYMOUS; 
  
    
  void *mapped_mem_temp =  mmap(NULL, sizeof(struct storage), prot, flags,
                  fd_memory, 0);
  
  if(mapped_mem_temp == MAP_FAILED)
    syserr("mmap");
  
  mapped_mem = (struct storage *) mapped_mem_temp;
  
  mapped_mem->produced_gifts = 0;
  mapped_mem->received_gifts = 0;
  if (sem_init(&mapped_mem->mutex, 1, 1)) syserr("sem init");
  if (sem_init(&mapped_mem->warehouse_in, 1, MAX_GIFTS)) syserr("sem init");
  if (sem_init(&mapped_mem->warehouse_out, 1, 0)) syserr("sem init");  

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
    
  sem_destroy(&mapped_mem->mutex);
  sem_destroy(&mapped_mem->warehouse_in);
  sem_destroy(&mapped_mem->warehouse_out);
  munmap(mapped_mem,  sizeof(struct storage)); // i tak zniknie, kiedy proces zginie    
    
  return 0;
  }
