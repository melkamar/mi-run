//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

//
// environments management
//
// these hold key-value bindings (associations).
// there is one global environment (obviously for globals),
// and a huge number of dynamic environments created during function
// application.
// The global environment is usualy big and uses hashing to access its slots;
// local environments typically are small (less than 5 slots), so that hashing
// may be slower than just comparing the slots sequentially.
//

#include "scheme.h"
#include <string.h>

OBJ globalEnvironment = NULL;

void
initializeGlobalEnvironment() {
    globalEnvironment = new_globalEnvironment(INITIAL_GLOBALENVIRONMENT_SIZE);
}

static void
growGlobalEnvironment(OBJ env) {
    envEntry *oldEntries = env->globalEnvironment.entries;
    int oldSize = env->globalEnvironment.size;
    int idx, newSize;

    env->globalEnvironment.size = newSize = nextPrimeAfter(oldSize * 2);

    env->globalEnvironment.entries = (envEntry *)malloc(sizeof(envEntry) * newSize);
    memset(env->globalEnvironment.entries, 0, (sizeof(envEntry) * newSize));

    for (idx = 0; idx < oldSize; idx++) {
	OBJ entriesKey = oldEntries[idx].key;

	if (entriesKey != NULL) {
	    OBJ entriesValue = oldEntries[idx].value;

	    defineGlobalValue(env, entriesKey, entriesValue);
	}
    }
}

static inline unsigned int
H(OBJ o) {
    return (unsigned int)((long)o);
}

OBJ
getGlobalValue(OBJ env, OBJ symbol) {
    for (;;) {
	unsigned int hashKey = H(symbol);
	unsigned int initialIndex = hashKey % env->globalEnvironment.size;
	unsigned int searchIndex;
	envEntry *probePtr;

	searchIndex = initialIndex;

	for (;;) {
	    probePtr = &(env->globalEnvironment.entries[ searchIndex ]);
	    if (probePtr->key == symbol) {
		return probePtr->value;
	    }
	    if (probePtr->key == NULL) {
		OBJ parent;

		if ((parent = env->globalEnvironment.parentEnvironment) == NULL) {
		    return NULL;
		}
		env = parent;
		break;
	    }
	    searchIndex = (searchIndex + 1) % env->globalEnvironment.size;
	    if (searchIndex == initialIndex) {
		fatal("oops something fishy");
	    }
	}
    }
}

void
defineGlobalValue(OBJ env, OBJ symbol, OBJ newValue) {
    unsigned int hashKey = H(symbol);
    unsigned int initialIndex = hashKey % env->globalEnvironment.size;
    unsigned int searchIndex;
    envEntry *probePtr;

    searchIndex = initialIndex;

    for (;;) {
	probePtr = &(env->globalEnvironment.entries[ searchIndex ]);
	if (probePtr->key == symbol) {
	    // redefining
	    probePtr->value = newValue;
	    return;
	}
	if (probePtr->key == NULL) {
	    // found a free slot
	    probePtr->key = symbol;
	    probePtr->value = newValue;
	    env->globalEnvironment.fillCount++;
	    if (env->globalEnvironment.fillCount > (env->globalEnvironment.size * 3 / 4)) {
		growGlobalEnvironment(env);
	    }
	    return;
	}
	searchIndex = (searchIndex + 1) % env->globalEnvironment.size;
	if (searchIndex == initialIndex) {
	    fatal("oops something fishy");
	}
    }
}

void
defineBuiltinFunction(char* name, OBJFUNC code) {
    OBJ func = new_builtinFunction(code, name);
    defineGlobalValue(globalEnvironment, new_symbol(name), func);
}

void
#ifdef RECURSIVE
defineBuiltinSyntax(char* name, OBJFUNC code)
#else
defineBuiltinSyntax(char* name, VOIDFUNCPTRFUNC code)
#endif
{
    OBJ func = new_builtinSyntax(code, name);
    defineGlobalValue(globalEnvironment, new_symbol(name), func);
}
