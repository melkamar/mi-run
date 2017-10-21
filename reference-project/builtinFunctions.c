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

//
// builtin functions
//
// these are called by eval with evaluated arguments on the
// stack. The single passed argument is the stack index of
// the first argument (AP, for argument pointer).
// AP == SP if no arguments were pushed.
// Stack grows upward, so the stack looks like:
// The nth argument can be accessed via the ARG(n) macro.
// The number of arguments is given by the NUMARGS() macro.
//
// ^                      ^
// +----------------------+
// +                      +  <--- SP (next free slot)
// +----------------------+
// +        argN          +
// +----------------------+
// /        ...           /
// +----------------------+
// +        arg1          +
// +----------------------+
// +        arg0          +  <--- AP (indexOfFirstArg)
// +----------------------+
//

OBJ
scm_plus(int indexOfFirstArg) {
    int argIndex = indexOfFirstArg;
    long sum = 0;

    while (argIndex < SP) {
	OBJ arg = stack[argIndex];

	// add it
	switch (tagOf(arg)) {
	    case T_INTEGER:
		sum += integerVal(arg);
		break;
	    default:
		error("[+] non integer argument:", arg);
	}
	argIndex++;
    }
    POPARGS();
    return new_integer(sum);
}

OBJ
scm_minus(int indexOfFirstArg) {
    long difference;
    OBJ firstArg;

    if (NUMARGS() == 0) {
	argumentCountError("-", 1, 0, C_TRUE);
    }

    firstArg = ARG(0);
    if (!isInteger(firstArg)) {
	error("[-] argument is not integer:", firstArg);
    }
    difference = integerVal(firstArg);

    if (NUMARGS() == 1) {
	difference = -difference;
    } else {
	int argIdx;

	for (argIdx=1; argIdx<NUMARGS(); argIdx++) {
	    OBJ arg = ARG(argIdx);

	    if (!isInteger(arg)) {
		error("[-] argument is not integer:", arg);
	    }
	    difference -= integerVal(arg);
	}
    }
    POPARGS();
    return new_integer(difference);
}

OBJ
scm_times(int indexOfFirstArg) {
    int argIndex = indexOfFirstArg;
    long product = 1;

    while (argIndex < SP) {
	OBJ arg = stack[argIndex];

	// add it
	switch (tagOf(arg)) {
	    case T_INTEGER:
		product *= integerVal(arg);
		break;
	    default:
		error("[*] non integer argument:", arg);
	}
	argIndex++;
    }
    POPARGS();
    return new_integer(product);
}

OBJ
scm_quotient(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_bitwise_and(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_bitwise_or(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_bitwise_xor(int indexOfFirstArg) {
    fatal("unimpl.");
}

///////////////////////////////////////////////
// compare functions
///////////////////////////////////////////////

OBJ
scm_eqNr(int indexOfFirstArg) {
    OBJ firstArg, secondArg;

    checkNumberOfArguments("=", 2);

    firstArg = ARG(0);
    secondArg = ARG(1);
    if (!isInteger(firstArg)) {
	error("[=] argument is not integer:", firstArg);
    }
    if (!isInteger(secondArg)) {
	error("[=] argument is not integer:", secondArg);
    }
    POPARGS();
    return(integerVal(firstArg) == integerVal(secondArg) ? SCM_TRUE : SCM_FALSE);
}

OBJ
scm_lessNr(int indexOfFirstArg) {
    OBJ firstArg, secondArg;

    checkNumberOfArguments("<", 2);

    firstArg = ARG(0);
    secondArg = ARG(1);
    if (!isInteger(firstArg)) {
	error("[<] argument is not integer:", firstArg);
    }
    if (!isInteger(secondArg)) {
	error("[<] argument is not integer:", secondArg);
    }
    POPARGS();
    return(integerVal(firstArg) < integerVal(secondArg) ? SCM_TRUE : SCM_FALSE);
}

OBJ
scm_lessEqualNr(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_greaterNr(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_greaterEqualNr(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_eq(int indexOfFirstArg) {
    OBJ firstArg, secondArg;

    checkNumberOfArguments("eq?", 2);

    firstArg = ARG(0);
    secondArg = ARG(1);
    POPARGS();
    return(firstArg == secondArg ? SCM_TRUE : SCM_FALSE);
}

///////////////////////////////////////////////
// list functions
///////////////////////////////////////////////

OBJ
scm_cons(int indexOfFirstArg) {
    OBJ firstArg, secondArg;

    checkNumberOfArguments("cons", 2);

    firstArg = ARG(0);
    secondArg = ARG(1);
    POPARGS();
    return new_cons(firstArg, secondArg);
}

OBJ
scm_car(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("car", 1);

    arg = ARG(0);
    if (!isCons(arg)) {
	error("[car] argument is not a cons cell:", arg);
    }
    POPARGS();
    return car(arg);
}

OBJ
scm_cdr(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("cdr", 1);

    arg = ARG(0);
    if (!isCons(arg)) {
	error("[cdr] argument is not a cons cell:", arg);
    }
    POPARGS();
    return cdr(arg);
}

///////////////////////////////////////////////
// string functions
///////////////////////////////////////////////

// (string-equal s1 s2) -> boolean
//
//      error if not both args are strings
//      true if the 2 string arguments are equal (same characters),
//      false otherwise
//
OBJ
scm_stringEqual(int indexOfFirstArg) {
    checkNumberOfArguments("string-equal?", 2);

    fatal("unimpl.");
}

// (string-ref s idx) -> character
//
//      error if s is not a string
//      error if idx is not an integer
//      error if idx is < 0
//      error if idx is > (size of string minus 1)
//      return character at idx (as character object)
//
OBJ
scm_stringRef(int indexOfFirstArg) {
    checkNumberOfArguments("string-ref", 2);

    fatal("unimpl.");
}

// (string-set! s idx char) -> void
//
//      error if s is not a string
//      error if char is not a character
//      error if idx is not an integer
//      error if idx is < 0
//      error if idx is > (size of string minus 1)
//      modify string by setting character at idx to char
//      return void
//
OBJ
scm_stringSet(int indexOfFirstArg) {
    checkNumberOfArguments("string-set!", 3);

    fatal("unimpl.");
}

///////////////////////////////////////////////
// i/o functions
///////////////////////////////////////////////

OBJ
xxx_display(int indexOfFirstArg) {
    checkNumberOfArguments("display", 1);

    scm_print(ARG(0), stdout);
    POPARGS();
    return SCM_VOID;
}

OBJ
scm_load(int indexOfFirstArg) {
    char *fileName;

    checkNumberOfArguments("load", 1);

    if (!isString(ARG(0))) {
	error("[load] argument is not a string:", ARG(0));
    }
    fileName = stringVal(ARG(0));

    POPARGS();
    {
	FILE* initFile = fopen(fileName, "r");
	OBJ fileStream = new_fileStream(initFile);

	readEvalPrintLoop(fileStream, C_FALSE);
	fclose(initFile);
    }
    return SCM_VOID;
}

///////////////////////////////////////////////
// predicate functions
///////////////////////////////////////////////

// (number? o) -> boolean
//      true if o is a number (for now: an integer)
//
OBJ
scm_numberP(int indexOfFirstArg) {
    checkNumberOfArguments("number?", 1);

    return isNumber(ARG(0)) ? SCM_TRUE : SCM_FALSE;
}

OBJ
scm_integerP(int indexOfFirstArg) {
    checkNumberOfArguments("integer?", 1);

    return isInteger(ARG(0)) ? SCM_TRUE : SCM_FALSE;
}

OBJ
scm_floatP(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_stringP(int indexOfFirstArg) {
    checkNumberOfArguments("string?", 1);

    return isString(ARG(0)) ? SCM_TRUE : SCM_FALSE;
}

OBJ
scm_symbolP(int indexOfFirstArg) {
    checkNumberOfArguments("symbol?", 1);

    return isSymbol(ARG(0)) ? SCM_TRUE : SCM_FALSE;
}

OBJ
scm_consP(int indexOfFirstArg) {
    checkNumberOfArguments("cons?", 1);

    return isCons(ARG(0)) ? SCM_TRUE : SCM_FALSE;
}

void
initializeBuiltinFunctions() {
    defineBuiltinFunction("+", scm_plus);
    defineBuiltinFunction("-", scm_minus);
    defineBuiltinFunction("*", scm_times);
    defineBuiltinFunction("/", scm_quotient);

    defineBuiltinFunction("bitwise-and", scm_bitwise_and);
    defineBuiltinFunction("bitwise-or", scm_bitwise_or);
    defineBuiltinFunction("bitwise-xor", scm_bitwise_xor);

    defineBuiltinFunction("eq?", scm_eq);

    defineBuiltinFunction("=", scm_eqNr);
    defineBuiltinFunction("<", scm_lessNr);
    defineBuiltinFunction("<=", scm_lessEqualNr);
    defineBuiltinFunction(">", scm_greaterNr);
    defineBuiltinFunction(">=", scm_greaterEqualNr);

    defineBuiltinFunction("string-equal", scm_stringEqual);
    defineBuiltinFunction("string-ref", scm_stringRef);
    defineBuiltinFunction("string-set!", scm_stringSet);

    defineBuiltinFunction("cons", scm_cons);
    defineBuiltinFunction("car", scm_car);
    defineBuiltinFunction("cdr", scm_cdr);

    defineBuiltinFunction("cons?", scm_consP);
    defineBuiltinFunction("symbol?", scm_symbolP);
    defineBuiltinFunction("string?", scm_stringP);
    defineBuiltinFunction("number?", scm_numberP);
    defineBuiltinFunction("integer?", scm_integerP);
    defineBuiltinFunction("float?", scm_floatP);

    defineBuiltinFunction("display", xxx_display);
    defineBuiltinFunction("load", scm_load);
}
