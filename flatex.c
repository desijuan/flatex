#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEST_FILE "a.tex"
#define MAX_DEPTH 7

bool isinput(char *line) {

  char *input_ptr = strstr(line, "\\input{");
  char *include_ptr = strstr(line, "\\include{");
  char *comment_ptr = strchr(line, '%');

  if ((input_ptr && (!comment_ptr || (input_ptr < comment_ptr))) ||
      (include_ptr && (!comment_ptr || (include_ptr < comment_ptr))))
    return true;
  else
    return false;
}

void flatit(char *source_path, FILE *dest, int *plevel) {

  FILE *source;
  if ((source = fopen(source_path, "r")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", source_path);
    return;
  }

  printf(" %*s-> %s\n", 2*(*plevel), "", source_path);

  ++*plevel;

  size_t len = 1024;
  char *line = malloc(len);
  while (getline(&line, &len, source) != -1) {

    if (errno != 0)
      break;

    if (!isinput(line))
      fputs(line, dest);
    else {

      if (*plevel+1 > MAX_DEPTH) {
        fprintf(stderr, "Error: Recursion limit exceeded\n");
        errno = 1;
        break;
      }

      char *start = strchr(line, '{') + 1;
      char *end = strchr(start, '}');

      int i;
      for (i = 0; start + i < end; i++)
        line[i] = *(start + i);
      line[i] = '\0';

      strcat(line, ".tex");

      flatit(line, dest, plevel);
    }
  }

  fclose(source);
  free(line);
  --*plevel;
}

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

  flatit(argv[1], dest, &level);

  fclose(dest);

  if (errno != 0) {
    remove(DEST_FILE);
    return EXIT_FAILURE;
  }

  printf("Written to: %s\n", DEST_FILE);
  return EXIT_SUCCESS;
}
