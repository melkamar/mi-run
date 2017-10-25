//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

#include "scheme.h"

void
fatal(char* message) {
    fprintf(stderr, "fatal: %s\n", message);
    abort();
}

void
error(char* message, OBJ objectOrNULL) {
    fprintf(stderr, "error: %s", message);
    if (objectOrNULL != NULL) {
	fprintf(stderr, " ");
	scm_print(objectOrNULL, stderr);
    }
    fprintf(stderr, "\n");
    backIntoMain();
}

void
argumentCountError(char* functionName, int numExpected, int numGiven, bool variableNumArgs) {
    char msg[256];

    snprintf(msg, sizeof(msg),
	     "[%s] expects %s%d arguments (%d given)",
	     functionName,
	     (variableNumArgs ? "at least " : ""),
	     numExpected, numGiven);
    error(msg, NULL);
}

// get this table with:
//  1 to:25 collect:[:p| (2 raisedTo:p) nextPrime]

static const int primes[] = {
    67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771,
    65537, 131101, 262147, 524309, -1
};

int
nextPrimeAfter(int nr) {
    int primeIdx;
    int nextPrime;

    primeIdx=0;
    do {
	nextPrime = primes[primeIdx++];

	if (nextPrime > nr) {
	    return nextPrime;
	}
    } while (nextPrime != -1);

    // ouch - a huge number
    return (nr | 1);
}

//
// the length of a list
// (if degenerated, only the length of the proper list is returned)
//
int
length(OBJ list) {
    int len = 0;

    while (list != SCM_NIL) {
	len++;
	list = cdr(list);
	if (! isCons(list)) {
	    break;
	}
    }
    return len;
}
