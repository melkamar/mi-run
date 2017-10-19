#include "scheme.h"
#include <string.h>

OBJ *stack = NULL;
int SP, currentStackSize;

void
initializeStack() {
    stack = (OBJ *)malloc(sizeof(OBJ) * INITIAL_STACK_SIZE);
    currentStackSize = INITIAL_STACK_SIZE;
    SP = 0;
}

void
growStack() {
    fatal("as yet unimpl");
}

OBJ
new_stringStream(char* buffer) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeStringStream));
    newObj->stringStream.tag = T_STRINGSTREAM;
    newObj->stringStream.theBuffer = buffer;
    newObj->stringStream.position = 0;
    return newObj;
}

OBJ
new_fileStream(FILE* f) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeFileStream));
    newObj->fileStream.tag = T_FILESTREAM;
    newObj->fileStream.theFile = f;
    newObj->fileStream.lookaheadChar = 0;
    return newObj;
}

OBJ
new_integer(long iVal) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeInteger));
    newObj->integer.tag = T_INTEGER;
    newObj->integer.intVal = iVal;
    return newObj;
}

OBJ
new_builtinFunction(OBJFUNC func, char* name) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeBuiltinFunction));
    newObj->builtinFunction.tag = T_BUILTINFUNCTION;
    newObj->builtinFunction.code = func;
    newObj->builtinFunction.nameString = name;
    return newObj;
}

OBJ
really_new_symbol(char* symChars) {
    OBJ newObj;
    int len = strlen(symChars);

    newObj = (OBJ)malloc(sizeof(struct schemeSymbol) + len - 1 /* one too many - see struct */
							   + 1 /* zero byte */ );
    newObj->symbol.tag = T_SYMBOL;
    strcpy(newObj->symbol.symbolVal, symChars);
    newObj->symbol.symbolSize = len;
    return newObj;
}

OBJ
new_symbol(char* symChars) {
    OBJ sym = getOldSymbolOrNil(symChars);

    if (sym == NULL) {
	sym = really_new_symbol(symChars);
	rememberSymbol(sym);
    }
    return sym;
}

OBJ
new_string(char* symChars) {
    OBJ newObj;
    int len = strlen(symChars);

    newObj = (OBJ)malloc(sizeof(struct schemeSymbol) + len - 1 /* one too many - see struct */
							   + 1 /* zero byte */ );
    newObj->symbol.tag = T_STRING;
    strcpy(newObj->symbol.symbolVal, symChars);
    newObj->symbol.symbolSize = len;
    return newObj;
}

OBJ
new_singleton(schemeTag tag) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeAny));
    newObj->any.tag = tag;
    return newObj;
}

OBJ
new_cons(OBJ car, OBJ cdr) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeCons));
    newObj->cons.tag = T_CONS;
    newObj->cons.car = car;
    newObj->cons.cdr = cdr;
    return newObj;
}
