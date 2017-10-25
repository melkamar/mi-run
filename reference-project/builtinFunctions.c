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
// The return value is the return value of the C-function is.

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
builtin_plus(int indexOfFirstArg) {
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
builtin_minus(int indexOfFirstArg) {
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
builtin_times(int indexOfFirstArg) {
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
builtin_quotient(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
builtin_bitwise_and(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
builtin_bitwise_or(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
builtin_bitwise_xor(int indexOfFirstArg) {
    fatal("unimpl.");
}

///////////////////////////////////////////////
// compare functions
///////////////////////////////////////////////

OBJ
builtin_eqNr(int indexOfFirstArg) {
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
builtin_lessNr(int indexOfFirstArg) {
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
builtin_lessEqualNr(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
builtin_greaterNr(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
builtin_greaterEqualNr(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
builtin_eq(int indexOfFirstArg) {
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
builtin_cons(int indexOfFirstArg) {
    OBJ firstArg, secondArg;

    checkNumberOfArguments("cons", 2);

    firstArg = ARG(0);
    secondArg = ARG(1);
    POPARGS();
    return new_cons(firstArg, secondArg);
}

OBJ
builtin_car(int indexOfFirstArg) {
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
builtin_cdr(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("cdr", 1);

    arg = ARG(0);
    if (!isCons(arg)) {
	error("[cdr] argument is not a cons cell:", arg);
    }
    POPARGS();
    return cdr(arg);
}

//
// destructively change the car of a given cons cell
// ATTENTION: non-functional and therefore dangerous
//
OBJ
builtin_setcar(int indexOfFirstArg) {
    OBJ arg1;

    checkNumberOfArguments("set-car!", 2);

    arg1 = ARG(0);
    if (!isCons(arg1)) {
	error("[set-car!] argument is not a cons cell:", arg1);
    }
    set_car(arg1, ARG(1));
    POPARGS();
    return SCM_VOID;
}

//
// destructively change the cdr of a given cons cell
// ATTENTION: non-functional and therefore dangerous
//
OBJ
builtin_setcdr(int indexOfFirstArg) {
    OBJ arg1;

    checkNumberOfArguments("set-cdr!", 2);

    arg1 = ARG(0);
    if (!isCons(arg1)) {
	error("[set-cdr!] argument is not a cons cell:", arg1);
    }
    set_cdr(arg1, ARG(1));
    POPARGS();
    return SCM_VOID;
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
builtin_stringEqual(int indexOfFirstArg) {
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
builtin_stringRef(int indexOfFirstArg) {
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
builtin_stringSet(int indexOfFirstArg) {
    checkNumberOfArguments("string-set!", 3);

    fatal("unimpl.");
}

///////////////////////////////////////////////
// i/o functions
///////////////////////////////////////////////

static OBJ
builtin_display(int indexOfFirstArg) {
    checkNumberOfArguments("display", 1);

    scm_display(ARG(0), stdout);
    POPARGS();
    return SCM_VOID;
}

static OBJ
builtin_print(int indexOfFirstArg) {
    checkNumberOfArguments("print", 1);

    scm_print(ARG(0), stdout);
    POPARGS();
    return SCM_VOID;
}

#ifdef RECURSIVE

OBJ
builtin_load(int indexOfFirstArg) {
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

OBJ
builtin_eval(int indexOfFirstArg) {
    OBJ expr;

    checkNumberOfArguments("eval", 1);

    expr = ARG(0);
    POPARGS();
    return scm_eval(expr, globalEnvironment);
}

#endif

///////////////////////////////////////////////
// predicate functions
///////////////////////////////////////////////

// (number? o) -> boolean
//      true if o is a number (for now: an integer)
//
OBJ
builtin_numberP(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("number?", 1);

    arg = ARG(0);
    POPARGS();
    return isNumber(arg) ? SCM_TRUE : SCM_FALSE;
}

OBJ
builtin_integerP(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("integer?", 1);

    arg = ARG(0);
    POPARGS();
    return isInteger(arg) ? SCM_TRUE : SCM_FALSE;
}

OBJ
builtin_floatP(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("float?", 1);

    arg = ARG(0);
    POPARGS();
    return isFloat(arg) ? SCM_TRUE : SCM_FALSE;
}

OBJ
builtin_stringP(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("string?", 1);

    arg = ARG(0);
    POPARGS();
    return isString(arg) ? SCM_TRUE : SCM_FALSE;
}

OBJ
builtin_symbolP(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("symbol?", 1);
    arg = ARG(0);
    POPARGS();

    return isSymbol(arg) ? SCM_TRUE : SCM_FALSE;
}

OBJ
builtin_consP(int indexOfFirstArg) {
    OBJ arg;

    checkNumberOfArguments("cons?", 1);

    arg = ARG(0);
    POPARGS();
    return isCons(arg) ? SCM_TRUE : SCM_FALSE;
}

///////////////////////////////////////////////
// compiler support functions
///////////////////////////////////////////////

// (set-bytecode fn bytecode constantTable) -> void
//
static unsigned char*
convert_scheme_list_to_charArray(OBJ l) {
    int len = length(l);
    unsigned char* bytes = (unsigned char*)malloc(len);
    int idx = 0;

    while (l != SCM_NIL) {
	OBJ element = car(l);

	l = cdr(l);
	if (! isInteger(element)) {
	    error("bad bytecode element:", element);
	}
	bytes[idx++] = integerVal(element);
    }
    return bytes;
}

static OBJ*
convert_scheme_list_to_objArray(OBJ l) {
    int len = length(l);
    OBJ* constTable = (OBJ*)malloc(len * sizeof(OBJ));
    int idx = 0;

    while (l != SCM_NIL) {
	OBJ element = car(l);

	l = cdr(l);
	constTable[idx++] = element;
    }
    return constTable;
}

OBJ
builtin_setBytecode(int indexOfFirstArg) {
    OBJ fn, bc, consts;

    checkNumberOfArguments("set-bytecode", 3);

    fn = ARG(0);
    bc = ARG(1);
    consts = ARG(2);
    POPARGS();
    if (! isUserDefinedFunction(fn)) {
	error("[set-bytecode] argument is not a function:", fn);
    }
    if (! isCons(bc)) {
	error("[set-bytecode] argument is not a cons:", bc);
    }
    if ((! isCons(consts)) && ! (consts == SCM_NIL)) {
	error("[set-bytecode] argument is not a cons:", consts);
    }
    fn->userDefinedFunction.bytecode = convert_scheme_list_to_charArray(bc);
    fn->userDefinedFunction.constantTable = convert_scheme_list_to_objArray(consts);
    fn->any.tag = T_BYTECODEFUNCTION;
    return SCM_VOID;
}

OBJ
builtin_getFunctionBody(int indexOfFirstArg) {
    OBJ fn;

    checkNumberOfArguments("get-function-body", 1);

    fn = ARG(0);
    POPARGS();
    if (! isUserDefinedFunction(fn)) {
	error("[get-function-body] argument is not a function:", fn);
    }
    return fn->userDefinedFunction.bodyList;
}

OBJ
builtin_getFunctionArgList(int indexOfFirstArg) {
    OBJ fn;

    checkNumberOfArguments("get-function-args", 1);

    fn = ARG(0);
    POPARGS();
    if (! isUserDefinedFunction(fn)) {
	error("[get-function-args] argument is not a function:", fn);
    }
    return fn->userDefinedFunction.argList;
}


void
initializeBuiltinFunctions() {
    defineBuiltinFunction("+", builtin_plus);
    defineBuiltinFunction("-", builtin_minus);
    defineBuiltinFunction("*", builtin_times);
    defineBuiltinFunction("/", builtin_quotient);

    defineBuiltinFunction("bitwise-and", builtin_bitwise_and);
    defineBuiltinFunction("bitwise-or", builtin_bitwise_or);
    defineBuiltinFunction("bitwise-xor", builtin_bitwise_xor);

    defineBuiltinFunction("eq?", builtin_eq);

    defineBuiltinFunction("=", builtin_eqNr);
    defineBuiltinFunction("<", builtin_lessNr);
    defineBuiltinFunction("<=", builtin_lessEqualNr);
    defineBuiltinFunction(">", builtin_greaterNr);
    defineBuiltinFunction(">=", builtin_greaterEqualNr);

    defineBuiltinFunction("string-equal", builtin_stringEqual);
    defineBuiltinFunction("string-ref", builtin_stringRef);
    defineBuiltinFunction("string-set!", builtin_stringSet);

    defineBuiltinFunction("cons", builtin_cons);
    defineBuiltinFunction("car", builtin_car);
    defineBuiltinFunction("cdr", builtin_cdr);
    defineBuiltinFunction("set-car!", builtin_setcar);
    defineBuiltinFunction("set-cdr!", builtin_setcdr);

    defineBuiltinFunction("cons?", builtin_consP);
    defineBuiltinFunction("symbol?", builtin_symbolP);
    defineBuiltinFunction("string?", builtin_stringP);
    defineBuiltinFunction("number?", builtin_numberP);
    defineBuiltinFunction("integer?", builtin_integerP);
    defineBuiltinFunction("float?", builtin_floatP);

    defineBuiltinFunction("display", builtin_display);
    defineBuiltinFunction("print", builtin_print);
#ifdef RECURSIVE
    defineBuiltinFunction("load", builtin_load);
    defineBuiltinFunction("eval", builtin_eval);
#endif
    defineBuiltinFunction("set-bytecode", builtin_setBytecode);
    defineBuiltinFunction("get-function-body", builtin_getFunctionBody);
    defineBuiltinFunction("get-function-args", builtin_getFunctionArgList);
}
