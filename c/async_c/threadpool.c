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
//   sem_t mutex;
//   sem_t mutex_all;          //Semafor dysponuje pool_size zezwoleniami
//   status *working;          //Stan poszczególnych wątków
//   size_t num_threads;       //Informacja o tym ile wątków jest zarezerwowanych w pamięci
// } thread_pool_t;

typedef struct async_arg
{
  thread_pool *pool;
  int num_threads;
  runnable_t runnable;
} async_arg_t;


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

  if (sem_init(&pool->mutex_all, 0, num_threads)) {
    fprintf(stderr, "Error in thread_pool_init – sem_init\n");
    free(pool->thread);
    free(pool->working);
    if (pthread_attr_destoy(pool->attr)) {
      fprintf(stderr, "Error in pthread_attr_destoy\n");
    }
    return -1;
  }

  if (sem_init(&pool->mutex, 0, 1)) {
    fprintf(stderr, "Error in thread_pool_init – sem_init\n");
    free(pool->thread);
    free(pool->working);
    if (pthread_attr_destoy(pool->attr)) {
      fprintf(stderr, "Error in pthread_attr_destoy\n");
    }
    if (sem_destroy(&pool->mutex_all)) {
      fprintf(stderr, "Error in sem_destroy\n");
    }
    return -1;
  }

  return 0;
}

void thread_pool_destroy(thread_pool_t *pool)
{
  free(pool->thread);
  free(pool->working);
  if (pthread_attr_destoy(&pool->attr)) {
    fprintf(stderr, "Error in thread_pool_destroy – pthread_attr_destoy\n");
  }
  if (sem_destroy(&pool->mutex_all)) {
    fprintf(stderr, "Error in thread_pool_destroy – sem_destroy\n");
  }
  if (sem_destroy(&pool->mutex)) {
    fprintf(stderr, "Error in thread_pool_destroy – sem_destroy\n");
  }
}

static void make_async(thread_pool_t *pool, int thread_num, runnable_t *runnable)
{
  //Tutaj odpalam funkcję
  (* runnable->funkction)(runnable->arg, runnable->argsz);

  if (sem_wait(&pool->mutex)) {
    fprintf(stderr, "Error in async funkction – sem_wait\n");
  }
  pool->working[thread_num] = Free;
  sem_post(&pool->mutex_all);
  sem_post(&pool->mutex);
}

static void unwrap(void *arg_temp) {
  async_arg_t *arg = ((*async_arg_t) arg_temp);
  free(arg_temp);
  make_async(arg->pool, arg->thread_num, arg->runnable);
}

static int delay(thread_pool_t *pool, int thread_num, runnable_t *runnable)
{
  async_arg_t arg = malloc(sizeof(async_arg_t));
  if (arg == NULL) {
    fprintf(stderr, "Error in defer – malloc\n"); //bo jest to funkcja wywoływane przez defer
    return -1;
  }
  arg->pool = pool;
  arg->thread_num = thread_num;
  arg->runnable = runnable;
  if (pthread_create(pool->threads[thread_num], &pool->attr, unwrap, arg)) {
    fprintf(stderr, "Error in defer – pthread_create\n");
    free(arg);
    return -1;
  }
  return 0;
}

int defer(thread_pool_t *pool, runnable_t runnable)
{
  //Sprawdzamy czy jakikolwiek wątek jest wolny
  if (sem_wait(&pool->mutex_all)) {
    fprintf(stderr, "Error in defer – sem_wait\n");
    return -1;
  }
  //Szukamy pierwszego wolnego wątku
  if (sem_wait(&pool->mutex)) {
    fprintf(stderr, "Error in defer – sem_wait\n");
    return -1;
  }
  int thread_num = -1;
  for (int i = 0; i < pool->num_threads; i++) {
    if (pool->working[i] == Free) {
      thread_num = i;
      pool->working[i] = Working;
      break;
    }
  }
  sem_post(&pool->mutex);

  if (thread_num == -1) {
    fprintf(stderr, "Error in defer – no free threads\n");
    sem_post(&pool->mutex_all)
    return -1;
  }

  if (delay(pool, thread_num, &runnable)) {
    fprintf(stderr, "Error in defer – pthread_create\n");
    pool->working[i] = Free;
    sem_post(&pool->mutex_all);
    return -1;
  }
  return 0;
}
