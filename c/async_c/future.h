#ifndef FUTURE_H
#define FUTURE_H

#include <semaphore.h>
#include "threadpool.h"

//NOTATKA DLA SIEBIE
// Kopiując callable trzeba skopiować też to co siedzi w argumencie poprzez malloc odpowiedni wielkości argsz.

typedef struct callable {
  void *(*function)(void *, size_t, size_t *);
  void *arg;
  size_t argsz;
} callable_t;

typedef struct future {
  void *result;
  sem_t lock;
} future_t;

int async(thread_pool_t *pool, future_t *future, callable_t callable);

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *));

void *await(future_t *future);

#endif
