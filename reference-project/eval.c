#include "scheme.h"
#include "functionPrototypes.h"

void
readEvalPrintLoop(OBJ inStream) {
    OBJ expr;
    OBJ evaluatedResult;

    expr = scm_read(inStream);
    evaluatedResult = scm_eval(expr);
    scm_print(evaluatedResult);
}

OBJ
scm_eval(OBJ expr) {
    fatal("as yet unimplemented: eval");
}
