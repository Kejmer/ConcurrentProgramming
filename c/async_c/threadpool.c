#include <stdlib.h>
#include <stdio.h>
#include "threadpool.h"

typedef struct async_arg
{
  thread_pool_t *pool;
  int thread_num;
  runnable_t runnable;
} async_arg_t;


int thread_pool_init(thread_pool_t *pool, size_t num_threads)
{
  pool->num_threads = num_threads;
  pool->thread = malloc(num_threads * sizeof(pthread_t));
  if (pool->thread == NULL) {
    fprintf(stderr, "Error in thread_pool_init – malloc\n");
    return EXIT_FAILURE;
  }

  pool->working = malloc(num_threads * sizeof(enum status));
  if (pool->working == NULL) {
    fprintf(stderr, "Error in thread_pool_init – malloc\n");
    free(pool->thread);
    return EXIT_FAILURE;
  }

  if (pthread_attr_init(&pool->attr)) {
    fprintf(stderr, "Error in thread_pool_init – pthread_attr_init\n");
    free(pool->thread);
    free(pool->working);
    return EXIT_FAILURE;
  }

  if (sem_init(&pool->mutex_all, 0, num_threads)) {
    fprintf(stderr, "Error in thread_pool_init – sem_init\n");
    free(pool->thread);
    free(pool->working);
    if (pthread_attr_destroy(&pool->attr)) {
      fprintf(stderr, "Error in pthread_attr_destoy\n");
    }
    return EXIT_FAILURE;
  }

  if (sem_init(&pool->mutex, 0, 1)) {
    fprintf(stderr, "Error in thread_pool_init – sem_init\n");
    free(pool->thread);
    free(pool->working);
    if (pthread_attr_destroy(&pool->attr)) {
      fprintf(stderr, "Error in pthread_attr_destoy\n");
    }
    if (sem_destroy(&pool->mutex_all)) {
      fprintf(stderr, "Error in sem_destroy\n");
    }
    return EXIT_FAILURE;
  }

  //Zapewnienie że pamięc zarezerwowana na wątek zostanie zwolniona po zakończeniu
  if (pthread_attr_setdetachstate(&pool->attr, PTHREAD_CREATE_DETACHED)) {
    fprintf(stderr, "Error in thread_pool_init – sem_init\n");
    free(pool->thread);
    free(pool->working);
    if (pthread_attr_destroy(&pool->attr)) {
      fprintf(stderr, "Error in pthread_attr_destoy\n");
    }
    if (sem_destroy(&pool->mutex_all)) {
      fprintf(stderr, "Error in sem_destroy\n");
    }
    if (sem_destroy(&pool->mutex)) {
      fprintf(stderr, "Error in sem_destroy\n");
    }
  }

  for (unsigned i = 0; i < num_threads; i++) {
    pool->working[i] = Free;
  }

  return EXIT_SUCCESS;
}

void thread_pool_destroy(thread_pool_t *pool)
{
  free(pool->thread);
  free(pool->working);
  if (pthread_attr_destroy(&pool->attr)) {
    fprintf(stderr, "Error in thread_pool_destroy – pthread_attr_destoy\n");
  }
  if (sem_destroy(&pool->mutex_all)) {
    fprintf(stderr, "Error in thread_pool_destroy – sem_destroy\n");
  }
  if (sem_destroy(&pool->mutex)) {
    fprintf(stderr, "Error in thread_pool_destroy – sem_destroy\n");
  }
}

static void make_async(thread_pool_t *pool, int thread_num, runnable_t runnable)
{
  (* runnable.function)(runnable.arg, runnable.argsz);
  // free(runnable.arg);

  if (sem_wait(&pool->mutex)) {
    fprintf(stderr, "Error in async funkction – sem_wait\n");
    exit(EXIT_FAILURE);
  }
  pool->working[thread_num] = Free;
  sem_post(&pool->mutex_all);
  sem_post(&pool->mutex);
}

static void *unwrap(void *arg_temp) {
  async_arg_t arg = *((async_arg_t *) arg_temp);
  free(arg_temp);
  make_async(arg.pool, arg.thread_num, arg.runnable);
  return EXIT_SUCCESS;
}

static int delay(thread_pool_t *pool, int thread_num, runnable_t runnable)
{
  async_arg_t *arg = malloc(sizeof(async_arg_t));
  if (arg == NULL) {
    fprintf(stderr, "Error in defer – malloc\n"); //bo jest to funkcja wywoływane przez defer
    return EXIT_FAILURE;
  }
  arg->pool = pool;
  arg->thread_num = thread_num;
  arg->runnable = runnable;
  if (pthread_create(&pool->thread[thread_num], &pool->attr, &unwrap, arg)) {
    fprintf(stderr, "Error in defer – pthread_create\n");
    free(arg);
    // free(runnable.arg);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int defer(thread_pool_t *pool, runnable_t runnable)
{
  //Tworzymy kopię runnable
  // runnable_t runnable_copy = runnable;
  // runnable_copy.arg = malloc(runnable.argsz);
  // if (runnable_copy.arg == NULL) {
  //   return EXIT_FAILURE;
  // }
  // memcpy(runnable_copy.arg, runnable.arg, runnable.argsz);

  //Sprawdzamy czy jakikolwiek wątek jest wolny
  if (sem_wait(&pool->mutex_all)) {
    fprintf(stderr, "Error in defer – sem_wait\n");
    return EXIT_FAILURE;
  }
  //Szukamy pierwszego wolnego wątku
  if (sem_wait(&pool->mutex)) {
    fprintf(stderr, "Error in defer – sem_wait\n");
    return EXIT_FAILURE;
  }
  int thread_num = -1;
  for (unsigned i = 0; i < pool->num_threads; i++) {
    if (pool->working[i] == Free) {
      thread_num = i;
      pool->working[i] = Working;
      break;
    }
  }
  sem_post(&pool->mutex);

  if (thread_num == -1) {
    fprintf(stderr, "Error in defer – no free threads\n");
    sem_post(&pool->mutex_all);
    return EXIT_FAILURE;
  }

  if (delay(pool, thread_num, runnable)) {
    fprintf(stderr, "Error in defer – pthread_create\n");
    pool->working[thread_num] = Free;
    sem_post(&pool->mutex_all);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
