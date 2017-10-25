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

    if (1)
    {
	FILE* initFile = fopen("init.scm", "r");
	OBJ fileStream = new_fileStream(initFile);

#ifdef RECURSIVE
	readEvalPrintLoop(fileStream, SCM_FALSE);
#else
	PUSH(fileStream); PUSH(SCM_FALSE);
	(void) trampoline((VOIDFUNCPTRFUNC)CP_readEvalPrintLoop);
#endif
	fclose(initFile);
    }

    printf("Welcome to this very incomplete scheme implementation\n");

    if (setjmp(getMeBackToMain) != 0) {
	printf("back in REPL after error\n");
	// must reset the stacks!
	SP = RSP = 0;
    }

#ifdef RECURSIVE
    readEvalPrintLoop(stdInputStream, SCM_TRUE);
#else
    PUSH(stdInputStream); PUSH(SCM_TRUE);
    (void)trampoline((VOIDFUNCPTRFUNC)CP_readEvalPrintLoop);
#endif

    return 0;
}

//
// trampline the initial function fn;
// returns the last retval
//
OBJ
trampoline(VOIDFUNCPTRFUNC fn) {
    PUSH_RET(NULL);

    do {
	fn = (VOIDFUNCPTRFUNC) (*fn)();
    } while(fn != NULL);
    return RETVAL;
}
