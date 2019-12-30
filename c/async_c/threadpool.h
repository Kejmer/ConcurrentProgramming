#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <semaphore.h>
#include <stddef.h>
#include <pthread.h>

typedef struct runnable
{
  void (*function)(void *, size_t);
  void *arg;
  size_t argsz;
} runnable_t;

typedef struct q_runnable
{
  struct q_runnable *next;
  runnable_t runnable;
} q_runnable_t;

typedef struct thread_pool
{
  pthread_t *thread;        //Tablica wątków
  pthread_attr_t attr;
  sem_t mutex;
  sem_t mutex_all;          //Semafor dysponuje num_threads zezwoleniami
  int working;              //Informacja o tym czy wątki mają dają pracować
  size_t num_threads;       //Informacja o tym ile wątków jest zarezerwowanych w pamięci
  q_runnable_t *queue;      //Kolejka zleceń które nie dostały przydzielonego wątku
  q_runnable_t *last;       //Wskaźnik na koniec kolejki
} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t pool_size);

void thread_pool_destroy(thread_pool_t *pool);

int defer(thread_pool_t *pool, runnable_t runnable);

#endif
