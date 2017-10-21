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
	    if (!doDisplay) {
		fprintf(outStream, "%s", stringVal(expr));
	    } else {
		fprintf(outStream, "\"%s\"", stringVal(expr));
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
	    fprintf(outStream, "(lambda ");
	    scm_printOrDisplay(expr->userDefinedFunction.argList, outStream, doDisplay);
	    fprintf(outStream, " ");
	    scm_printList(expr->userDefinedFunction.bodyList, outStream, doDisplay);
	    return;

	case T_GLOBALENVIRONMENT:
	    fprintf(outStream, "<the global env>");
	    return;

	default:
	    fatal("unimplemented: printing of this object");
    }

}

void
scm_print(OBJ expr, FILE* outStream) {
    scm_printOrDisplay(expr, outStream, C_FALSE);
}

void
scm_display(OBJ expr, FILE* outStream) {
    scm_printOrDisplay(expr, outStream, C_TRUE);
}
