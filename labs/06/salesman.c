#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */

#define MAX_COOKIES 100
#define SOME_REALLY_BIG_NUMBER 1000000

int main(int argc, char const *argv[]) {
  int i;                        /*      loop variables          */
  key_t shmkey;                 /*      shared memory key       */
  int shmid;                    /*      shared memory id        */
  sem_t *sem;                   /*      synch semaphore         *//*shared */
  pid_t pid;                    /*      fork pid                */
  int *p;                       /*      shared variable         *//*shared */
  unsigned int n;               /*      fork count              */
  unsigned int value = 2;       /*      semaphore value         */

  /* initialize a shared variable in shared memory */
  shmkey = ftok ("/dev/null", 12);       /* valid directory name and a number */
  printf ("shmkey for p = %d\n", shmkey);
  shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);
  if (shmid < 0){                           /* shared memory error check */
      perror ("shmget\n");
      exit (1);
  }

  p = (int *) shmat (shmid, NULL, 0);   /* attach p to shared memory */
  if (*p == 0) {
    *p = MAX_COOKIES;
  }
  printf ("we have %d cookies in store.\n", *p);

  /* initialize semaphores for shared processes */
  sem = sem_open ("pSem", O_CREAT | O_EXCL, 0644, value);
  /* name of semaphore is "pSem", semaphore is reached using this name */
  sem_unlink ("pSem");
  /* unlink prevents the semaphore existing forever */
  /* if a crash occurs during the execution         */
  printf ("semaphores initialized.\n\n");

  while (*p > 0) {
    int amount;
    printf("How many cookies do you want?");
    scanf("%d", &amount);
    sem_wait (sem);
    printf ("I'm checking the cookie store...\n");

    if (amount >= SOME_REALLY_BIG_NUMBER/2 && amount < SOME_REALLY_BIG_NUMBER) {
      printf("That's not a big enough number! try again.\n");
    }
    else if (amount >= SOME_REALLY_BIG_NUMBER) {
      // shared memory detach
      shmdt (p);
      shmctl (shmid, IPC_RMID, 0);

      // cleanup semaphores
      sem_destroy (sem);
      exit(0);
    }
    else if (*p >= amount) {
      printf("Now you have %d more cookies than before, lucky you!\n", amount);
      *p -= amount;
    }
    else {
      printf("We have not enough cookies...\n");
    }
    // Signal operation
    sem_post (sem);

    printf ("We have %d cookies! Type some really big number to exit.\n", *p);
  }

  return 0;
}
