#include "future.h"
#include <stdlib.h>

typedef void *(*function_t)(void *, size_t, size_t*);

typedef struct future_arg
{
  callable_t callable;
  future_t *future;
} future_arg_t;

typedef struct map_arg
{
  future_t *future;
  future_t *from;
  function_t function;
} map_arg_t;

static void user_receive(void *arg_temp, size_t argsz __attribute__((unused)))
{
  future_arg_t *arg = ((future_arg_t*) arg_temp);
  callable_t callable = arg->callable;
  future_t *future = arg->future;
  free(arg_temp);

  future->result = (*callable.function)(callable.arg, callable.argsz, &future->retsz);
  // free(arg->callable.arg);
  sem_post(&future->lock);
}

static void system_receive(void *arg_temp, size_t argsz __attribute__((unused)))
{
  map_arg_t *arg = ((map_arg_t*) arg_temp);
  future_t *future = arg->future;
  future_t *from = arg->from;
  function_t function = arg->function;
  free(arg_temp);

  void *fut_arg = await(from);
  future->result = (*function)(fut_arg, from->retsz, &future->retsz);
  free(from->result);
  sem_post(&future->lock);
}

int async(thread_pool_t *pool, future_t *future, callable_t callable)
{
  sem_init(&future->lock, 0, 0);
  future_arg_t *arg = malloc(sizeof(future_arg_t));
  arg->callable = callable;
  // arg->callable.arg = malloc(sizeof(callable.argsz));
  // *arg->callable.arg = *callable.arg;
  arg->future = future;

  defer(pool, (runnable_t){.function = &user_receive,
                           .arg = arg,
                           .argsz = sizeof(future_arg_t)});
  return 0;
}

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *))
{
  sem_init(&future->lock, 0, 0);
  map_arg_t *arg = malloc(sizeof(map_arg_t));
  arg->future = future;
  arg->from = from;
  arg->function = function;

  defer(pool, (runnable_t){.function = &system_receive,
                           .arg = arg,
                           .argsz = sizeof(map_arg_t)});
  return 0;
}

void *await(future_t *future)
{
  sem_wait(&future->lock);
  sem_destroy(&future->lock);
  return future->result;
}
