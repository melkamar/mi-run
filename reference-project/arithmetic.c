#include "scheme.h"

OBJ
scm_plusFunction(int indexOfFirstArg) {
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
		error("non integer argument: ", arg);
	}
	argIndex++;
    }
    SP = indexOfFirstArg;
    return new_integer(sum);
}

OBJ
scm_minusFunction(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_timesFunction(int indexOfFirstArg) {
    fatal("unimpl.");
}

OBJ
scm_quotientFunction(int indexOfFirstArg) {
    fatal("unimpl.");
}
