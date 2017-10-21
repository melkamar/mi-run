#include "scheme.h"
#include <string.h>

OBJ *stack = NULL;
int SP = 0, currentStackSize = 0;

void
initializeStack() {
    stack = (OBJ *)malloc(sizeof(OBJ) * INITIAL_STACK_SIZE);
    currentStackSize = INITIAL_STACK_SIZE;
    SP = 0;
}

void
growStack() {
    currentStackSize = currentStackSize * 2;
    stack = (OBJ*)realloc(stack, sizeof(OBJ) * INITIAL_STACK_SIZE);
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
new_builtinSyntax(OBJFUNC func, char* name) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeBuiltinSyntax));
    newObj->builtinSyntax.tag = T_BUILTINSYNTAX;
    newObj->builtinSyntax.code = func;
    newObj->builtinSyntax.nameString = name;
    return newObj;
}

OBJ
new_userDefinedFunction(OBJ argList, OBJ bodyList, OBJ homeEnv) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeUserDefinedFunction));
    newObj->userDefinedFunction.tag = T_USERDEFINEDFUNCTION;
    newObj->userDefinedFunction.argList = argList;
    newObj->userDefinedFunction.bodyList = bodyList;
    newObj->userDefinedFunction.homeEnvironment = homeEnv;
    return newObj;
}

OBJ
really_new_symbol(char* symChars) {
    OBJ newObj;
    int len = (int)(strlen(symChars));

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
    int len = (int)(strlen(symChars));

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

OBJ
new_globalEnvironment(int size) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeGlobalEnvironment));
    newObj->globalEnvironment.tag = T_GLOBALENVIRONMENT;
    newObj->globalEnvironment.entries = (envEntry *)malloc(sizeof(envEntry) * size);
    memset(newObj->globalEnvironment.entries, 0, (sizeof(envEntry) * size));
    newObj->globalEnvironment.size = size;
    newObj->globalEnvironment.fillCount = 0;
    newObj->globalEnvironment.parentEnvironment = NULL;
    return newObj;
}
