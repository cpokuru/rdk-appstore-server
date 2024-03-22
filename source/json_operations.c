#include "appstoreserver.h"
#include "microhttpd_wrapper.h"

json_t *readJsonFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open output file\n");
        return NULL;
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t nread;
    size_t offset = 0;
    while ((nread = fread(buffer + offset, 1, sizeof(buffer) - offset, file)) > 0) {
        offset += nread;
    }
    fclose(file);

    buffer[offset] = '\0';

    json_error_t error;
    json_t *root = json_loads(buffer, 0, &error);
    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return NULL;
    }

    return root;
}


