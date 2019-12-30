#include <stdlib.h>
#include <stdio.h>
#include "threadpool.h"

typedef struct async_arg
{
  thread_pool_t *pool;
  int thread_num;
  runnable_t runnable;
} async_arg_t;

//IMPLEMENTACJA KOLEJKI
//**********************************************************************************************************************//

static int empty(thread_pool_t *pool)
{
  return pool->queue == NULL ? 0 : 1;
}

static void free_queue(q_runnable_t *queue) {
  if (queue == NULL) return;
  free_queue(queue->next);
  free(queue);
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

static int put(thread_pool_t *pool, runnable_t runnable)
{
  q_runnable_t *new_q = malloc(sizeof(q_runnable_t));
  if (new_q == NULL) {
    return EXIT_FAILURE;
  }
  new_q->next = NULL;
  new_q->runnable = runnable;
  if (empty(pool)) {
    pool->queue = new_q;
  }
  pool->queue = new_q;
  return EXIT_SUCCESS;
}

//**********************************************************************************************************************//

//Oczekiwanie na zadania do wykonania
static void* delayed_worker_start(void *arg)
{
  int *err = malloc(sizeof(int));
  if (err == NULL) {
    return err;
  }
  thread_pool_t *pool = ((thread_pool_t *) arg);
  *err = 0;
  runnable_t runnable;

  while (pool->working) {
    if ((*err = sem_wait(&pool->mutex_all))) {
      return err;
    }
    if ((*err = sem_wait(&pool->mutex))) {
      return err;
    }
    if (pool->working) {
      runnable = pop(pool);
    }
    if ((*err = sem_post(&pool->mutex))) {
      return err;
    }
    if (pool->working) {
      (* runnable.function)(runnable.arg, runnable.argsz);
    }
  }
  return err;
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
    return EXIT_FAILURE;
  }

  if ((err = pthread_attr_init(&pool->attr))) {
    fprintf(stderr, "%d error in thread_pool_init – pthread_attr_init\n", err);
    free(pool->thread);
    return EXIT_FAILURE;
  }

  if ((err = sem_init(&pool->mutex_all, 0, 0))) {
    fprintf(stderr, "%d error in thread_pool_init – sem_init\n", err);
    free(pool->thread);
    if (pthread_attr_destroy(&pool->attr)) {
      fprintf(stderr, "%d error in pthread_attr_destroy\n", err);
    }
    return EXIT_FAILURE;
  }

  if ((err = sem_init(&pool->mutex, 0, 1))) {
    fprintf(stderr, "%d error in thread_pool_init – sem_init\n", err);
    free(pool->thread);
    if ((err = pthread_attr_destroy(&pool->attr))) {
      fprintf(stderr, "%d error in pthread_attr_destroy\n", err);
    }
    if ((err = sem_destroy(&pool->mutex_all))) {
      fprintf(stderr, "%d error in sem_destroy\n", err);
    }
    return EXIT_FAILURE;
  }

  pool->working = 1;
  pool->queue = NULL;

  for (unsigned i = 0; i < num_threads; i++) {
    if ((err = pthread_create(&pool->thread[i], &pool->attr, &delayed_worker_start, pool))) {
      fprintf(stderr, "%d error in pthread_attr_init – pthread_create\n", err);
    }
  }
  return EXIT_SUCCESS;
}

void thread_pool_destroy(thread_pool_t *pool)
{
  int err;
  int *res;
  pool->working = 0;

  for (unsigned i = 0; i < pool->num_threads; i++) {
    if ((err = sem_post(&pool->mutex_all))) {
      fprintf(stderr, "%d error in thread_pool_destroy – sem_post\n", err);
    }
  }

  for (unsigned i = 0; i < pool->num_threads; i++) {
    if (((err = pthread_join(pool->thread[i], (void **) &res)))) {
      fprintf(stderr, "%d error in thread_pool_destroy – pthread_join\n", err);
    }
    if (*res) {
      fprintf(stderr, "%d error in thread_pool_destroy – invalid thread end\n", *res);
    }
    free(res);
  }

  free(pool->thread);
  free_queue(pool->queue);

  if ((err = pthread_attr_destroy(&pool->attr))) {
    fprintf(stderr, "%d error in thread_pool_destroy – pthread_attr_destoy\n", err);
  }
  if ((err = sem_destroy(&pool->mutex_all))) {
    fprintf(stderr, "%d error in thread_pool_destroy – sem_destroy\n", err);
  }
  if ((err = sem_destroy(&pool->mutex))) {
    fprintf(stderr, "%d error in thread_pool_destroy – sem_destroy\n", err);
  }
}

int defer(thread_pool_t *pool, runnable_t runnable)
{
  int err;
  if ((err = sem_wait(&pool->mutex))) {
    fprintf(stderr, "%d error in defer – sem_wait\n", err);
    return EXIT_FAILURE;
  }
  if (put(pool, runnable)) {
    fprintf(stderr, "Error in defer – fail at put\n");
    return EXIT_FAILURE;
  }
  if ((err = sem_post(&pool->mutex))) {
    fprintf(stderr, "%d error in defer – sem_post\n", err);
    return EXIT_FAILURE;
  }
  if ((err = sem_post(&pool->mutex_all))) {
    fprintf(stderr, "%d error in defer – sem_post\n", err);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//**********************************************************************************************************************//