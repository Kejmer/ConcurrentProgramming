#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "err.h"

#define READERS 3
#define WRITERS 2
#define NAP 2
#define BSIZE 32

struct readwrite {
  pthread_mutex_t lock;         
  pthread_cond_t readers;      
  pthread_cond_t writers;    
  int rcount, wcount, rwait, wwait;
  int change;
};

struct readwrite library;
char book[BSIZE];
int working = 1;

/* Initialize a buffer */

void init(struct readwrite *rw) {
}

/* Destroy the buffer */

void destroy(struct readwrite *rw) {
}

void *reader(void *data)
{
   while (working) {

     printf("reader read: %s\n", book); /* reading */

   }
   return 0;
}

void *writer(void *data)
{  
   int l;
   while (working) {

     l = rand()%10;
     snprintf(book, BSIZE, "6 times a number %d %d %d %d %d %d", l, l, l, l, l, l);

   }
   return 0;
}


int main() {
  pthread_t th[READERS+WRITERS];
  pthread_attr_t attr;
  int i, err;
  void *retval;

  srand((unsigned)time(0));
  
  init(&library);
  if ((err = pthread_attr_init (&attr)) != 0)
    syserr (err, "attr_init failed");
  if ((err = pthread_attr_setdetachstate (&attr,PTHREAD_CREATE_JOINABLE)) != 0)
    syserr (err, "attr_setdetachstate failed");

  for (i = 0; i < READERS + WRITERS; i++) {
    if (i < READERS) {
      if ((err = pthread_create(&th[i], &attr, reader, 0)) != 0)
	syserr (err, "create failed");
    } else 
      if ((err = pthread_create(&th[i], &attr, writer, 0)) != 0)
	syserr (err, "create failed");
  }
  
  sleep(NAP);
  working = 0;

  for (i = 0; i < READERS + WRITERS; i++) {
    if ((err = pthread_join(th[i], &retval)) != 0)
    syserr (err, "join failed");
  }
  
  if ((err = pthread_attr_destroy (&attr)) != 0)
    syserr (err, "cond destroy failed");
  destroy(&library);
  return 0;
}
