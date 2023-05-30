#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEST_FILE "a.tex"
#define MAX_DEPTH 7

char *isinput(char *line) {

  char *comment = strchr(line, '%');
  char *input = strstr(line, "\\input{");
  char *include = strstr(line, "\\include{");

  if (input && (!comment || (input < comment)))
    return input;
  else if (include && (!comment || (include < comment)))
    return include;

  return NULL;
}

char *isincludepdf(char *line) {

  char *comment = strchr(line, '%');
  char *includepdf = strstr(line, "\\includepdf{");
  char *includegraphics = strstr(line, "\\includegraphics[");

  if (includepdf && (!comment || (includepdf < comment)))
    return includepdf;
  else if (includegraphics && (!comment || (includegraphics < comment)))
    return includegraphics;

  return NULL;
}

void flatit(char *source_path, FILE *dest, int *plevel) {

  FILE *source;
  if ((source = fopen(source_path, "r")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", source_path);
    return;
  }

  printf(" %*s-> %s\n", 2 * (*plevel), "", source_path);

  ++*plevel;

  size_t nread;
  size_t len = 1024;
  char *line = malloc(len);
  while ((nread = getline(&line, &len, source)) != -1) {

    if (errno != 0)
      break;

    char *s;

    if ((s = isinput(line)) != NULL) {

      if (*plevel + 1 > MAX_DEPTH) {
        fprintf(stderr, "Error: Recursion limit exceeded\n");
        errno = 1;
        break;
      }

      char *p = line;
      char *q = strchr(s, '{') + 1;
      char *end = strchr(q, '}');

      for (unsigned short offset = q - p; p + offset < end; p++)
        *(p) = *(p + offset);
      *p = '\0';

      strcat(line, ".tex");

      flatit(line, dest, plevel);
    } else {

      if ((s = isincludepdf(line)) != NULL) {

        char *p = strchr(s, '{') + 1;
        char *q = strrchr(p, '/') + 1;
        char *end = line + nread;

        for (unsigned short offset = q - p; p + offset < end; p++)
          *(p) = *(p + offset);
        *p = '\0';
      }

      fputs(line, dest);
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
