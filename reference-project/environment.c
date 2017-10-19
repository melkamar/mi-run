#include "scheme.h"
#include <string.h>

#define INITIAL_GLOBALENVIRONMENT_SIZE    67

struct envEntry {
    OBJ key;
    OBJ value;
};
typedef struct envEntry envEntry;

envEntry *globalEnvironment = NULL;
static int globalEnvironmentSize;
static int globalEnvironmentFillcount;

static void
allocateGlobalEnvironment(int size) {
    globalEnvironmentSize = size;
    globalEnvironmentFillcount = 0;
    globalEnvironment = (envEntry *)malloc(sizeof(envEntry) * size);
    memset(globalEnvironment, 0, (sizeof(envEntry) * size));
}

void
initializeGlobalEnvironment() {
    allocateGlobalEnvironment(INITIAL_GLOBALENVIRONMENT_SIZE);
}

static void
growGlobalEnvironment() {
    fatal("unimpl");
}

static inline long
H(OBJ o) {
    return (long)o;
}

OBJ
getGlobalValue(OBJ symbol) {
    unsigned int hashKey = H(symbol);
    unsigned int initialIndex = hashKey % globalEnvironmentSize;
    unsigned int searchIndex;
    envEntry *probePtr;

    searchIndex = initialIndex;

    for (;;) {
	probePtr = &(globalEnvironment[ searchIndex ]);
	if (probePtr->key == symbol) {
	    return probePtr->value;
	}
	if (probePtr->key == NULL) {
	    return NULL;
	}
	searchIndex = (searchIndex + 1) % globalEnvironmentSize;
	if (searchIndex == initialIndex) {
	    fatal("oops something fishy");
	}
    }
}

void
defineGlobalValue(OBJ symbol, OBJ newValue) {
    unsigned int hashKey = H(symbol);
    unsigned int initialIndex = hashKey % globalEnvironmentSize;
    unsigned int searchIndex;
    envEntry *probePtr;

    searchIndex = initialIndex;

    for (;;) {
	probePtr = &(globalEnvironment[ searchIndex ]);
	if (probePtr->key == NULL) {
	    // found a free slot
	    probePtr->key = symbol;
	    probePtr->value = newValue;
	    globalEnvironmentFillcount++;
	    if (globalEnvironmentFillcount > (globalEnvironmentSize * 3 / 4)) {
		growGlobalEnvironment();
	    }
	    return;
	}
	searchIndex = (searchIndex + 1) % globalEnvironmentSize;
	if (searchIndex == initialIndex) {
	    fatal("oops something fishy");
	}
    }
}

void
defineBuiltinFunction(char* name, OBJFUNC code) {
    OBJ func = new_builtinFunction(code, name);
    defineGlobalValue(new_symbol(name), func);
}
