// just created

#include "scheme.h"
#include "functionPrototypes.h"

int
main(int argc, char **argv) {
    OBJ stdInputStream = new_fileStream(stdin);

    selftest();

    printf("Welcome to this very incomplete scheme implementation\n");

    readEvalPrintLoop(stdInputStream);
}
