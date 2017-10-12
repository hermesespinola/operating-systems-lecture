#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NITER 500

int cookies = 0;

void * buyCookie(void *a) {
  for (size_t i = 0; i < NITER; i++) {
    cookies += 1;
  }
}

int main(int argc, char const *argv[]) {
  pthread_t salesman1, salesman2;

    if(pthread_create(&salesman1, NULL, buyCookie, NULL)) {
      printf("ERROR creating thread 1");
      exit(1);
    }

    if(pthread_create(&salesman2, NULL, buyCookie, NULL)) {
      printf("ERROR creating thread 2");
      exit(1);
    }

    if(pthread_join(salesman1, NULL)) {
      printf("ERROR joining salesman 1");
      exit(1);
    }

    if(pthread_join(salesman2, NULL)) {
      printf("ERROR joining salesman 2");
      exit(1);
    }

    if (cookies < 2 * NITER)
        printf("There are %d cookies, who ate %d of them?\n", cookies, 2*NITER - cookies);
    else
        printf("There are %d cookies\n", cookies);

    pthread_exit(NULL);
  return 0;
}
