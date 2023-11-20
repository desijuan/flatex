#ifndef FLATEX_H
#define FLATEX_H

#include <stdio.h>

char *is_input(char *line);
char *is_includepdf(char *line);
void flat_it(char *source_path, FILE *dest, int *plevel);

#endif // FLATEX
