#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)  // Stack of the cloned child

long long num_steps = 1000000000;
long long steps_per_thread;
double step;
double sum=0.0;
double *sums;

// function for cloned childs
static int sum_par(void *arg) {
  double x;
  int id_sum = *(int *)arg;
  long long init_step = id_sum * steps_per_thread;
  for (long long i = init_step; i < init_step + steps_per_thread; i++) {
    x = (i + .5) * step;
    sums[id_sum] += 4.0/(1. + x*x);
  }
  printf("sums[%d] = %f\n", id_sum, sums[id_sum]);
  return 0;
}

typedef void* Stack;

int main(void) {
  long long start_ts;
  long long stop_ts;
  float elapsed_time;
  struct timeval ts;
  double x, pi;
  long num_clones = sysconf(_SC_NPROCESSORS_ONLN);
  sums = malloc(num_clones * sizeof(double));

  step = 1./(double)num_steps;
  steps_per_thread = num_steps / num_clones;

  gettimeofday(&ts, NULL);
  start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo inicial
  Stack *child_stacks = malloc(STACK_SIZE * num_clones);
  int *idxs = malloc(num_clones * sizeof(int));

  // Allocate stacks for child processes
  for (int i = 0; i < num_clones; i++) {
    idxs[i] = i;
    child_stacks[i] = malloc(STACK_SIZE);
    if (child_stacks[i] == NULL)
      exit(-1);
  }

  for (int i = 0; i < num_clones; i++) {
    pid_t pid;
    Stack stackTop = child_stacks[i] + STACK_SIZE;  /* Assume stack grows downward */

    pid = clone(sum_par, stackTop, SIGCHLD | CLONE_VM, &idxs[i]);
    printf("clone %d\n", pid);
    if (pid == -1) { // error
      fprintf(stderr, "Clone failed\n");
      exit(-1);
    }
  }

  while (wait(NULL) != -1 || errno != ECHILD) {};

  for (int i = 0; i < num_clones; i++) {
    sum += sums[i];
    printf("sums[%d] = %f\n", i, sums[i]);
  }

  gettimeofday(&ts, NULL);
  stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo final
  elapsed_time = (float) (stop_ts - start_ts)/1000000.0;
  pi = sum * step;
  printf("El valor de PI es %1.12f\n", pi);
  printf("Tiempo = %2.2f segundos\n", elapsed_time);
  exit(0);
}
