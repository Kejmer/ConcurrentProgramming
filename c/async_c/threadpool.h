#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <semaphore.h>
#include <stddef.h>
#include <pthread.h>

typedef struct runnable {
  void (*function)(void *, size_t);
  void *arg;
  size_t argsz;
} runnable_t;

typedef struct thread_pool {
  pthread_t *thread;        //Tablica wątków
  pthread_attr_t *attr;
  sem_t *mutex_all;         //Semafor dysponuje pool_size zezwoleniami
  sem_t *mutex_t;           //Semafory dla poszczególnych wątków
  size_t pool_size;         //Informacja o tym ile wątków jest dostępnych np. pool_size == 2 to dwa wątki a nie dwa bajty
} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t pool_size);

void thread_pool_destroy(thread_pool_t *pool);

int defer(thread_pool_t *pool, runnable_t runnable);

#endif
