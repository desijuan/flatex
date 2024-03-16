#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flatex.h"

#define MAX_DEPTH 7

char *is_input(char *line) {

    char *comment = strchr(line, '%');
    char *input = strstr(line, "\\input{");
    char *include = strstr(line, "\\include{");

    if (input && (!comment || (input < comment)))
        return strchr(input, '{') + 1;
    else if (include && (!comment || (include < comment)))
        return strchr(include, '{') + 1;

    return NULL;
}

char *is_includepdf(char *line) {

    char *comment = strchr(line, '%');
    char *includepdf = strstr(line, "\\includepdf{");
    char *includegraphics = strstr(line, "\\includegraphics[");

    if (includepdf && (!comment || (includepdf < comment)))
        return strchr(includepdf, '{') + 1;
    else if (includegraphics && (!comment || (includegraphics < comment)))
        return strchr(includegraphics, '{') + 1;

    return NULL;
}

void flat_it(const char *source_path, FILE *dest, int *plevel) {

    FILE *source;
    if ((source = fopen(source_path, "r")) == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", source_path);
        return;
    }

    printf(" %*s-> %s\n", 2 * (*plevel), "", source_path);

    ++*plevel;

    char *p, *q, *end;

    size_t nread;
    size_t len = 1024;
    char *line = malloc(len);
    while ((nread = getline(&line, &len, source)) != -1) {

        if (errno != 0) break;

        if ((q = is_input(line)) != NULL) {

            if (*plevel + 1 > MAX_DEPTH) {
                fprintf(stderr, "Error: Recursion limit exceeded\n");
                errno = 1;
                break;
            }

            p = line;
            end = strchr(q, '}');

            for (unsigned short offset = q - p; p + offset < end; p++)
                *p = *(p + offset);
            *p = '\0';

            strcat(line, ".tex");

            flat_it(line, dest, plevel);
        } else {

            if ((p = is_includepdf(line)) != NULL) {

                q = strrchr(p, '/') + 1;
                end = line + nread;

                for (unsigned short offset = q - p; p + offset < end; p++)
                    *p = *(p + offset);
                *p = '\0';
            }

            fputs(line, dest);
        }
    }

    fclose(source);
    free(line);
    --*plevel;
}
