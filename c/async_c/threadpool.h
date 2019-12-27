#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <semaphore.h>
#include <stddef.h>
#include <pthread.h>

enum status {Working, Free};

typedef struct runnable
{
  void (*function)(void *, size_t);
  void *arg;
  size_t argsz;
} runnable_t;

typedef struct thread_pool
{
  pthread_t *thread;        //Tablica wątków
  pthread_attr_t attr;
  sem_t mutex;
  sem_t mutex_all;          //Semafor dysponuje pool_size zezwoleniami
  enum status *working;     //Stan poszczególnych wątków
  size_t num_threads;       //Informacja o tym ile wątków jest zarezerwowanych w pamięci
} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t pool_size);

void thread_pool_destroy(thread_pool_t *pool);

int defer(thread_pool_t *pool, runnable_t runnable);

#endif
