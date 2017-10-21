#include "scheme.h"

static int trace = 0;
static int traceLevel = 0;

static void
spaces(n) {
    while (n--) { printf(" "); }
}

void
readEvalPrintLoop(OBJ inStream, bool doPrompt) {
    OBJ expr;
    OBJ evaluatedResult;

    for (;;) {
	if (doPrompt) {
	    printf("> ");
	}
	expr = scm_read(inStream);
	if (expr == SCM_EOF) {
	    return;
	}
	evaluatedResult = scm_eval(expr, globalEnvironment);
	if (evaluatedResult != SCM_VOID) {
	    scm_print(evaluatedResult, stdout);
	    printf("\n");
	}
    }
}

OBJ
scm_evalSymbol(OBJ expr, OBJ env) {
    OBJ valOrNULL = getGlobalValue(env, expr);

    if (valOrNULL == NULL) {
	error("undefined variable:", expr);
    }
    return valOrNULL;
}

OBJ
scm_evalList(OBJ expr, OBJ env) {
    OBJ unevaluatedFunction = car(expr);
    OBJ evaluatedFunction = scm_eval(unevaluatedFunction, env);

    switch (tagOf(evaluatedFunction)) {
	case T_BUILTINFUNCTION:
	    {
		OBJ unevaluatedArgList = cdr(expr);
		int indexOfFirstArg = SP;
		OBJ retVal;

		// evaluate each argument and push them onto the stack

		// printf("the arguments are: ");
		// scm_print(unevaluatedArgList);
		// printf("\n");
		while (unevaluatedArgList != SCM_NIL) {
		    OBJ unevaluatedArg = car(unevaluatedArgList);
		    OBJ evaluatedArg = scm_eval(unevaluatedArg, env);

		    PUSH(evaluatedArg);
		    unevaluatedArgList = cdr(unevaluatedArgList);

		    // printf("next argument: ");
		    // scm_print(unevaluatedArg, stdout);
		    // printf(" evaluated to: ");
		    // scm_print(evaluatedArg, stdout);
		    // printf("\n");
		}

		// call the code of the builtin with eval'd args on the stack
		retVal = evaluatedFunction->builtinFunction.code(indexOfFirstArg);
		return retVal;
	    }

	case T_BUILTINSYNTAX:
	    {
		OBJ unevaluatedArgList = cdr(expr);
		OBJ listElement = unevaluatedArgList;
		int indexOfFirstArg = SP;
		OBJ retVal;

		// push all unevaluated args onto the stack

		// printf("the arguments are: ");
		// scm_print(unevaluatedArgList);
		// printf("\n");
		while (listElement != SCM_NIL) {
		    OBJ unevaluatedArg = car(listElement);

		    PUSH(unevaluatedArg);
		    listElement = cdr(listElement);
		}

		// call the builtin syntax code with uneval'd args on stack

		retVal = evaluatedFunction->builtinSyntax.code(indexOfFirstArg, env, unevaluatedArgList);
		return retVal;
	    }

	case T_USERDEFINEDFUNCTION:
	    {
		OBJ formalArgs = evaluatedFunction->userDefinedFunction.argList;
		OBJ bodyList = evaluatedFunction->userDefinedFunction.bodyList;
		OBJ functionsHome = evaluatedFunction->userDefinedFunction.homeEnvironment;
		OBJ unevaluatedArgList = cdr(expr);
		OBJ newEnv = new_globalEnvironment(7);
		OBJ lastResult;

		newEnv->globalEnvironment.parentEnvironment = functionsHome;

		if (trace) {
		    spaces(traceLevel);
		    printf("formal argList is: "); scm_print(formalArgs, stdout); printf("\n");
		    printf("unevalated argList is: "); scm_print(unevaluatedArgList, stdout); printf("\n");
		}
		while (unevaluatedArgList != SCM_NIL) {
		    OBJ argName = car(formalArgs);
		    OBJ unevaluatedArg = car(unevaluatedArgList);
		    OBJ evaluatedArg = scm_eval(unevaluatedArg, env);

		    // printf("arg is: "); scm_print(argName, stdout); printf("\n");
		    // printf("value is: "); scm_print(evaluatedArg, stdout); printf("\n");

		    defineGlobalValue(newEnv, argName, evaluatedArg);

		    unevaluatedArgList = cdr(unevaluatedArgList);
		    formalArgs = cdr(formalArgs);
		}
		while (bodyList != SCM_NIL) {
		    lastResult = scm_eval(car(bodyList), newEnv);
		    bodyList = cdr(bodyList);
		}
		return lastResult;
	    }

	default:
	    error("non function in function slot:", evaluatedFunction);
    }
}

OBJ
scm_eval(OBJ expr, OBJ env) {
    OBJ result;

    if (trace) {
	spaces(traceLevel);
	printf("eval: "); scm_print(expr, stdout); printf("\n");
    }

    switch (tagOf(expr)) {
	case T_SYMBOL:
	    result = scm_evalSymbol(expr, env);
	    break;
	case T_CONS:
	    traceLevel++;
	    result = scm_evalList(expr, env);
	    traceLevel--;
	    break;
	default:
	    result = expr;
	    break;
    }

    if (trace) {
	spaces(traceLevel);
	printf("--> "); scm_print(result, stdout); printf("\n");
    }
    return result;
}

OBJ
scm_evalCString(char *expressionString) {
    OBJ inStream = new_stringStream(expressionString);
    OBJ expr;

    expr = scm_read(inStream);
    return scm_eval(expr, globalEnvironment);
}
