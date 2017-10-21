#include "scheme.h"

static VOIDPTRFUNC f();

void
selftest() {
    OBJ o, o1, o2, rslt;
    OBJ inStream;
    long v;
    int ch;
    int spBefore;
    OBJ env = globalEnvironment;

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

    // create many symbols (to see if it grows correctly)
    {
	int count;

	for (count=0; count<500; count++) {
	    char nameBuffer[64];
	    OBJ sym;

	    sprintf(nameBuffer, "sym%d", count);

	    assert( getOldSymbolOrNil(nameBuffer) == NULL, "sym should not be here");
	    sym = really_new_symbol(nameBuffer);
	    rememberSymbol(sym);

	    assert( getOldSymbolOrNil(nameBuffer) == sym, "sym should not be here");
	}

	// the must all be present (none lost)
	for (count=0; count<500; count++) {
	    char nameBuffer[64];
	    OBJ sym;

	    sprintf(nameBuffer, "sym%d", count);
	    sym = getOldSymbolOrNil(nameBuffer);
	    assert( sym != NULL, "sym should be present");
	    assert( streq(symbolVal(sym), nameBuffer), "symbol characters wrong");

	    assert( new_symbol(nameBuffer) == sym, "oops new_symbol");
	}

	// cross check - this may not be present
	assert( getOldSymbolOrNil("sym501") == NULL, "sym should not be present");
    }

    // global variables test
    {
	OBJ sym_a = new_symbol("a");
	OBJ sym_b = new_symbol("b");
	OBJ v1, v2;

	assert( getGlobalValue(env, sym_a) == NULL, "global a");
	assert( getGlobalValue(env, sym_b) == NULL, "global b");

	defineGlobalValue(env, sym_a, new_integer(100));
	v1 = getGlobalValue(env, sym_a);
	assert( isInteger(v1), "oops a");
	assert( integerVal(v1) == 100, "oops a");

	defineGlobalValue(env, sym_b, new_integer(200));
	v2 = getGlobalValue(env, sym_b);
	assert( isInteger(v2), "oops b");
	assert( integerVal(v2) == 200, "oops b");
    }

    // create many variable bindings (to test grow of global env)
    {
	int count;

	for (count=0; count<200; count++) {
	    char nameBuffer[64];
	    OBJ name;

	    sprintf(nameBuffer, "sym%d", count);
	    name = new_symbol(nameBuffer);
	    defineGlobalValue(env, name, new_integer(count));
	}

	// the must all be present (none lost)
	for (count=0; count<200; count++) {
	    char nameBuffer[64];
	    OBJ name, value;

	    sprintf(nameBuffer, "sym%d", count);
	    name = new_symbol(nameBuffer);
	    value = getGlobalValue(env, name);
	    assert( isInteger(value), "oops not the stored value");
	    assert( integerVal(value) == count, "oops not the stored value");
	}
    }

    // check builtin functions

    spBefore = SP;

    // +
    //
    PUSH( new_integer(10) );
    PUSH( new_integer(20) );
    {
	extern OBJ scm_plus(int);
	rslt = scm_plus(SP-2);
    }
    assert(SP == spBefore, "+ did not pop args");
    assert(isInteger(rslt), "+ result not integer");
    assert(integerVal(rslt) == 30, "+ result wrong");

    // -
    //
    PUSH( new_integer(20) );
    PUSH( new_integer(10) );
    {
	extern OBJ scm_minus(int);
	rslt = scm_minus(SP-2);
    }
    assert(SP == spBefore, "- did not pop args");
    assert(isInteger(rslt), "- result not integer");
    assert(integerVal(rslt) == 10, "- result wrong");

    // =
    //
    PUSH( new_integer(20) );
    PUSH( new_integer(10) );
    {
	extern OBJ scm_eqNr(int);
	rslt = scm_eqNr(SP-2);
    }
    assert(SP == spBefore, "= did not pop args");
    assert( rslt == SCM_FALSE, "= result wrong");

    PUSH( new_integer(20) );
    PUSH( new_integer(20) );
    {
	extern OBJ scm_eqNr(int);
	rslt = scm_eqNr(SP-2);
    }
    assert(SP == spBefore, "= did not pop args");
    assert( rslt == SCM_TRUE, "= result wrong");

    // <
    //
    PUSH( new_integer(10) );
    PUSH( new_integer(10) );
    {
	extern OBJ scm_lessNr(int);
	rslt = scm_lessNr(SP-2);
    }
    assert(SP == spBefore, "< did not pop args");
    assert( rslt == SCM_FALSE, "< result wrong");

    PUSH( new_integer(10) );
    PUSH( new_integer(20) );
    {
	extern OBJ scm_lessNr(int);
	rslt = scm_lessNr(SP-2);
    }
    assert(SP == spBefore, "< did not pop args");
    assert( rslt == SCM_TRUE, "< result wrong");

    // <
    //
    PUSH( new_integer(10) );
    PUSH( new_integer(10) );
    {
	extern OBJ scm_lessNr(int);
	rslt = scm_lessNr(SP-2);
    }
    assert(SP == spBefore, "< did not pop args");
    assert( rslt == SCM_FALSE, "< result wrong");

    PUSH( new_integer(10) );
    PUSH( new_integer(20) );
    {
	extern OBJ scm_lessNr(int);
	rslt = scm_lessNr(SP-2);
    }
    assert(SP == spBefore, "< did not pop args");
    assert( rslt == SCM_TRUE, "< result wrong");

    // more tests, using reader and eval (see scm_evalCString)
    //
    rslt = scm_evalCString("(+ 1 2 3)");
    assert(isInteger(rslt), "+ result not integer");
    assert(integerVal(rslt) == 6, "+ result wrong");

    rslt = scm_evalCString("(= (+ 1 2 3) 6)");
    assert_true(rslt, "= result wrong");

    rslt = scm_evalCString("(= (car (cons 1 2)) 1)");
    assert_true(rslt, "car/cons result wrong");

    rslt = scm_evalCString("(= (cdr (cons 1 2)) 2)");
    assert_true(rslt, "car/cons result wrong");

    rslt = scm_evalCString("(< 10 11)");
    assert_true(rslt, "< result wrong");

    rslt = scm_evalCString("(< 10 10)");
    assert_false(rslt, "< result wrong");

    rslt = scm_evalCString("(< 11 10)");
    assert_false(rslt, "< result wrong");


    rslt = scm_evalCString("(= (* 10 10) 100)");
    assert_true(rslt, "* result wrong");

    rslt = scm_evalCString("(= (* -10 10) -100)");
    assert_true(rslt, "* result wrong");

    rslt = scm_evalCString("(= (* 10 -10) -100)");
    assert_true(rslt, "* result wrong");

    rslt = scm_evalCString("(= (* -10 -10) 100)");
    assert_true(rslt, "* result wrong");

    //
    printf("testing trampoline...\n");
    {
	VOIDPTRFUNC f();

	trampoline(f);
	printf("after test of trampoline...\n");
    }
}

static VOIDPTRFUNC f1();
static VOIDPTRFUNC g();
static VOIDPTRFUNC f2();
static VOIDPTRFUNC h();
static VOIDPTRFUNC i();

static VOIDPTRFUNC
f() {
    printf("here f\n");
    CALL(g, f1);
    // never reached
}

static VOIDPTRFUNC
f1() {
    printf("back in f(1)\n");
    CALL(h, f2);
    // never reached
}

static VOIDPTRFUNC
f2() {
    printf("back in f(2)\n");
    RETURN(SCM_TRUE);
    // never reached
}

static VOIDPTRFUNC
g() {
    printf("here in g\n");
    RETURN(SCM_FALSE);
    // never reached
}

static VOIDPTRFUNC
h() {
    printf("here in h\n");
    TCALL(i);
    // never reached
}

static VOIDPTRFUNC
i() {
    printf("here in i\n");
    RETURN(SCM_FALSE);
    // never reached
}
