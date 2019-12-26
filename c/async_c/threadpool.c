#include "threadpool.h"

// INFORMACJA POMOCNICZA O STRUKTURACH
// typedef struct runnable {
//   void (*function)(void *, size_t);
//   void *arg;
//   size_t argsz;
// } runnable_t;

// typedef struct thread_pool {
//   sem_t *mutex;
//   pthread_t *thread;
//   pthread_attr_t *attr;
// } thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t num_threads)

{
  return 0;
}

void thread_pool_destroy(struct thread_pool *pool)
{

}

int defer(struct thread_pool *pool, runnable_t runnable)
{
  return 0;
}
