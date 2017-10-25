#include "scheme.h"

static int trace = 0;
static int traceLevel = 0;

static void
spaces(n) {
    while (n--) { printf(" "); }
}

#ifdef RECURSIVE

void
readEvalPrintLoop(OBJ inStream, OBJ doPrompt) {
    OBJ expr;
    OBJ evaluatedResult;

    for (;;) {
	if (doPrompt == SCM_TRUE) {
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

#else // non-recursive

static VOIDFUNCPTRFUNC CP_readEvalPrintLoop_1();

VOIDFUNCPTRFUNC
CP_readEvalPrintLoop() {
    OBJ inStream, doPrompt;
    OBJ expr;

    doPrompt = POP();
    inStream = POP();

    if (doPrompt == SCM_TRUE) {
	printf("> ");
    }
    expr = scm_read(inStream);
    if (expr == SCM_EOF) {
	RETURN(SCM_VOID);
    }

    PUSH(inStream);
    PUSH(doPrompt);

    PUSH(expr);
    PUSH(globalEnvironment);
    CALL(CP_eval, CP_readEvalPrintLoop_1);
    // never reached
}

// coming back from the call to:
// scm_eval(expr, globalEnvironment);

static VOIDFUNCPTRFUNC
CP_readEvalPrintLoop_1() {
    OBJ doPrompt;
    OBJ inStream;
    OBJ evaluatedResult;

    doPrompt = POP();
    inStream = POP();

    evaluatedResult = RETVAL;
    if (evaluatedResult != SCM_VOID) {
	scm_print(evaluatedResult, stdout);
	printf("\n");
    }

    PUSH(inStream); PUSH(doPrompt);
    TCALL(CP_readEvalPrintLoop);
}


#endif // non RECURSIVE

#ifdef RECURSIVE

static OBJ
scm_evalSymbol(OBJ expr, OBJ env) {
    OBJ valOrNULL = getGlobalValue(env, expr);

    if (valOrNULL == NULL) {
	error("undefined:", expr);
    }
    return valOrNULL;
}

static OBJ
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


#else // non-RECURSIVE

static VOIDFUNCPTRFUNC CP_evalList_1();

static VOIDFUNCPTRFUNC
CP_evalList() {
    OBJ expr, env;

    // in arguments
    env = POP();
    expr = POP();

    {
	OBJ unevaluatedFunction = car(expr);

	// save locals (for CP_evalList_1)
	PUSH(expr);
	PUSH(env);

	// out arguments
	PUSH(unevaluatedFunction);
	PUSH(env);
	CALL(CP_eval, CP_evalList_1);
	// not reached
    }
}

// here returning from scm_eval(unevaluatedFunction, env);
//
VOIDFUNCPTRFUNC CP_evalList_2();
VOIDFUNCPTRFUNC CP_evalUDF_1();
VOIDFUNCPTRFUNC CP_evalUDF_2();

VOIDFUNCPTRFUNC
CP_evalList_1() {
    OBJ evaluatedFunction = RETVAL;
    OBJ env, expr;

    // restore locals
    env = POP();
    expr = POP();

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

		    // save locals..
		    PUSH_INT(indexOfFirstArg);
		    PUSH(evaluatedFunction);
		    PUSH(unevaluatedArgList);
		    PUSH(env);

		    // out args
		    PUSH(unevaluatedArg);
		    PUSH(env);
		    CALL(CP_eval, CP_evalList_2);
		    // not reached
		}

		// call the code of the builtin with eval'd args on the stack
		retVal = evaluatedFunction->builtinFunction.code(indexOfFirstArg);
		RETURN( retVal );
	    }

	case T_BUILTINSYNTAX:
	    {
		OBJ unevaluatedArgList = cdr(expr);

		// just call syntax handler passing the unevaluatedArgList
		// this is now a continuation returning handler
		// therefore, return whatever the handler returns
		return (VOIDFUNCPTRFUNC)(evaluatedFunction->builtinSyntax.code(unevaluatedArgList, env));
	    }

	case T_USERDEFINEDFUNCTION:
	    {
		OBJ formalArgs = evaluatedFunction->userDefinedFunction.argList;
		OBJ bodyList = evaluatedFunction->userDefinedFunction.bodyList;
		OBJ functionsHome = evaluatedFunction->userDefinedFunction.homeEnvironment;
		OBJ unevaluatedArgList = cdr(expr);
		OBJ newEnv = new_globalEnvironment(7);

		newEnv->globalEnvironment.parentEnvironment = functionsHome;

		if (trace) {
		    spaces(traceLevel);
		    printf("formal argList is: "); scm_print(formalArgs, stdout); printf("\n");
		    printf("unevalated argList is: "); scm_print(unevaluatedArgList, stdout); printf("\n");
		}
		while (unevaluatedArgList != SCM_NIL) {
		    OBJ argName = car(formalArgs);
		    OBJ unevaluatedArg = car(unevaluatedArgList);

		    PUSH(bodyList);
		    PUSH(formalArgs);
		    PUSH(unevaluatedArgList);
		    PUSH(newEnv);
		    PUSH(argName);
		    PUSH(env);

		    PUSH(unevaluatedArg);
		    PUSH(env);
		    CALL(CP_eval, CP_evalUDF_1);
		    // not reached
		}
		if (bodyList == SCM_NIL) {
		    // can actually not happen (now checked for in lambda)
		    RETURN ( SCM_VOID );
		}
		// the last bodyExpr is evaluated with a tail call!
		if (cdr(bodyList) == SCM_NIL) {
		    PUSH(car(bodyList)); PUSH(newEnv);
		    TCALL(CP_eval);
		}
		// otherwise...
		// save for later

		PUSH(cdr(bodyList)); PUSH(env);

		PUSH(car(bodyList)); PUSH(env);
		CALL(CP_eval, CP_evalUDF_2);
		// not reached
	    }

	default:
	    error("non function in function slot:", evaluatedFunction);
    }
}


VOIDFUNCPTRFUNC
CP_evalUDF_1() {
    OBJ evaluatedArg = RETVAL;
    OBJ env, newEnv, argName, unevaluatedArgList, formalArgs, bodyList;

    env = POP();
    argName = POP();
    newEnv = POP();
    unevaluatedArgList = POP();
    formalArgs = POP();
    bodyList = POP();

    defineGlobalValue(newEnv, argName, evaluatedArg);

    unevaluatedArgList = cdr(unevaluatedArgList);
    formalArgs = cdr(formalArgs);

    while (unevaluatedArgList != SCM_NIL) {
	OBJ argName = car(formalArgs);
	OBJ unevaluatedArg = car(unevaluatedArgList);

	PUSH(bodyList);
	PUSH(formalArgs);
	PUSH(unevaluatedArgList);
	PUSH(newEnv);
	PUSH(argName);
	PUSH(env);

	PUSH(unevaluatedArg); PUSH(env);
	CALL(CP_eval, CP_evalUDF_1);
	// not reached
    }

    while (bodyList != SCM_NIL) {
	OBJ nextExpr = car(bodyList);

	bodyList = cdr(bodyList);
	if (bodyList == SCM_NIL) {
	    // the last expression via tail call!
	    PUSH(nextExpr); PUSH(newEnv);
	    TCALL(CP_eval);
	}

	PUSH(bodyList);
	PUSH(newEnv);

	PUSH(nextExpr); PUSH(newEnv);
	CALL(CP_eval, CP_evalUDF_2);
	// not reached
    }
    RETURN ( SCM_VOID );
}

VOIDFUNCPTRFUNC
CP_evalUDF_2() {
    OBJ bodyList, newEnv, nextExpr;

    newEnv = POP();
    bodyList = POP();

    nextExpr = car(bodyList);
    bodyList = cdr(bodyList);
    if (bodyList == SCM_NIL) {
	// the last expression via tail call!
	PUSH(nextExpr); PUSH(newEnv);
	TCALL(CP_eval);
	// not reached
    }

    // remember for next round
    PUSH(bodyList);
    PUSH(newEnv);

    PUSH(nextExpr); PUSH(newEnv);
    CALL(CP_eval, CP_evalUDF_2);
    // not reached
}

VOIDFUNCPTRFUNC
CP_evalList_2() {
    OBJ evaluatedArg = RETVAL;
    OBJ env, unevaluatedArgList, evaluatedFunction;
    int indexOfFirstArg;
    OBJ retVal;

    // restore locals..
    env = POP();
    unevaluatedArgList = POP();
    evaluatedFunction = POP();
    indexOfFirstArg = POP_INT();

    PUSH(evaluatedArg);
    unevaluatedArgList = cdr(unevaluatedArgList);

    while (unevaluatedArgList != SCM_NIL) {
	OBJ unevaluatedArg = car(unevaluatedArgList);

	// save locals..
	PUSH_INT(indexOfFirstArg);
	PUSH(evaluatedFunction);
	PUSH(unevaluatedArgList);
	PUSH(env);

	// out args
	PUSH(unevaluatedArg); PUSH(env);
	CALL(CP_eval, CP_evalList_2);
	// not reached
    }

    // call the code of the builtin with eval'd args on the stack
    retVal = evaluatedFunction->builtinFunction.code(indexOfFirstArg);
    RETURN (retVal);
}

VOIDFUNCPTRFUNC
CP_eval() {
    OBJ expr, env;

    // in args
    env = POP();
    expr = POP();

    if (trace) {
	spaces(traceLevel);
	printf("eval: "); scm_print(expr, stdout); printf("\n");
    }

    switch (tagOf(expr)) {
	case T_SYMBOL:
	    {
		OBJ valOrNULL = getGlobalValue(env, expr);

		if (valOrNULL == NULL) {
		    error("undefined:", expr);
		}
		RETURN( valOrNULL );
	    }
	    // not reached

	case T_CONS:
	    // out args
	    PUSH(expr);
	    PUSH(env);
	    TCALL(CP_evalList)
	    // not reached

	default:
	    RETURN(expr);
	    // not reached
    }
}

#endif // non-RECURSIVE

//
// the following is (currently) only used by the selftests
//
OBJ
scm_evalCString(char *expressionString) {
    OBJ inStream = new_stringStream(expressionString);
    OBJ expr;
    OBJ result;

    expr = scm_read(inStream);
#ifdef RECURSIVE
    result = scm_eval(expr, globalEnvironment);
#else
    PUSH(expr); PUSH(globalEnvironment);
    (void) trampoline((VOIDFUNCPTRFUNC)CP_eval);
    result = RETVAL;
#endif
    return result;
}
