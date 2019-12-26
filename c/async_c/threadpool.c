#include "threadpool.h"

// INFORMACJA POMOCNICZA O STRUKTURACH

enum status {Working, Free};

// typedef struct runnable {
//   void (*function)(void *, size_t);
//   void *arg;
//   size_t argsz;
// } runnable_t;

// typedef struct thread_pool {
//   pthread_t *thread;        //Tablica wątków
//   pthread_attr_t *attr;
//   sem_t mutex_all;          //Semafor dysponuje pool_size zezwoleniami
//   status *working;          //Stan poszczególnych wątków
//   size_t num_threads;       //Informacja o tym ile wątków jest zarezerwowanych w pamięci
// } thread_pool_t;


int thread_pool_init(thread_pool_t *pool, size_t num_threads)
{
  pool->num_threads = num_threads;
  pool->thread = malloc(num_threads * sizeof(pthread_t));
  if (pool->thread == NULL) return -1;

  pool->working = malloc(num_threads * sizeof(status));
  if (pool->working == NULL) {
    free(pool->thread);
    return -1;
  }

  if (pthread_attr_init(&pool->attr) != 0 ) {
    free(pool->thread);
    free(pool->working);
    return -1;
  }

  if (sem_init(&pool->mutex_all) != 0) {
    free(pool->thread);
    free(pool->working);
    // pthread_attr_destoy(pool->attr);
    return -1;
  }
  return 0;
}

void thread_pool_destroy(struct thread_pool *pool)
{

}

int defer(struct thread_pool *pool, runnable_t runnable)
{
  return 0;
}
