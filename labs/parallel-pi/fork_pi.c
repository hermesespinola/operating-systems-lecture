#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>

long long num_steps = 1000000000;
long long steps_per_thread;
double step;
double sum=0.0;

int main(void) {
  int parent_pid = getpid();
  pid_t pid;

  long long start_ts;
  long long stop_ts;
  float elapsed_time;
  struct timeval ts;
  double x, pi;
  long num_forks = sysconf(_SC_NPROCESSORS_ONLN);

  /*
   * We'll name our shared memory segment
   * "5678".
   */
  key_t key = 5423;
  int shmid;

  step = 1./(double)num_steps;
  steps_per_thread = num_steps / num_forks;
  double *sums;

  gettimeofday(&ts, NULL);
  start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo inicial
  /*
   * Create the segment.
   */
  shmid = shmget(key, num_forks * sizeof(double), IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("shmget");
    exit(1);
  }

  /*
  * Now we attach the segment to our data space (for the parent).
  */
  if ((sums = shmat(shmid, NULL, 0)) == (double *) -1) {
    perror("shmat");
    exit(1);
  }

  for (int id_sum = 0; id_sum < num_forks; id_sum++) {
    pid = fork();
    if (pid < 0) { // error
      fprintf(stderr, "Fork failed\n");
      exit(-1);
    } else if (pid == 0) { // Child process
      double x;
      long long init_step = id_sum * steps_per_thread;

      shmid = shmget(key, num_forks * sizeof(double), IPC_CREAT);
      sums = (double *) shmat(shmid, 0, 0);
      sums[id_sum] = 0;
      for (long long i = init_step; i < init_step + steps_per_thread; i++) {
        x = (i + .5) * step;
        sums[id_sum] += 4.0/(1. + x*x);
      }
      exit(0);
    }
  }

  while (wait(NULL) != -1 || errno != ECHILD) {};

  for (int i = 0; i < num_forks; i++) sum += sums[i];

  gettimeofday(&ts, NULL);
  stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo final
  elapsed_time = (float) (stop_ts - start_ts)/1000000.0;
  pi = sum * step;
  printf("El valor de PI es %1.12f\n", pi);
  printf("Tiempo = %2.2f segundos\n", elapsed_time);
  exit(0);
}
