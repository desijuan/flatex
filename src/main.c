#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "flatex.h"

#define DEST_FILE "a.tex"

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *dest;
  if ((dest = fopen(DEST_FILE, "w")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", DEST_FILE);
    return EXIT_FAILURE;
  }

  int level = 0;

  flat_it(argv[1], dest, &level);

  fclose(dest);

  if (errno != 0) {
    remove(DEST_FILE);
    return EXIT_FAILURE;
  }

  printf("Written to: %s\n", DEST_FILE);
  return EXIT_SUCCESS;
}
