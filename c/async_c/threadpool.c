#include <stdlib.h>
#include <stdio.h>
#include "threadpool.h"

//IMPLEMENTACJA KOLEJKI
//**********************************************************************************************************************//

static int empty(thread_pool_t *pool)
{
  return pool->queue == NULL ? 1 : 0;
}

static runnable_t pop(thread_pool_t *pool)
{
  runnable_t result = pool->queue->runnable;
  q_runnable_t *next = pool->queue->next;
  free(pool->queue);
  pool->queue = next;
  if (empty(pool)) {
    pool->last = NULL;
  }
  return result;
}

static void put(thread_pool_t *pool, runnable_t runnable)
{
  q_runnable_t *new_q = malloc(sizeof(q_runnable_t));
  if (new_q == NULL) {
    exit(-1);
  }
  new_q->next = NULL;
  new_q->runnable = runnable;
  if (empty(pool)) {
    pool->queue = new_q;
  } else {
    pool->last->next = new_q;
  }
  pool->last = new_q;
}

//**********************************************************************************************************************//
//OCZEKIWANIE NA ZADANIA DO WYKONANIA

static void* delayed_worker_start(void *arg)
{
  int err;
  thread_pool_t *pool = ((thread_pool_t *) arg);
  runnable_t runnable;
  int skip;

  while (pool->working || empty(pool) == 0) {
    if ((err = sem_wait(&pool->mutex_all))) {
      fprintf(stderr, "%d error in async – sem_wait\n", err);
      exit(-1);
    }
    if ((err = sem_wait(&pool->mutex))) {
      fprintf(stderr, "%d error in async – sem_wait\n", err);
      exit(-1);
    }
    skip = 1;
    if (empty(pool) == 0) {
      runnable = pop(pool);
      skip = 0;
    }
    if ((err = sem_post(&pool->mutex))) {
      fprintf(stderr, "%d error in async – sem_post\n", err);
      exit(-1);
    }
    if (skip == 0) (* runnable.function)(runnable.arg, runnable.argsz);
  }
  return 0;
}

//IMPLEMENTACJA PLIKU NAGŁÓWKOWEGO
//**********************************************************************************************************************//

int thread_pool_init(thread_pool_t *pool, size_t num_threads)
{
  int err;

  pool->num_threads = num_threads;
  pool->thread = malloc(num_threads * sizeof(pthread_t));
  if (pool->thread == NULL) {
    fprintf(stderr, "Error in thread_pool_init – malloc\n");
    return -1;
  }

  if ((err = pthread_attr_init(&pool->attr))) {
    fprintf(stderr, "%d error in thread_pool_init – pthread_attr_init\n", err);
    free(pool->thread);
    return -1;
  }

  if ((err = sem_init(&pool->mutex_all, 0, 0))) {
    fprintf(stderr, "%d error in thread_pool_init – sem_init\n", err);
    free(pool->thread);
    if (pthread_attr_destroy(&pool->attr)) {
      fprintf(stderr, "%d error in pthread_attr_destroy\n", err);
      exit(-1);
    }
    return -1;
  }

  if ((err = sem_init(&pool->mutex, 0, 1))) {
    fprintf(stderr, "%d error in thread_pool_init – sem_init\n", err);
    free(pool->thread);
    if ((err = pthread_attr_destroy(&pool->attr))) {
      fprintf(stderr, "%d error in pthread_attr_destroy\n", err);
      exit(-1);
    }
    if ((err = sem_destroy(&pool->mutex_all))) {
      fprintf(stderr, "%d error in sem_destroy\n", err);
      exit(-1);
    }
    return -1;
  }

  pool->working = 1;
  pool->queue = NULL;
  pool->last = NULL;

  for (unsigned i = 0; i < num_threads; i++) {
    if ((err = pthread_create(&pool->thread[i], &pool->attr, &delayed_worker_start, pool))) {
      fprintf(stderr, "%d error in pthread_attr_init – pthread_create\n", err);
      exit(-1);
    }
  }
  return 0;
}

void thread_pool_destroy(thread_pool_t *pool)
{
  int err;
  int *res;
  if ((err = sem_wait(&pool->mutex))) {
    fprintf(stderr, "%d error in defer – sem_wait\n", err);
    exit(-1);
  }
  pool->working = 0;
  if ((err = sem_post(&pool->mutex))) {
    fprintf(stderr, "%d error in defer – sem_post\n", err);
    exit(-1);
  }

  for (unsigned i = 0; i < pool->num_threads; i++) {
    if ((err = sem_post(&pool->mutex_all))) {
      fprintf(stderr, "%d error in thread_pool_destroy – sem_post\n", err);
      exit(-1);
    }
  }

  for (unsigned i = 0; i < pool->num_threads; i++) {
    if (((err = pthread_join(pool->thread[i], (void **) &res)))) {
      fprintf(stderr, "%d error in thread_pool_destroy – pthread_join\n", err);
      exit(-1);
    }
  }

  free(pool->thread);

  if ((err = pthread_attr_destroy(&pool->attr))) {
    fprintf(stderr, "%d error in thread_pool_destroy – pthread_attr_destoy\n", err);
    exit(-1);
  }
  if ((err = sem_destroy(&pool->mutex_all))) {
    fprintf(stderr, "%d error in thread_pool_destroy – sem_destroy\n", err);
    exit(-1);
  }
  if ((err = sem_destroy(&pool->mutex))) {
    fprintf(stderr, "%d error in thread_pool_destroy – sem_destroy\n", err);
    exit(-1);
  }
}

int defer(thread_pool_t *pool, runnable_t runnable)
{
  int err;
  if (pool->working == 0) return -1;
  if ((err = sem_wait(&pool->mutex))) {
    fprintf(stderr, "%d error in defer – sem_wait\n", err);
    return -1;
  }
  if (pool->working == 0) {
    if ((err = sem_post(&pool->mutex))) {
      fprintf(stderr, "%d error in defer – sem_post\n", err);
      exit(-1);
    }
    return -1;
  }
  put(pool, runnable);
  if ((err = sem_post(&pool->mutex))) {
    fprintf(stderr, "%d error in defer – sem_post\n", err);
    return -1;
  }
  if ((err = sem_post(&pool->mutex_all))) {
    fprintf(stderr, "%d error in defer – sem_post\n", err);
    return -1;
  }
  return 0;
}

//**********************************************************************************************************************//