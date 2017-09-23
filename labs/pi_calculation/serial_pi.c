#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_REPETITIONS 10
#define STEP 500
#define INIT_NUM_POINTS 500
#define MAX_NUM_POINTS 500000

int circle_count = 0;

int main(int argc, char const *argv[]) {
  srand(time(NULL));

  struct timeval ts;
  long long start_ts;
  long long stop_ts;
  float elapsed_time;
  float sum = 0;
  double pi;

  printf("num_points,pi,time\n");

  for (long num_points = INIT_NUM_POINTS; num_points <= MAX_NUM_POINTS; num_points += STEP) {
    for (size_t rep = 0; rep < NUM_REPETITIONS; rep++) {
      gettimeofday(&ts, NULL);
      start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Start time

      for (size_t i = 0; i < num_points; i++) {
        double x = (double) rand() / (RAND_MAX / 2) - 1;
        double y = (double) rand() / (RAND_MAX / 2) - 1;
        double magnitud = sqrt(x * x + y * y);

        if (magnitud <= 1)
        circle_count++;
      }


      pi = 4 * (float)circle_count / (double)num_points;

      gettimeofday(&ts, NULL);
      stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // End time
      sum += (float) (stop_ts - start_ts)/1000000.0;
      circle_count = 0;
    }

    elapsed_time = sum / NUM_REPETITIONS;
    printf("%ld,%f,%2.6f\n", num_points, pi, elapsed_time);
    sum = 0;
  }
  return 0;
}
