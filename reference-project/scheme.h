//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "parameters.h"

typedef unsigned char bool;
#define C_TRUE 1
#define C_FALSE 0

typedef enum schemeTag {
    T_UNUSED = 0,
    T_SMALLINTEGER = 1,
    T_INTEGER,
    T_BIGINTEGER,
    T_FLOAT,
    T_FRACTION,
    T_STRING,
    T_CONS,
    T_NIL,
    T_TRUE,
    T_FALSE,
    T_EOF,
    T_VOID,
    T_SYMBOL,
    T_FILESTREAM,
    T_STRINGSTREAM,
    T_BUILTINFUNCTION,
    T_BUILTINSYNTAX,
    T_BYTECODEFUNCTION,
    T_ENVIRONMENT,
    T_USERDEFINEDFUNCTION,
} schemeTag;

typedef union schemeObject *OBJ;

struct envEntry {
    OBJ key;
    OBJ value;
};
typedef struct envEntry envEntry;

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
typedef void* (*VOIDPTRFUNC)();
typedef VOIDPTRFUNC* (*VOIDFUNCPTRFUNC)();
// typedef VOIDPTRFUNC (*VOIDPTRFUNCFUNC)();

struct schemeBuiltinFunction {
    enum schemeTag tag;
    OBJFUNC code;
    char *nameString;
};

struct schemeBuiltinSyntax {
    enum schemeTag tag;
#ifdef RECURSIVE
    OBJFUNC code;
#else
    VOIDFUNCPTRFUNC code;
#endif
    char *nameString;
};

struct schemeUserDefinedFunction {
    enum schemeTag tag;
    OBJ argList;
    OBJ bodyList;
    OBJ homeEnvironment;
    unsigned char* bytecode;
    OBJ *constantTable;
};

struct schemeEnvironment {
    enum schemeTag tag;
    OBJ parentEnvironment;
    envEntry *entries;
    int size;
    int fillCount;
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
    struct schemeBuiltinSyntax builtinSyntax;
    struct schemeUserDefinedFunction userDefinedFunction;
    struct schemeEnvironment environment;

    struct schemeString20 dummy20;
};

#include "functionPrototypes.h"
#include "assertions.h"

#define STREAM_EOF  -1

/// globals
OBJ SCM_NIL;
OBJ SCM_TRUE;
OBJ SCM_FALSE;
OBJ SCM_EOF;
OBJ SCM_VOID;

extern OBJ globalEnvironment;

extern OBJ *stack;
extern int SP, currentStackSize;

extern VOIDFUNCPTRFUNC *returnStack;
extern int RSP, currentReturnStackSize;

OBJ RETVAL;

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
isFloat(OBJ o) {
    return hasTag(o, T_FLOAT);
}

static inline bool
isFraction(OBJ o) {
    return hasTag(o, T_FRACTION);
}

static inline bool
isNumber(OBJ o) {
    return isInteger(o)
	   || isFloat(o)
	   || isFraction(o);
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

static inline bool
isUserDefinedFunction(OBJ o) {
    return hasTag(o, T_USERDEFINEDFUNCTION);
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

static inline void
set_car(OBJ cons, OBJ newCar) {
    cons->cons.car = newCar;
}

static inline void
set_cdr(OBJ cons, OBJ newCar) {
    cons->cons.cdr = newCar;
}

static inline void
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

static inline void
PUSH_RET(VOIDFUNCPTRFUNC retAddr) {
    returnStack[RSP++] = retAddr;
    if (RSP >= currentReturnStackSize) {
	growReturnStack();
    }
}

static inline VOIDFUNCPTRFUNC
POP_RET() {
    if (RSP == 0) {
	fatal("oops - return stack underflow");
    }
    return returnStack[--RSP];
}

static inline void
PUSH_INT(int someInt) {
    PUSH_RET( (VOIDFUNCPTRFUNC)(long)someInt );
}

static inline int
POP_INT() {
    return (int)(long)(POP_RET());
}

#define CALL(fn, cont) \
    { \
	PUSH_RET((VOIDFUNCPTRFUNC)cont); \
	return (VOIDFUNCPTRFUNC)fn;      \
    }

#define TCALL(fn) \
    { \
	return (VOIDFUNCPTRFUNC)fn;      \
    }

#define JUMP(fn) \
    { \
	return (VOIDFUNCPTRFUNC)fn;      \
    }

#define RETURN(retVal) \
    { \
	RETVAL = retVal; \
	return (VOIDFUNCPTRFUNC)POP_RET();      \
    }
