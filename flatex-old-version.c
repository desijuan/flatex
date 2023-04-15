#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEST_FILE "a.tex"

void get_path(char *line) {

  char *start = strstr(line, "{") + 1;
  char *end = strstr(start, "}");

  int i = 0;
  while (start + i < end) {
    line[i] = *(start + i);
    i++;
  }
  line[i] = '\0';
}

void fputf(char *source_path, FILE *dest) {

  FILE *source = fopen(source_path, "r");
  if (source == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", source_path);
    exit(1);
  }

  char ch;
  while ((ch = fgetc(source)) != EOF) {
    fputc(ch, dest);
  }

  fclose(source);
}

int main(int argc, char **argv) {

  FILE *source = fopen(argv[1], "r");
  if (source == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", argv[1]);
    exit(1);
  }

  FILE *dest = fopen(DEST_FILE, "w");
  if (dest == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", DEST_FILE);
    exit(1);
  }

  char *line = NULL;
  size_t len = 0;
  while (getline(&line, &len, source) != -1) {
    char *input_ptr = strstr(line, "\\input{");
    char *comment_ptr = strstr(line, "%");

    if (input_ptr && (!comment_ptr || (input_ptr < comment_ptr))) {
      get_path(line);
      fputf(strcat(line, ".tex"), dest);
    } else {
      fputs(line, dest);
    }
  }

  free(line);

  fclose(dest);
  fclose(source);

  printf("Generated file: %s\n", DEST_FILE);
}
