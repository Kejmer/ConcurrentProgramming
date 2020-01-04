#include "future.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct factorial
{
  int step;
  // int limit;
  uint64_t result;
} factorial_t;

static void *part_factorial(void *arg, size_t argsz __attribute__((unused)),
                            size_t *retsz __attribute__((unused)))
{
  factorial_t *factorial = (factorial_t *) arg;
  factorial_t *new_factorial = malloc(sizeof(factorial_t));
  // new_factorial->limit = factorial->limit;
  new_factorial->step = factorial->step+1;
  new_factorial->result = new_factorial->step * factorial->result;
  return (void *)new_factorial;
}

int main() {
  int err;
  thread_pool_t pool;
  const int thread_num = 3;
  if ((err = thread_pool_init(&pool, thread_num))) {
    fprintf(stderr, "%d error – thread_pool_init\n", err);
    exit(-1);
  }

  int n;
  scanf("%d", &n);

  factorial_t *factorial = malloc(sizeof(factorial_t));
  factorial->step = 0;
  factorial->result = 1;

  future_t *future = malloc(n * sizeof(future_t));
  async(&pool, &future[0], (callable_t){.function = &part_factorial,
                                        .arg = factorial,
                                        .argsz = sizeof(factorial_t)});
int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *));
  for (int i = 1; i < n; i++) {
    if ((err = map(&pool, &future[i], &future[i-1], &part_factorial))) {
      fprintf(stderr, "%d error – map\n", err);
      exit(-1);
    }
  }

  factorial_t *result = (void *)await(&future[n-1]);
  printf("%ld\n", result->result);

  thread_pool_destroy(&pool);
  free(factorial);
  free(future);

  return 0;
}