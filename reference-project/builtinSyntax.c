//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

#include "scheme.h"
#include "builtinSupport.h"

#ifdef RECURSIVE

OBJ
builtin_if(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ unevaluatedCond = ARG(0);
    OBJ ifPart = ARG(1);
    OBJ elsePart = ARG(2);
    OBJ evaluatedCond, exprToEvaluate;

    POPARGS();

    evaluatedCond = scm_eval(unevaluatedCond, env);

    if (evaluatedCond == SCM_TRUE) {
	exprToEvaluate = ifPart;
    } else {
	exprToEvaluate = elsePart;
    }
    return scm_eval(exprToEvaluate, env);
}

OBJ
builtin_quote(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ unevaluatedCond = ARG(0);

    POPARGS();
    return unevaluatedCond;
}

OBJ
builtin_lambda(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ argList, bodyList;

    if (NUMARGS() < 2) {
	argumentCountError("lambda", 2, NUMARGS(), C_TRUE);
    }
    argList = ARG(0);
    // printf("lambda argList is: "); scm_print(argList, stdout); printf("\n");
    bodyList = cdr(unevaluatedArgList);
    // printf("lambda bodyList is: "); scm_print(bodyList, stdout); printf("\n");

    POPARGS();
    return new_userDefinedFunction(argList, bodyList, env);
}

OBJ
builtin_define(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ nameOrFunctionTemplate;
    OBJ name;
    OBJ definedValue;

    if (NUMARGS() < 2) {
	argumentCountError("define", 2, NUMARGS(), C_TRUE);
    }
    nameOrFunctionTemplate = ARG(0);
    if (isCons(nameOrFunctionTemplate)) {
	OBJ argList, bodyList, func;

	// (define (fn-name argList) bodyList)
	name = car(nameOrFunctionTemplate);
	argList = cdr(nameOrFunctionTemplate);
	bodyList = cdr(unevaluatedArgList);
	if (bodyList == SCM_NIL) {
	    error("[define] bad (missing) function body", NULL);
	}
	func = new_userDefinedFunction(argList, bodyList, env);
	definedValue = func;
    } else {
	// simple (define var expr)

	OBJ unevaluatedExpr, evaluatedExpr;

	if (NUMARGS() != 2) {
	    argumentCountError("define", 2, NUMARGS(), C_FALSE);
	}
	if (!isSymbol(nameOrFunctionTemplate)) {
	    error("[define] name must be a symbol", NULL);
	}

	name = nameOrFunctionTemplate;
	unevaluatedExpr = ARG(1);

	evaluatedExpr = scm_eval(unevaluatedExpr, env);
	definedValue = evaluatedExpr;
    }

    defineGlobalValue(env, name, definedValue);
    POPARGS();
    return SCM_VOID;
}

OBJ
builtin_set(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    fatal("unimpl.");
}

void
initializeBuiltinSyntax() {
    defineBuiltinSyntax("if", builtin_if);
    defineBuiltinSyntax("lambda", builtin_lambda);
    defineBuiltinSyntax("quote", builtin_quote);
    defineBuiltinSyntax("define", builtin_define);
    defineBuiltinSyntax("set!", builtin_set);
}

#else // not RECURSIVE i.e. CONTINUATION PASSING

//
// for continuation passing, builtin syntax returns a continuation
//
VOIDFUNCPTRFUNC CP_if2();

VOIDFUNCPTRFUNC
builtin_CP_if(OBJ unevaluatedArgList, OBJ env) {
    if (unevaluatedArgList != SCM_NIL) {
	OBJ unevaluatedCond = car(unevaluatedArgList);
	OBJ restList = cdr(unevaluatedArgList);

	if (restList != SCM_NIL) {
	    OBJ ifPart = car(restList);
	    restList = cdr(restList);

	    if (restList != SCM_NIL) {
		OBJ elsePart = car(restList);
		restList = cdr(restList);

		if (restList == SCM_NIL) {
		    // save locals for if2
		    PUSH(ifPart);
		    PUSH(elsePart);
		    PUSH(env);

		    // out args
		    PUSH(unevaluatedCond);
		    PUSH(env);
		    CALL(CP_eval, CP_if2);
		    // not reached
		}
	    }
	}
    }
    argumentCountError("if", 3, length(unevaluatedArgList), C_FALSE);
}

VOIDFUNCPTRFUNC
CP_if2() {
    OBJ ifPart, elsePart, env, evaluatedCond, exprToEvaluate;

    // restore locals
    env = POP();
    elsePart = POP();
    ifPart = POP();

    evaluatedCond = RETVAL;

    if (evaluatedCond == SCM_TRUE) {
	exprToEvaluate = ifPart;
    } else {
	exprToEvaluate = elsePart;
    }
    PUSH(exprToEvaluate);
    PUSH(env);
    TCALL(CP_eval);
}

VOIDFUNCPTRFUNC
builtin_CP_quote(OBJ unevaluatedArgList, OBJ env) {
    if (unevaluatedArgList != SCM_NIL) {
	OBJ unevaluatedArg = car(unevaluatedArgList);
	OBJ restList = cdr(unevaluatedArgList);

	if (restList == SCM_NIL) {
	    RETURN (unevaluatedArg);
	}
    }
    argumentCountError("quote", 1, length(unevaluatedArgList), C_FALSE);
}

VOIDFUNCPTRFUNC
builtin_CP_lambda(OBJ unevaluatedArgList, OBJ env) {
    if (unevaluatedArgList != SCM_NIL) {
	OBJ argList = car(unevaluatedArgList);
	OBJ restList = cdr(unevaluatedArgList);

	if (restList != SCM_NIL) {
	    OBJ bodyList = restList;
	    OBJ retVal = new_userDefinedFunction(argList, bodyList, env);

	    RETURN(retVal);
	}
    }
    argumentCountError("lambda", 2, length(unevaluatedArgList), C_TRUE);
}

static VOIDFUNCPTRFUNC CP_define2();

VOIDFUNCPTRFUNC
builtin_CP_define(OBJ unevaluatedArgList, OBJ env) {
    if (unevaluatedArgList != SCM_NIL) {
	OBJ nameOrFunctionTemplate = car(unevaluatedArgList);
	OBJ restList = cdr(unevaluatedArgList);

	if (isSymbol(nameOrFunctionTemplate)) {
	    OBJ expr = car(restList);
	    OBJ name = nameOrFunctionTemplate;
	    restList = cdr(restList);

	    if (restList == SCM_NIL) {
		PUSH(name);
		PUSH(env);

		PUSH(expr);
		PUSH(env);
		CALL(CP_eval, CP_define2);
		// not reached
	    }
	} else {
	    OBJ argList, bodyList, func, functionName;

	    if (! isCons(nameOrFunctionTemplate)) {
		error("[define] name must be a symbol", NULL);
	    }

	    // simple (define (fn-name argList) bodyList)
	    functionName = car(nameOrFunctionTemplate);
	    argList = cdr(nameOrFunctionTemplate);
	    bodyList = cdr(unevaluatedArgList);
	    func = new_userDefinedFunction(argList, bodyList, env);
	    defineGlobalValue(env, functionName, func);
	    RETURN( SCM_VOID );
	}
    }
    argumentCountError("define", 2, length(unevaluatedArgList), C_TRUE);
}

static VOIDFUNCPTRFUNC
CP_define2() {
    OBJ evaluatedExpr = RETVAL;
    OBJ name, env;

    env = POP();
    name = POP();

    defineGlobalValue(env, name, evaluatedExpr);
    RETURN( SCM_VOID );
}

VOIDFUNCPTRFUNC
builtin_CP_set(OBJ unevaluatedArgList) {
    fatal("unimpl.");
}

//
// defined as syntax, because it calls eval recursively
//
static VOIDFUNCPTRFUNC CP_load2();

VOIDFUNCPTRFUNC
builtin_CP_load(OBJ unevaluatedArgList, OBJ env) {
    if (unevaluatedArgList != SCM_NIL) {
	OBJ unevaluatedFileName = car(unevaluatedArgList);
	OBJ restList = cdr(unevaluatedArgList);

	if (restList == SCM_NIL) {
	    // preserve for later
	    PUSH(env);

	    // args for eval
	    PUSH(unevaluatedFileName); PUSH(env);
	    CALL(CP_eval, CP_load2);
	    // not reached
	}
    }
    argumentCountError("load", 1, length(unevaluatedArgList), C_FALSE);
}

static VOIDFUNCPTRFUNC CP_load3();

VOIDFUNCPTRFUNC
CP_load2() {
    OBJ evaluatedArg = RETVAL;
    OBJ env, fileStream;
    FILE* initFile;
    char *fileName;

    env = POP();

    fileName = stringVal(evaluatedArg);

    initFile = fopen(fileName, "r");
    fileStream = new_fileStream(initFile);

    // needed later
    PUSH(fileStream);

    PUSH(fileStream); PUSH(SCM_FALSE);
    CALL(CP_readEvalPrintLoop, CP_load3);
    // not reached
}

//
// only needed to close the file
//
VOIDFUNCPTRFUNC
CP_load3() {
    OBJ fileStream;
    FILE* initFile;

    fileStream = POP();
    initFile = fileStream->fileStream.theFile;

    fclose(initFile);
    RETURN ( SCM_VOID );
}

void
initializeBuiltinSyntax() {
    defineBuiltinSyntax("if", (VOIDFUNCPTRFUNC)builtin_CP_if);
    defineBuiltinSyntax("lambda", (VOIDFUNCPTRFUNC)builtin_CP_lambda);
    defineBuiltinSyntax("quote", (VOIDFUNCPTRFUNC)builtin_CP_quote);
    defineBuiltinSyntax("define", (VOIDFUNCPTRFUNC)builtin_CP_define);
    defineBuiltinSyntax("set!", (VOIDFUNCPTRFUNC)builtin_CP_set);
    defineBuiltinSyntax("load", (VOIDFUNCPTRFUNC)builtin_CP_load);
}

#endif // not RECURSIVE
