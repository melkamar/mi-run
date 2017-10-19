#include "scheme.h"

void
fatal(char *message) {
    fprintf(stderr, "fatal %s\n", message);
    abort();
}

void
error(char *message) {
    fprintf(stderr, "error %s\n", message);
    abort();
}
