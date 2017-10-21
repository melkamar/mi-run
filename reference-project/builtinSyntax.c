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

OBJ
scm_if(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ unevaluatedCond = ARG(0);
    OBJ ifPart = ARG(1);
    OBJ elsePart = ARG(2);
    OBJ evaluatedCond, exprToEvaluate;

    POPARGS();

    evaluatedCond = scm_eval(unevaluatedCond, env);

    if (evaluatedCond == SCM_TRUE) {
	exprToEvaluate = ARG(1);
    } else {
	exprToEvaluate = ARG(2);
    }
    return scm_eval(exprToEvaluate, env);
}

OBJ
scm_quote(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ unevaluatedCond = ARG(0);

    POPARGS();
    return unevaluatedCond;
}

OBJ
scm_lambda(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
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
scm_define(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    OBJ nameOrFunctionTemplate;
    OBJ name;

    if (NUMARGS() < 2) {
	argumentCountError("define", 2, NUMARGS(), C_TRUE);
    }
    nameOrFunctionTemplate = ARG(0);
    if (!isSymbol(nameOrFunctionTemplate)) {
	OBJ argList, bodyList, func;

	if (! isCons(nameOrFunctionTemplate)) {
	    error("[define] name must be a symbol", NULL);
	}
	// simple (define (fn-name argList) bodyList)
	name = car(nameOrFunctionTemplate);
	argList = cdr(nameOrFunctionTemplate);
	bodyList = cdr(unevaluatedArgList);
	func = new_userDefinedFunction(argList, bodyList, env);
	defineGlobalValue(env, name, func);
	return SCM_VOID;
    }
    if (NUMARGS() != 2) {
	argumentCountError("define", 2, NUMARGS(), C_FALSE);
    }

    // simple (define var expr)
    name = nameOrFunctionTemplate;

    {
	OBJ unevaluatedExpr = ARG(1);;
	OBJ evaluatedExpr;

	POPARGS();
	evaluatedExpr = scm_eval(unevaluatedExpr, env);
	defineGlobalValue(env, nameOrFunctionTemplate, evaluatedExpr);

	return SCM_VOID;
    }
}

OBJ
scm_set(int indexOfFirstArg, OBJ env, OBJ unevaluatedArgList) {
    fatal("unimpl.");
}

void
initializeBuiltinSyntax() {
    defineBuiltinSyntax("if", scm_if);
    defineBuiltinSyntax("lambda", scm_lambda);
    defineBuiltinSyntax("quote", scm_quote);
    defineBuiltinSyntax("define", scm_define);
    defineBuiltinSyntax("set", scm_set);
}
