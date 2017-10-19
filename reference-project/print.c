#include "scheme.h"

static void
scm_printList(OBJ expr) {
    OBJ firstElement, restList;

    for (;;) {
	firstElement = car(expr);
	scm_print(firstElement);

	restList = cdr(expr);
	if (restList == SCM_NIL) {
	    printf(")");
	    return;
	}
	printf(" ");

	// scm_printList(restList);
	expr = restList;
    }
}

void
scm_print(OBJ expr) {
    switch (tagOf(expr)) {
	case T_INTEGER:
	    printf("%ld", integerVal(expr));
	    return;

	case T_SYMBOL:
	    printf("%s", symbolVal(expr));
	    return;

	case T_STRING:
	    printf("\"%s\"", stringVal(expr));
	    return;

	case T_NIL:
	    printf("()", stringVal(expr));
	    return;

	case T_CONS:
	    printf("(");
	    scm_printList(expr);
	    return;

	case T_BUILTINFUNCTION:
	    printf("<builtin function: %s>", expr->builtinFunction.nameString);
	    return;

	default:
	    fatal("unimlemented");
    }

}
