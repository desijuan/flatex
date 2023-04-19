#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEST_FILE "a.tex"

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

void flatit(FILE *source, FILE *dest) {

  size_t len = 1024;
  char *line = malloc(len);
  while (getline(&line, &len, source) != -1) {

    if (errno != 0)
      break;

    if (!isinput(line))
      fputs(line, dest);
    else {

      char *start = strchr(line, '{') + 1;
      char *end = strchr(start, '}');

      int i;
      for (i = 0; start + i < end; i++)
        line[i] = *(start + i);
      line[i] = '\0';

      strcat(line, ".tex");

      FILE *input_file;

      if ((input_file = fopen(line, "r")) == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", line);
        break;
      }

      printf(" -> %s\n", line);

      flatit(input_file, dest);

      fclose(input_file);
    }
  }

  free(line);
}

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *source_path = argv[1];

  FILE *source;
  FILE *dest;

  if ((source = fopen(source_path, "r")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", source_path);
    return EXIT_FAILURE;
  }

  if ((dest = fopen(DEST_FILE, "w")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", DEST_FILE);
    fclose(source);
    return EXIT_FAILURE;
  }

  printf("Input file: %s\n", source_path);

  flatit(source, dest);

  fclose(dest);
  fclose(source);

  if (errno != 0) {
    remove(DEST_FILE);
    return EXIT_FAILURE;
  }

  printf("Written to: %s\n", DEST_FILE);
  return EXIT_SUCCESS;
}
