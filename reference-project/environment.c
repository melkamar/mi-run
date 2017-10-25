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
    globalEnvironment = new_environment(INITIAL_GLOBALENVIRONMENT_SIZE);
}

static void
growEnvironment(OBJ env) {
    envEntry *oldEntries = env->environment.entries;
    int oldSize = env->environment.size;
    int idx, newSize;

    env->environment.size = newSize = nextPrimeAfter(oldSize * 2);

    env->environment.entries = (envEntry *)malloc(sizeof(envEntry) * newSize);
    memset(env->environment.entries, 0, (sizeof(envEntry) * newSize));

    for (idx = 0; idx < oldSize; idx++) {
	OBJ entriesKey = oldEntries[idx].key;

	if (entriesKey != NULL) {
	    OBJ entriesValue = oldEntries[idx].value;

	    defineOrSetValue(env, entriesKey, entriesValue, C_TRUE);
	}
    }
}

static inline unsigned int
H(OBJ o) {
    return (unsigned int)((long)o);
}

OBJ
getValue(OBJ env, OBJ symbol) {
    for (;;) {
	unsigned int hashKey = H(symbol);
	unsigned int initialIndex = hashKey % env->environment.size;
	unsigned int searchIndex;
	envEntry *probePtr;

	searchIndex = initialIndex;

	for (;;) {
	    probePtr = &(env->environment.entries[ searchIndex ]);
	    if (probePtr->key == symbol) {
		return probePtr->value;
	    }
	    if (probePtr->key == NULL) {
		OBJ parent;

		if ((parent = env->environment.parentEnvironment) == NULL) {
		    return NULL;
		}
		env = parent;
		break;
	    }
	    searchIndex = (searchIndex + 1) % env->environment.size;
	    if (searchIndex == initialIndex) {
		fatal("oops something fishy");
	    }
	}
    }
}

//
// common function for define and set!
//
void
defineOrSetValue(OBJ env, OBJ symbol, OBJ newValue, bool defineIfUndefined) {
again:
    {
	unsigned int hashKey = H(symbol);
	unsigned int initialIndex = hashKey % env->environment.size;
	unsigned int searchIndex;
	envEntry *probePtr;

	searchIndex = initialIndex;

	for (;;) {
	    probePtr = &(env->environment.entries[ searchIndex ]);
	    if (probePtr->key == symbol) {
		// redefining
		probePtr->value = newValue;
		return;
	    }
	    if (probePtr->key == NULL) {
		if (! defineIfUndefined) {
		    if (env->environment.parentEnvironment == NULL) {
			error("[set!]: undefined:", symbol);
		    }
		    env = env->environment.parentEnvironment;
		    goto again;
		}

		// found a free slot
		probePtr->key = symbol;
		probePtr->value = newValue;
		env->environment.fillCount++;
		if (env->environment.fillCount > (env->environment.size * 3 / 4)) {
		    growEnvironment(env);
		}
		return;
	    }
	    searchIndex = (searchIndex + 1) % env->environment.size;
	    if (searchIndex == initialIndex) {
		fatal("oops something fishy");
	    }
	}
    }
}

void
defineBuiltinFunction(char* name, OBJFUNC code) {
    OBJ func = new_builtinFunction(code, name);
    defineOrSetValue(globalEnvironment, new_symbol(name), func, C_TRUE);
}

void
#ifdef RECURSIVE
defineBuiltinSyntax(char* name, OBJFUNC code)
#else
defineBuiltinSyntax(char* name, VOIDFUNCPTRFUNC code)
#endif
{
    OBJ func = new_builtinSyntax(code, name);
    defineOrSetValue(globalEnvironment, new_symbol(name), func, C_TRUE);
}
