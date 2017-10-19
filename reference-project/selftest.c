#include "scheme.h"

void
selftest() {
    OBJ o, o1, o2;
    OBJ inStream;
    long v;
    int ch;

    //
    // test streaming from a string-stream
    //
    o = new_stringStream("hello");
    assert( (tagOf(o) == T_STRINGSTREAM), "invalid tag");

    ch = scm_peekCharacter(o);
    assert( (ch == 'h'), "invalid peeking");

    ch = scm_readCharacter(o);
    assert( (ch == 'h'), "invalid reading");
    ch = scm_readCharacter(o);
    assert( (ch == 'e'), "invalid reading");
    ch = scm_readCharacter(o);
    assert( (ch == 'l'), "invalid reading");
    ch = scm_readCharacter(o);
    assert( (ch == 'l'), "invalid reading");
    ch = scm_readCharacter(o);
    assert( (ch == 'o'), "invalid reading");

    ch = scm_peekCharacter(o);
    assert( (ch == STREAM_EOF), "invalid peeking");

    ch = scm_readCharacter(o);
    assert( (ch == STREAM_EOF), "invalid reading");

    //
    // allocating integers
    //
    o = new_integer(1234);
    assert( (tagOf(o) == T_INTEGER), "invalid tag");
    assert( isInteger(o), "invalid tag");
    assert( ((v = integerVal(o)) == 1234), "invalid value");

    ////////////////////////////////////////////
    // reader tests
    //
    inStream = new_stringStream("    1234");
    o = scm_read(inStream);
    assert( (tagOf(o) == T_INTEGER), "invalid tag");
    assert( ((v = integerVal(o)) == 1234), "invalid value");

    inStream = new_stringStream("    -1234");
    o = scm_read(inStream);
    assert( (tagOf(o) == T_INTEGER), "invalid tag");
    assert( ((v = integerVal(o)) == -1234), "invalid value");

#ifdef LATER
    inStream = new_stringStream("    #x1234");
    o = scm_read(inStream);
    assert( (tagOf(o) == T_INTEGER), "invalid tag");
    assert( ((v = integerVal(o)) == 0x1234), "invalid value");
#endif

    inStream = new_stringStream("abc");
    o = scm_read(inStream);
    assert( (tagOf(o) == T_SYMBOL), "invalid tag");
    assert( (symbolSize(o) == 3), "invalid size");
    assert( streq(symbolVal(o), "abc"), "invalid value");

    inStream = new_stringStream(" \"abc\" ");
    o = scm_read(inStream);
    assert( (tagOf(o) == T_STRING), "invalid tag");
    assert( streq(symbolVal(o), "abc"), "invalid value");

    // test the symbol table

    inStream = new_stringStream(" abc ");
    o1 = scm_read(inStream);
    inStream = new_stringStream("      abc ");
    o2 = scm_read(inStream);
    assert( (o1 == o2), "symbol table code broken");

    inStream = new_stringStream(" abcd ");
    o1 = scm_read(inStream);
    inStream = new_stringStream("      abc ");
    o2 = scm_read(inStream);

    assert( (o1 != o2), "symbol table code broken");

    // create many symbols
    {
	int count;

	for (count=0; count<200; count++) {
	    char nameBuffer[64];
	    OBJ sym;

	    sprintf(nameBuffer, "sym%d", count);

	    assert( getOldSymbolOrNil(nameBuffer) == NULL, "sym should not be here");
	    sym = really_new_symbol(nameBuffer);
	    rememberSymbol(sym);

	    assert( getOldSymbolOrNil(nameBuffer) == sym, "sym should not be here");
	}
    }

    // global variables test
    {
	OBJ sym_a = new_symbol("a");
	OBJ sym_b = new_symbol("b");
	OBJ v1, v2;

	assert( getGlobalValue(sym_a) == NULL, "global a");
	assert( getGlobalValue(sym_b) == NULL, "global b");

	defineGlobalValue(sym_a, new_integer(100));
	v1 = getGlobalValue(sym_a);
	assert( isInteger(v1), "oops a");
	assert( integerVal(v1) == 100, "oops a");

	defineGlobalValue(sym_b, new_integer(200));
	v2 = getGlobalValue(sym_b);
	assert( isInteger(v2), "oops b");
	assert( integerVal(v2) == 200, "oops b");
    }
}
