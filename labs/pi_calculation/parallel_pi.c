/* https://raw.githubusercontent.com/hermesespinola/operating-systems-lecture/master/labs/pi_calculation/parallel_pi.png */
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_REPETITIONS 100
#define NUM_POINTS 10000000
#define MAX_THREADS 32

int circle_count = 0;
pthread_mutex_t lock;
int num_points_per_thread;

void* circle_count_range(void *init_seed) {
  unsigned seed = (*(unsigned *)init_seed) | pthread_self();
  int count = 0;
  for (int i = 0; i < num_points_per_thread; i++) {
    double x = (double) rand_r(&seed) / (RAND_MAX / 2) - 1;
    double y = (double) rand_r(&seed) / (RAND_MAX / 2) - 1;
    double magnitud = sqrt(x * x + y * y);

    if (magnitud <= 1)
      count++;
  }

  pthread_mutex_lock(&lock);
  circle_count += count;
  pthread_mutex_unlock(&lock);
}

int main(int argc, char const *argv[]) {
  pthread_t *threads = malloc(MAX_THREADS * sizeof(pthread_t));
  struct timeval ts;
  long long start_ts;
  long long stop_ts;
  float elapsed_time;
  float sum = 0;
  double pi;

  printf("num_threads,pi,time\n");

  for (long num_threads = 1; num_threads <= MAX_THREADS; num_threads += 1) {
    for (size_t rep = 0; rep < NUM_REPETITIONS; rep++) {

      num_points_per_thread = NUM_POINTS / num_threads;
      gettimeofday(&ts, NULL);
      start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Start time

      for (int i = 0; i < num_threads; i++) {
        int r_code = pthread_create(&threads[i], NULL, circle_count_range, &num_points_per_thread);
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

      pi = 4 * (float)circle_count / (double) NUM_POINTS;

      gettimeofday(&ts, NULL);
      stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // End time
      sum += (float) (stop_ts - start_ts)/1000000.0;
      circle_count = 0;

    }

    elapsed_time = sum / NUM_REPETITIONS;
    printf("%ld,%f,%2.15f\n", num_threads, pi, elapsed_time);
    sum = 0;
  }
  return 0;
}
