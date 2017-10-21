//
// symbols management
//
// the symbolTable here remembers ALL ever created symbols,
// and - if asked for a symbol - is first consulted to check if such
// a symbol already exists, before a new one is created.
//
// Therefore, two symbols with the same characters are always eq (identical),
// and symbol comparison (hashing) can therefore be done on the identity instead
// if the equality (contents), which is much faster.
//

#include "scheme.h"
#include <string.h>

OBJ *symbolTable = NULL;
static int currentSymbolTableSize;
static int currentSymbolTableFillcount;

static void
allocateSymbolTable(int size) {
    currentSymbolTableSize = size;
    symbolTable = (OBJ *)malloc(sizeof(OBJ) * currentSymbolTableSize);
    memset(symbolTable, 0, (sizeof(OBJ) * currentSymbolTableSize));
}

void
initializeSymbolTable() {
    allocateSymbolTable(INITIAL_SYMBOLTABLE_SIZE);
    currentSymbolTableFillcount = 0;
}

#define USE_REAL_HASH

static inline unsigned int
H(char *cp) {
    unsigned int hVal = 0;
    char c;

#ifdef USE_REAL_HASH
    while ((c = *cp++) != '\0') {
	hVal = (hVal*67)+(c & 0xFF);
    }
#endif
    return(hVal);
}

static void
growSymbolTable() {
    OBJ *oldSymbols = symbolTable;
    int oldSymbolTableSize = currentSymbolTableSize;
    int idx;
    int newSize = nextPrimeAfter(oldSymbolTableSize * 2);

    allocateSymbolTable(newSize);
    for (idx = 0; idx < oldSymbolTableSize; idx++) {
	OBJ oldSym = oldSymbols[idx];

	if (oldSym != NULL) {
	    rememberSymbol(oldSym);
	}
    }
}

OBJ
getOldSymbolOrNil(char* chars) {
    unsigned int hashKey = H(chars);
    unsigned int initialIndex = hashKey % currentSymbolTableSize;
    unsigned int searchIndex;
    OBJ probe;

    searchIndex = initialIndex;

    for (;;) {
	probe = symbolTable[ searchIndex ];
	if (probe == NULL) {
	    return NULL;
	}
	if (streq(symbolVal(probe), chars)) {
	    return probe;
	}
	searchIndex = (searchIndex + 1) % currentSymbolTableSize;
	if (searchIndex == initialIndex) {
	    fatal("oops something fishy");
	}
    }
}

void
rememberSymbol(OBJ symbol) {
    unsigned int hashKey = H(symbolVal(symbol));
    unsigned int initialIndex = hashKey % currentSymbolTableSize;
    unsigned int searchIndex;
    OBJ probe;

    searchIndex = initialIndex;

    for (;;) {
	probe = symbolTable[ searchIndex ];
	if (probe == NULL) {
	    // found a free slot
	    symbolTable[ searchIndex ] = symbol;
	    currentSymbolTableFillcount++;
	    if (currentSymbolTableFillcount > (currentSymbolTableSize * 3 / 4)) {
		growSymbolTable();
	    }
	    return;
	}
	searchIndex = (searchIndex + 1) % currentSymbolTableSize;
	if (searchIndex == initialIndex) {
	    fatal("oops something fishy");
	}
    }
}
