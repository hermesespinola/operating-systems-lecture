#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

int main (int argc, char *argv[]){
  struct passwd *p;
  char *name, *filename;

  if (argc == 1) {
    fprintf(stderr, "%s\n", "chown: missing operand");
    return EXIT_FAILURE;
  }

  if (argc > 1) {
    name = argv[1];
    if (argc > 2) {
      filename = argv[2];
    } else {
      fprintf(stderr, "chown: missing operand after '%s'\n", name);
      return EXIT_FAILURE;
    }
  }

  if ((p = getpwnam(name)) == NULL) {
    fprintf(stderr, "chown: invalid user: '%s'\n", name);
    return EXIT_FAILURE;
  }

  if (chown(filename, p->pw_uid, p->pw_gid) == -1) {
    fprintf(stderr, "chown: cannot access '%s': %s\n", filename, strerror(errno));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
