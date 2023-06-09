#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEST_FILE "a.tex"

void flatit(FILE *source, FILE *dest) {

  size_t len = 0;
  char *line = NULL;
  while (getline(&line, &len, source) != -1) {

    char *input_ptr = strstr(line, "\\input{");
    char *comment_ptr = strchr(line, '%');

    if (!input_ptr || (comment_ptr && (comment_ptr < input_ptr)))
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

      char ch;
      while ((ch = getc(input_file)) != EOF) {
        putc(ch, dest);
      }

      fclose(input_file);
    }
  }

  free(line);
}

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("%s\n", "Usage: flatex <file>");
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
