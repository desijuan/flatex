#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "flatex.h"

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <dest>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *source_path = argv[1];
    const char *dest_path = argv[2];

    FILE *dest_file;
    if ((dest_file = fopen(dest_path, "w")) == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", dest_path);
        return EXIT_FAILURE;
    }

    int level = 0;

    flat_it(source_path, dest_file, &level);

    fclose(dest_file);

    if (errno != 0) {
        remove(dest_path);
        return EXIT_FAILURE;
    }

    printf("Written to: %s\n", dest_path);
    return EXIT_SUCCESS;
}
