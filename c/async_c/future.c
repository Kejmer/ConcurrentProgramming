#include "future.h"

// typedef struct callable {
//   void *(*function)(void *, size_t, size_t *);
//   void *arg;
//   size_t argsz;
// } callable_t;

// typedef struct future {
//   void *result;
//   sem_t lock;
//
// } future_t;

typedef struct future_arg
{
  callable_t callable;
  future_t *future;
} future_arg_t;

typedef void *(*function_t)(void *);

void wrap(void *arg_temp)
{
  future_arg_t *arg = ((future_arg_t*)arg_temp);
  callable_t callable = arg->callable;
  future_t *future = arg->future;

  size_t *retsz;
  future->result = (*callable.function)(callable.arg, argsz, retsz);
  sem_post(&pool->lock);
}

int async(thread_pool_t *pool, future_t *future, callable_t callable)
{
  sem_init(&future->lock, 0, 0);
  future_arg_t *arg = malloc(sizeof(future_arg_t));
  arg->callable = callable;
  arg->callable.arg = malloc(sizeof(callable.argsz));
  *arg->callable.arg = *callable.arg;
  arg->future = future;

  defer(pool, (runnable_t){.function = wrap
                           .arg = arg
                           .argsz = sizeof(future_arg_t)});
  return 0;
}

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *))
{

  async(pool, future, )
  return 0;
}

void *await(future_t *future)
{
  sem_wait(&future->lock);
  sem_destroy(&future->lock);
  return future->result;
}
