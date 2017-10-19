#include "scheme.h"

void
readEvalPrintLoop(OBJ inStream) {
    OBJ expr;
    OBJ evaluatedResult;

    for (;;) {
	printf("> ");
	expr = scm_read(inStream);
	if (expr == SCM_EOF) {
	    return;
	}
	evaluatedResult = scm_eval(expr);
	scm_print(evaluatedResult);
	printf("\n");
    }
}

OBJ
scm_evalSymbol(OBJ expr) {
    OBJ valOrNULL = getGlobalValue(expr);

    if (valOrNULL == NULL) {
	error("undefined variable: ", expr);
    }
    return valOrNULL;
}

OBJ
scm_evalList(OBJ expr) {
    OBJ unevaluatedFunction = car(expr);
    OBJ evaluatedFunction = scm_eval(unevaluatedFunction);

    switch (tagOf(evaluatedFunction)) {
	case T_BUILTINFUNCTION:
	    {
		OBJ unevaluatedArgList = cdr(expr);
		int indexOfFirstArg = SP;
		OBJ retVal;

//printf("the arguments are: ");
//scm_print(unevaluatedArgList);
//printf("\n");
		while (unevaluatedArgList != SCM_NIL) {
		    OBJ unevaluatedArg = car(unevaluatedArgList);
		    OBJ evaluatedArg = scm_eval(unevaluatedArg);

		    PUSH(evaluatedArg);
		    unevaluatedArgList = cdr(unevaluatedArgList);
printf("next argument: ");
scm_print(unevaluatedArg);
printf(" evaluated to: ");
scm_print(evaluatedArg);
printf("\n");
		}
		retVal = evaluatedFunction->builtinFunction.code(indexOfFirstArg);
		return retVal;
	    }
	    fatal("as yet unimplemented: scm_evalList");

	default:
	    error("non function in function slot: ", evaluatedFunction);
    }
}

OBJ
scm_eval(OBJ expr) {
    switch (tagOf(expr)) {
	case T_SYMBOL:
	    return scm_evalSymbol(expr);
	case T_CONS:
	    return scm_evalList(expr);
	default:
	    return expr;
    }
}
