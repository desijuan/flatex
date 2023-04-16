#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEST_FILE "a.tex"

void getpath(char *line) {

  char *start = strstr(line, "{") + 1;
  char *end = strstr(start, "}");

  int i = 0;
  while (start + i < end) {
    line[i] = *(start + i);
    i++;
  }
  line[i] = '\0';
  strcat(line, ".tex");
}

void flatit(FILE *source, FILE *dest) {

  char *line = NULL;
  size_t len = 0;
  while (getline(&line, &len, source) != -1) {

    char *input_ptr = strstr(line, "\\input{");
    char *comment_ptr = strstr(line, "%");

    if (!input_ptr || (comment_ptr && (comment_ptr < input_ptr)))
      fputs(line, dest);
    else {

      getpath(line);

      FILE *input_file;

      if ((input_file = fopen(line, "r")) == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", line);
        break;
      }

      printf(" -> %s\n", line);

      char ch;
      while ((ch = fgetc(input_file)) != EOF) {
        fputc(ch, dest);
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
  } else if ((dest = fopen(DEST_FILE, "w")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", DEST_FILE);
    fclose(source);
  } else {
    printf("Input file: %s\n", source_path);
    flatit(source, dest);
    fclose(dest);
    fclose(source);
  }

  if (errno != 0) {
    remove(DEST_FILE);
    return EXIT_FAILURE;
  }

  printf("Written to: %s\n", DEST_FILE);
  return EXIT_SUCCESS;
}
