#include <semaphore.h>
#include <unistd.h>
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>


typedef struct matrix
{
  int width;
  int *cells;
  int *time;
  int *rows;
  sem_t *rows_sem;
} matrix_t;

typedef struct query
{
  matrix_t *matrix;
  int cellno;
} query_t;

static void count_cell(void *args, size_t argsz __attribute__((unused)))
{
  int err;
  query_t *query = ((query_t *) args);
  matrix_t *matrix = query->matrix;
  int i = query->cellno;
  int row = (i / matrix->width);
  sleep(matrix->time[i]/1000);
  if ((err = sem_wait(&matrix->rows_sem[row]))) {
    fprintf(stderr, "%d error in count_cell – sem_wait\n", err);
    exit(-1);
  }
  matrix->rows[row] += matrix->cells[i];
  if ((err = sem_post(&matrix->rows_sem[row]))) {
    fprintf(stderr, "%d error in count_cell – sem_post\n", err);
    exit(-1);
  }
}

int main()
{
  matrix_t matrix;
  int n, k, size;
  scanf("%d%d", &k, &n);
  matrix.width = n;
  size = n*k;
  matrix.rows = malloc(sizeof(int) * k);
  matrix.time = malloc(sizeof(int) * size);
  matrix.cells = malloc(sizeof(int) * size);
  matrix.rows_sem = malloc(sizeof(sem_t) * k);

  for (int i=0; i<k; i++) {
    sem_init(&matrix.rows_sem[i], 0, 1);
  }

  int val, time;
  for (int i=0; i<size; i++) {
    scanf("%d%d", &val, &time);
    matrix.cells[i] = val;
    matrix.time[i] = time;
  }

  thread_pool_t pool;
  if (thread_pool_init(&pool, 4)) {
    exit(-1);
  }

  query_t *queries = malloc(sizeof(query_t) * size);
  runnable_t runnable;
  runnable.function = &count_cell;
  runnable.argsz = sizeof(query_t);
  for (int i=0; i<size; i++) {
    queries[i].matrix = &matrix;
    queries[i].cellno = i;
    runnable.arg = (void *)&queries[i];
    if (defer(&pool, runnable)) {
      exit(-1);
    }
  }

  thread_pool_destroy(&pool);
  free(queries);

  for(int i=0; i<k; i++) {
    printf("%d\n", matrix.rows[i]);
  }

  for(int i=0; i<k; i++) {
    sem_destroy(&matrix.rows_sem[i]);
  }
  free(matrix.rows);
  free(matrix.time);
  free(matrix.cells);
  free(matrix.rows_sem);

}