#include "scheme.h"

static void
initializeWellknownObjects() {
    SCM_NIL = new_singleton(T_NIL);
    SCM_TRUE = new_singleton(T_TRUE);
    SCM_FALSE = new_singleton(T_FALSE);
    SCM_EOF = new_singleton(T_EOF);

    defineBuiltinFunction("+", scm_plusFunction);
    defineBuiltinFunction("-", scm_minusFunction);
    defineBuiltinFunction("*", scm_timesFunction);
    defineBuiltinFunction("/", scm_quotientFunction);

    defineBuiltinFunction("cons", scm_consFunction);
    defineBuiltinFunction("car", scm_carFunction);
    defineBuiltinFunction("cdr", scm_cdrFunction);
}

#ifdef DEBUGGING
static void
initializeDummyDefinitions() {
    OBJ sym_a = new_symbol("a");
    OBJ sym_b = new_symbol("b");

    defineGlobalValue(sym_a, new_integer(100));
    defineGlobalValue(sym_b, new_integer(200));
}

#endif

int
main(int argc, char **argv) {
    OBJ stdInputStream = new_fileStream(stdin);

    initializeStack();
    initializeSymbolTable();
    initializeGlobalEnvironment();
    initializeWellknownObjects();

    selftest();

#ifdef DEBUGGING
    initializeDummyDefinitions();
#endif
    printf("Welcome to this very incomplete scheme implementation\n");

    readEvalPrintLoop(stdInputStream);
}
