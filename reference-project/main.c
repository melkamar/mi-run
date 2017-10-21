#include "scheme.h"
#include <setjmp.h>

jmp_buf getMeBackToMain;

void
backIntoMain() {
    // printf("about to do a longjmp...\n");
    longjmp(getMeBackToMain, 1);
}

static void
initializeWellknownObjects() {
    SCM_NIL = new_singleton(T_NIL);
    SCM_TRUE = new_singleton(T_TRUE);
    SCM_FALSE = new_singleton(T_FALSE);
    SCM_EOF = new_singleton(T_EOF);
    SCM_VOID = new_singleton(T_VOID);

    defineGlobalValue(globalEnvironment, new_symbol("#t"), SCM_TRUE);
    defineGlobalValue(globalEnvironment, new_symbol("#f"), SCM_FALSE);
}

#ifdef DEBUGGING

static void
initializeDummyDefinitions() {
    OBJ sym_a = new_symbol("a");
    OBJ sym_b = new_symbol("b");

    defineGlobalValue(globalEnvironment, sym_a, new_integer(100));
    defineGlobalValue(globalEnvironment, sym_b, new_integer(200));
}

#endif

int
main(int argc, char **argv) {
    OBJ stdInputStream = new_fileStream(stdin);

    initializeStack();
    initializeReturnStack();
    initializeSymbolTable();
    initializeGlobalEnvironment();
    initializeWellknownObjects();
    initializeBuiltinFunctions();
    initializeBuiltinSyntax();

#ifdef SELFTEST
    selftest();
#endif

#ifdef DEBUGGING
    initializeDummyDefinitions();
#endif

    {
	FILE* initFile = fopen("init.scm", "r");
	OBJ fileStream = new_fileStream(initFile);

	readEvalPrintLoop(fileStream, C_FALSE);
	fclose(initFile);
    }

    printf("Welcome to this very incomplete scheme implementation\n");

    if (setjmp(getMeBackToMain) != 0) {
	printf("back in REPL after error\n");
    }

    PUSH(stdInputStream);
    PUSH(SCM_TRUE);
    trampoline((VOIDPTRFUNC)CP_readEvalPrintLoop);
//    readEvalPrintLoop(stdInputStream, C_TRUE);
}

void
trampoline(VOIDPTRFUNC fn) {
    PUSH_RET(NULL);

    do {
	fn = (*fn)();
    } while(fn != NULL);
}
