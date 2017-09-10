/**
 * Wannabe interactive shell.
 */
 
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 100

typedef char* String;
typedef int PID;

String* split(String str) {
  int spaces = 0;
  for (int i = 1; i < strlen(str); i++) {
    if (str[i-1] == ' ') spaces++;
  }

  int i = 0;
  String *array = (String*) calloc(spaces + 2, sizeof(String));
  array[i] = strtok(str, " ");

  while (array[i] != NULL) array[++i] = strtok (NULL, " ");

  // Remove new line character
  String last = array[i-1];
  last[strlen(last)-1] = 0;

  return array;
}

int main(void) {
  char inputstring[MAX_LINE/2 + 1];
  char cwd[1024];
  while (1) {
    getcwd(cwd, sizeof(cwd));
    printf("%s\n⤷ ", cwd);

    fgets(inputstring, MAX_LINE/2 + 1, stdin);
    if (strcmp(inputstring, "exit\n") == 0) return 0;

    // Fork a child process after user input
    PID pid = fork();
    if (pid < 0) {
      fprintf(stderr, "Error forking: %s\n", strerror(errno));
      return 1;
    } else if (pid == 0) { // child process
      // split arguments
      String *args = split(inputstring);

      if (strcmp(args[0], "cd") == 0) {
        chdir(args[1]);
      } else {
        // execute the command introduced
        if(execvp(args[0], args) == -1)
          fprintf(stderr, "No command '%s' found.\n", args[0]);
        free(args);
      }
    } else { // parent process
      wait(NULL);
    }
  }
  return 0;
}
