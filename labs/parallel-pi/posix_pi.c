#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

long long num_steps = 1000000000;
long long steps_per_thread;
double step;
double sum=0.0;
pthread_mutex_t lock;

void* sum_par(void *ptr);

int main(int argc, char* argv[]) {
  long long start_ts;
  long long stop_ts;
  float elapsed_time;
  struct timeval ts;
  double x, pi;
  long num_threads = sysconf(_SC_NPROCESSORS_ONLN) * 2;

  step = 1./(double)num_steps;
  steps_per_thread = num_steps / num_threads;
  pthread_mutex_init(&lock, NULL);

  pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
  long long *init_steps = calloc(num_threads, sizeof(long long));

  printf("Spawn %ld threads.\n", num_threads);

  gettimeofday(&ts, NULL);
  start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo inicial
  for (int i = 0; i < num_threads; i++) {
    init_steps[i] = i * steps_per_thread;
    int r_code = pthread_create(&threads[i], NULL, sum_par, &init_steps[i]);
    if (r_code) {
      fprintf(stderr, "Error spawning thread, return code: %d\n", r_code);
      exit(EXIT_FAILURE);
    }
  }

  /* Wait till threads are complete before main continues. Unless we
    wait we run the risk of executing an exit which will terminate
    the process and all threads before the threads have completed.*/

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  
  gettimeofday(&ts, NULL);
  stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo final
  elapsed_time = (float) (stop_ts - start_ts)/1000000.0;
  pi = sum * step;
  printf("El valor de PI es %1.12f\n",pi);
  printf("Tiempo = %2.2f segundos\n",elapsed_time);
  return 0;

  for (int i = 0; i < num_steps; i++) {
    x = (i + .5)*step;
    sum = sum + 4.0/(1.+ x*x);
  }
}

void* sum_par(void *ptr) {
  long long init_step = *(long long*)ptr;
  double x, partial_sum;
  for (long long i = init_step; i < init_step + steps_per_thread; i++) {
    x = (i + .5) * step;
    partial_sum = partial_sum + 4.0/(1. + x*x);
  }
  pthread_mutex_lock(&lock);
  sum += partial_sum;
  pthread_mutex_unlock(&lock);
  return 0;
}
