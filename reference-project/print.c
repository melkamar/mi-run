//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

#include "scheme.h"

static void
scm_printList(OBJ expr, FILE* outStream, bool asDisplay) {
    OBJ firstElement, restList;

    for (;;) {
	firstElement = car(expr);
	scm_printOrDisplay(firstElement, outStream, asDisplay);

	restList = cdr(expr);
	if (restList == SCM_NIL) {
	    fprintf(outStream, ")");
	    return;
	}
	if (!isCons(restList)) {
	    fprintf(outStream, " . ");
	    scm_printOrDisplay(restList, outStream, asDisplay);
	    fprintf(outStream, ")");
	    return;
	}

	fprintf(outStream, " ");
	// scm_printList(restList, outStream, asDisplay);
	expr = restList;
    }
}

//
// print and display are similar;
// display is meant for the user,
// whereas print is meant for a program.
// Strings printed by print can be reconstructed by the reader
// (they have double quotes around)
//
static void
printWithStringEscapes(char* s, FILE* outStream) {
    char ch;

    while ((ch = *s++) != '\0') {
	switch (ch) {
	    case '\n':
		putc('\\', outStream);
		ch = 'n';
		break;
	    case '\t':
		putc('\\', outStream);
		ch = 't';
		break;
	    case '\r':
		putc('\\', outStream);
		ch = 'r';
		break;
	    case '"':
	    case '\\':
		putc('\\', outStream);
		break;
	     default:
		break;
	}
	putc(ch, outStream);
    }
}

void
scm_printOrDisplay(OBJ expr, FILE* outStream, bool doDisplay) {
    switch (tagOf(expr)) {
	case T_INTEGER:
	    fprintf(outStream, "%ld", integerVal(expr));
	    return;

	case T_SYMBOL:
	    fprintf(outStream, "%s", symbolVal(expr));
	    return;

	case T_STRING:
	    if (doDisplay) {
		fprintf(outStream, "%s", stringVal(expr));
	    } else {
		fprintf(outStream, "\"");
		printWithStringEscapes(stringVal(expr), outStream);
		fprintf(outStream, "\"");
	    }
	    return;

	case T_NIL:
	    fprintf(outStream, "()");
	    return;

	case T_CONS:
	    fprintf(outStream, "(");
	    scm_printList(expr, outStream, doDisplay);
	    return;

	case T_TRUE:
	    fprintf(outStream, "#t");
	    return;

	case T_FALSE:
	    fprintf(outStream, "#f");
	    return;

	case T_VOID:
	    fprintf(outStream, "<void>");
	    return;

	case T_BUILTINFUNCTION:
	    fprintf(outStream, "<builtin function: %s>", expr->builtinFunction.nameString);
	    return;

	case T_BUILTINSYNTAX:
	    fprintf(outStream, "<builtin syntax: %s>", expr->builtinSyntax.nameString);
	    return;

	case T_USERDEFINEDFUNCTION:
	case T_BYTECODEFUNCTION:
	    fprintf(outStream, "(lambda ");
	    scm_printOrDisplay(expr->userDefinedFunction.argList, outStream, doDisplay);
	    fprintf(outStream, " ");
	    scm_printList(expr->userDefinedFunction.bodyList, outStream, doDisplay);
	    return;

	case T_ENVIRONMENT:
	    if (expr == globalEnvironment) {
		fprintf(outStream, "<the global environment>");
	    } else {
		fprintf(outStream, "<environment>");
	    }
	    return;

	default:
	    fatal("unimplemented: printing of this object");
    }

}
