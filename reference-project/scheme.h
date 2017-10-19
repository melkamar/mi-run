#include <stdio.h>
#include <stdlib.h>

typedef unsigned char bool;
#define C_TRUE 1
#define C_FALSE 0

typedef enum schemeTag {
    T_INTEGER,
    // T_FLOAT,
    T_STRING,
    T_CONS,
    T_NIL,
    T_TRUE,
    T_FALSE,
    T_EOF,
    T_SYMBOL,
    T_FILESTREAM,
    T_STRINGSTREAM,
    T_BUILTINFUNCTION,
} schemeTag;

typedef union schemeObject *OBJ;

struct schemeInteger {
    enum schemeTag tag;
    long intVal;
};

struct schemeString {
    enum schemeTag tag;
    int stringSize;
    char stringVal[1];
};

struct schemeString20 {
    enum schemeTag tag;
    int stringSize;
    char stringVal[20];
};

struct schemeSymbol {
    enum schemeTag tag;
    int symbolSize;
    char symbolVal[1];
};

struct schemeCons {
    enum schemeTag tag;
    OBJ car;
    OBJ cdr;
};

struct schemeFileStream {
    enum schemeTag tag;
    int lookaheadChar;
    FILE* theFile;
};

struct schemeStringStream {
    enum schemeTag tag;
    char* theBuffer;
    int position;
};

typedef OBJ (*OBJFUNC)();
// typedef void (*VOIDFUNC)();
// typedef void* (*VOIDPTRFUNC)();
// typedef VOIDPTRFUNC (*VOIDPTRFUNCFUNC)();

struct schemeBuiltinFunction {
    enum schemeTag tag;
    OBJFUNC code;
    char *nameString;
};

struct schemeAny {
    enum schemeTag tag;
};

union schemeObject {
    struct schemeInteger integer;
    struct schemeString string;
    struct schemeSymbol symbol;
    struct schemeCons cons;
    struct schemeFileStream fileStream;
    struct schemeStringStream stringStream;
    struct schemeAny any;
    struct schemeBuiltinFunction builtinFunction;

    struct schemeString20 dummy20;
};

// #define integerVal(o)   ((o)->u.intVal)

static inline bool
streq(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

static inline schemeTag
tagOf(OBJ o) {
    return o->any.tag;
}

static inline bool
hasTag(OBJ o, schemeTag tag) {
    return (tagOf(o) == tag);
}

static inline bool
isInteger(OBJ o) {
    return hasTag(o, T_INTEGER);
}

static inline bool
isString(OBJ o) {
    return hasTag(o, T_STRING);
}

static inline bool
isCons(OBJ o) {
    return hasTag(o, T_CONS);
}

static inline bool
isSymbol(OBJ o) {
    return hasTag(o, T_SYMBOL);
}


static inline long
integerVal(OBJ o) {
    return o->integer.intVal;
}

static inline char*
stringVal(OBJ o) {
    return o->string.stringVal;
}

static inline char*
symbolVal(OBJ o) {
    return o->symbol.symbolVal;
}

static inline int
symbolSize(OBJ o) {
    return o->symbol.symbolSize;
}

static inline OBJ
car(OBJ o) {
    return o->cons.car;
}

static inline OBJ
cdr(OBJ o) {
    return o->cons.cdr;
}

#include "functionPrototypes.h"

#define STREAM_EOF  -1

#define _assert(cond, msg, filename, lineNr) \
    { \
	if (! (cond)) { \
	    fprintf(stderr, "%s[%d]: assertion failed: %s\n", filename, lineNr, msg); \
	    abort(); \
	} \
    }

#define assert(cond, message) _assert(cond, message, __FILE__, __LINE__)

/// globals
OBJ SCM_NIL;
OBJ SCM_TRUE;
OBJ SCM_FALSE;
OBJ SCM_EOF;
extern OBJ *stack;
extern int SP, currentStackSize;

extern void growStack();

static inline
PUSH(OBJ o) {
    stack[SP++] = o;
    if (SP >= currentStackSize) {
	growStack();
    }
}

static inline OBJ
POP() {
    if (SP == 0) {
	fatal("oops - stack underflow");
    }
    return stack[--SP];
}

#include "parameters.h"
