#include "future.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct factorial
{
  int step;
  uint64_t result;
} factorial_t;

static void *part_factorial(void *arg, size_t argsz __attribute__((unused)),
                            size_t *retsz __attribute__((unused)))
{
  factorial_t *factorial = (factorial_t *) arg;
  factorial_t *new_factorial = malloc(sizeof(factorial_t));
  new_factorial->step = factorial->step + 3;
  new_factorial->result = factorial->step * factorial->result;
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

  if (n < 3) {
    int res = n == 0 ? 1 : n;
    printf("%d\n", res);
    return 0;
  }

  future_t *future = malloc(n * sizeof(future_t));
  if (future == NULL) {
    fprintf(stderr, "bad malloc\n");
  }
  factorial_t **factorial = malloc(sizeof(factorial_t *) * 3);
  if (factorial == NULL) {
    fprintf(stderr, "bad malloc\n");
  }
  for (int i = 0; i < 3; i++) {
    factorial[i] = malloc(sizeof(factorial_t));
    if (factorial[i] == NULL) {
      fprintf(stderr, "bad malloc\n");
    }
    factorial[i]->step = i+1;
    factorial[i]->result = 1;
    async(&pool, &future[i], (callable_t){.function = &part_factorial,
                                          .arg = factorial[i],
                                          .argsz = sizeof(factorial_t)});
  }

  for (int i = 3; i < n; i++) {
    if ((err = map(&pool, &future[i], &future[i-3], &part_factorial))) {
      fprintf(stderr, "%d error – map\n", err);
      exit(-1);
    }
  }

  uint64_t result = 1;
  for (int i = 1; i <= 3; i++) {
    factorial_t *fact = (void *)await(&future[n-i]);
    result *= fact->result;
    free(fact);
  }

  printf("%ld\n", result);

  thread_pool_destroy(&pool);
  for (int i = 0; i < 3; i++) {
    free(factorial[i]);
  }
  free(factorial);
  free(future);

  return 0;
}