#include <stdio.h>
#include <stdlib.h>

typedef enum schemeTag {
    T_INTEGER,
    // T_FLOAT,
    T_STRING,
    T_CONS,
    T_NIL,
    T_TRUE,
    T_FALSE,
    T_SYMBOL,
    T_FILESTREAM,
    T_STRINGSTREAM,
} schemeTag;

typedef union schemeObject *OBJ;

struct schemeInteger {
    enum schemeTag tag;
    long intVal;
};

struct schemeString {
    enum schemeTag tag;
    char stringVal[1];
};

struct schemeSymbol {
    enum schemeTag tag;
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
};

// #define integerVal(o)   ((o)->u.intVal)

static inline schemeTag
tagOf(OBJ o) {
    return o->any.tag;
}

static inline long
integerVal(OBJ o) {
    return o->integer.intVal;
}

#define STREAM_EOF  -1

#define _assert(cond, msg, filename, lineNr) \
    { \
	if (! (cond)) { \
	    fprintf(stderr, "%s[%d]: assertion failed: %s\n", filename, lineNr, msg); \
	    abort(); \
	} \
    }

#define assert(cond, message) _assert(cond, message, __FILE__, __LINE__)
