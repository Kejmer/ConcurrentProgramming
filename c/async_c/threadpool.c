#include "threadpool.h"

// INFORMACJA POMOCNICZA O STRUKTURACH

// enum status {Working, Free};

// typedef struct runnable {
//   void (*function)(void *, size_t);
//   void *arg;
//   size_t argsz;
// } runnable_t;

// typedef struct thread_pool {
//   pthread_t *thread;        //Tablica wątków
//   pthread_attr_t *attr;
//   sem_t mutex;              //Semafor dysponuje pool_size zezwoleniami
//   status *working;          //Stan poszczególnych wątków
//   size_t num_threads;       //Informacja o tym ile wątków jest zarezerwowanych w pamięci
// } thread_pool_t;


int thread_pool_init(thread_pool_t *pool, size_t num_threads)
{
  pool->num_threads = num_threads;
  pool->thread = malloc(num_threads * sizeof(pthread_t));
  if (pool->thread == NULL) {
    fprintf(stderr, "Error in thread_pool_init – malloc\n");
    return -1;
  }

  pool->working = malloc(num_threads * sizeof(status));
  if (pool->working == NULL) {
    fprintf(stderr, "Error in thread_pool_init – malloc\n");
    free(pool->thread);
    return -1;
  }

  if (pthread_attr_init(&pool->attr)) {
    fprintf(stderr, "Error in thread_pool_init – pthread_attr_init\n");
    free(pool->thread);
    free(pool->working);
    return -1;
  }

  if (sem_init(&pool->mutex)) {
    fprintf(stderr, "Error in thread_pool_init – sem_init\n");
    free(pool->thread);
    free(pool->working);
    if (pthread_attr_destoy(pool->attr)) {
      fprintf(stderr, "Error in pthread_attr_destoy\n");
    }
    return -1;
  }
  return 0;
}

void thread_pool_destroy(struct thread_pool *pool)
{
  free(pool->thread);
  free(pool->working);
  if (pthread_attr_destoy(&pool->attr)) {
    fprintf(stderr, "Error in thread_pool_destroy – pthread_attr_destoy\n");
  }

  if (sem_destroy(&pool->mutex)) {
    fprintf(stderr, "Error in thread_pool_destroy – sem_destroy\n");
  }
}

int defer(struct thread_pool *pool, runnable_t runnable)
{
  //Sprawdzamy czy jakikolwiek wątek jest wolny
  if (sem_wait(pool->mutex)) {
    fprintf(stderr, "Error in defer – pool->mutex wait\n");
    return -1;
  }
  //Szukamy pierwszego wolnego wątku
  int thread_no = -1;
  for (int i = 0; i < pool->num_threads; i++) {
    if (pool->working[i] == Free) {
      thread_no = i;
      pool->working[i] = Working;
      break;
    }
  }

  if (thread_no == -1) {
    fprintf(stderr, "Error in defer – no free threads\n");
    return -1;
  }

  if (pthread_create(&pool->thread[thread_no], &pool->attr, runnable.function, runnable.arg)) {
    fprintf(stderr, "Error in defer – pthread_create\n");
    pool->working[i] = Free;
    sem_post(pool->mutex);
    return -1;
  }

  return 0;
}
