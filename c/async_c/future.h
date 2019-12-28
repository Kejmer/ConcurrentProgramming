#ifndef FUTURE_H
#define FUTURE_H

#include <semaphore.h>
#include "threadpool.h"


typedef struct callable {
  void *(*function)(void *, size_t, size_t *);
  void *arg;
  size_t argsz;
} callable_t;

typedef struct future {
  void *result;
  sem_t lock;
  size_t retsz;
} future_t;

int async(thread_pool_t *pool, future_t *future, callable_t callable);

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *));

void *await(future_t *future);

//Zakładamy że await można wywołać tylko raz!
//Wywołanie await po wywołaniu map traktowane jest jako bład.
//Map zwalnia pamięc wyniku from->result

#endif
