#include "scheme.h"

#define BE_SILENT

static int silent = 1;

#ifndef RECURSIVE
static void trampolineTest();
#endif

#ifdef SELFTEST

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
    ASSERT( (tagOf(o) == T_STRINGSTREAM), "invalid tag");

    ch = scm_peekCharacter(o);
    ASSERT( (ch == 'h'), "invalid peeking");

    ch = scm_readCharacter(o);
    ASSERT( (ch == 'h'), "invalid reading");
    ch = scm_readCharacter(o);
    ASSERT( (ch == 'e'), "invalid reading");
    ch = scm_readCharacter(o);
    ASSERT( (ch == 'l'), "invalid reading");
    ch = scm_readCharacter(o);
    ASSERT( (ch == 'l'), "invalid reading");
    ch = scm_readCharacter(o);
    ASSERT( (ch == 'o'), "invalid reading");

    ch = scm_peekCharacter(o);
    ASSERT( (ch == STREAM_EOF), "invalid peeking");

    ch = scm_readCharacter(o);
    ASSERT( (ch == STREAM_EOF), "invalid reading");

    //
    // allocating integers
    //
    o = new_integer(1234);
    ASSERT( (tagOf(o) == T_INTEGER), "invalid tag");
    ASSERT( isInteger(o), "invalid tag");
    ASSERT( ((v = integerVal(o)) == 1234), "invalid value");

    ////////////////////////////////////////////
    // reader tests
    //
    inStream = new_stringStream("    1234");
    o = scm_read(inStream);
    ASSERT( (tagOf(o) == T_INTEGER), "invalid tag");
    ASSERT( ((v = integerVal(o)) == 1234), "invalid value");

    inStream = new_stringStream("    -1234");
    o = scm_read(inStream);
    ASSERT( (tagOf(o) == T_INTEGER), "invalid tag");
    ASSERT( ((v = integerVal(o)) == -1234), "invalid value");

# ifdef LATER
    //
    // hex numbers
    //
    inStream = new_stringStream("    #x1234");
    o = scm_read(inStream);
    ASSERT( (tagOf(o) == T_INTEGER), "invalid tag");
    ASSERT( ((v = integerVal(o)) == 0x1234), "invalid value (hex reading)");
    //
    // binary numbers
    //
    inStream = new_stringStream("    #b1010");
    o = scm_read(inStream);
    ASSERT( (tagOf(o) == T_INTEGER), "invalid tag");
    ASSERT( ((v = integerVal(o)) == 10), "invalid value (binary reading)");
# endif

    inStream = new_stringStream("abc");
    o = scm_read(inStream);
    ASSERT( (tagOf(o) == T_SYMBOL), "invalid tag");
    ASSERT( (symbolSize(o) == 3), "invalid size");
    ASSERT( streq(symbolVal(o), "abc"), "invalid value");

    inStream = new_stringStream(" \"abc\" ");
    o = scm_read(inStream);
    ASSERT( (tagOf(o) == T_STRING), "invalid tag");
    ASSERT( streq(symbolVal(o), "abc"), "invalid value");

    // test the symbol table

    inStream = new_stringStream(" abc ");
    o1 = scm_read(inStream);
    inStream = new_stringStream("      abc ");
    o2 = scm_read(inStream);
    ASSERT( (o1 == o2), "symbol table code broken");

    inStream = new_stringStream(" abcd ");
    o1 = scm_read(inStream);
    inStream = new_stringStream("      abc ");
    o2 = scm_read(inStream);

    ASSERT( (o1 != o2), "symbol table code broken");

    // create many symbols (to see if it grows correctly)
    {
	int count;

	for (count=0; count<500; count++) {
	    char nameBuffer[64];
	    OBJ sym;

	    snprintf(nameBuffer, sizeof(nameBuffer), "sym%d", count);

	    ASSERT( (getOldSymbolOrNil(nameBuffer) == NULL), "sym should not be here");
	    sym = really_new_symbol(nameBuffer);
	    rememberSymbol(sym);

	    ASSERT( getOldSymbolOrNil(nameBuffer) == sym, "sym should not be here");
	}

	// the must all be present (none lost)
	for (count=0; count<500; count++) {
	    char nameBuffer[64];
	    OBJ sym;

	    snprintf(nameBuffer, sizeof(nameBuffer), "sym%d", count);
	    sym = getOldSymbolOrNil(nameBuffer);
	    ASSERT( (sym != NULL), "sym should be present");
	    ASSERT( streq(symbolVal(sym), nameBuffer), "symbol characters wrong");

	    ASSERT( (new_symbol(nameBuffer) == sym), "oops new_symbol");
	}

	// cross check - this may not be present
	ASSERT( getOldSymbolOrNil("sym501") == NULL, "sym should not be present");
    }

    // global variables test
    {
	OBJ sym_a = new_symbol("a");
	OBJ sym_b = new_symbol("b");
	OBJ v1, v2;

	ASSERT( getGlobalValue(env, sym_a) == NULL, "global a");
	ASSERT( getGlobalValue(env, sym_b) == NULL, "global b");

	defineGlobalValue(env, sym_a, new_integer(100));
	v1 = getGlobalValue(env, sym_a);
	ASSERT( isInteger(v1), "oops a");
	ASSERT( integerVal(v1) == 100, "oops a");

	defineGlobalValue(env, sym_b, new_integer(200));
	v2 = getGlobalValue(env, sym_b);
	ASSERT( isInteger(v2), "oops b");
	ASSERT( integerVal(v2) == 200, "oops b");
    }

    // create many variable bindings (to test grow of global env)
    {
	int count;

	for (count=0; count<200; count++) {
	    char nameBuffer[64];
	    OBJ name;

	    snprintf(nameBuffer, sizeof(nameBuffer), "sym%d", count);
	    name = new_symbol(nameBuffer);
	    defineGlobalValue(env, name, new_integer(count));
	}

	// the must all be present (none lost)
	for (count=0; count<200; count++) {
	    char nameBuffer[64];
	    OBJ name, value;

	    snprintf(nameBuffer, sizeof(nameBuffer), "sym%d", count);
	    name = new_symbol(nameBuffer);
	    value = getGlobalValue(env, name);
	    ASSERT( isInteger(value), "oops not the stored value");
	    ASSERT( integerVal(value) == count, "oops not the stored value");
	}
    }

    // check builtin functions

    spBefore = SP;

    // +
    //
    PUSH( new_integer(10) );
    PUSH( new_integer(20) );
    {
	extern OBJ builtin_plus(int);
	rslt = builtin_plus(SP-2);
    }
    ASSERT(SP == spBefore, "+ did not pop args");
    ASSERT(isInteger(rslt), "+ result not integer");
    ASSERT(integerVal(rslt) == 30, "+ result wrong");

    // -
    //
    PUSH( new_integer(20) );
    PUSH( new_integer(10) );
    {
	extern OBJ builtin_minus(int);
	rslt = builtin_minus(SP-2);
    }
    ASSERT(SP == spBefore, "- did not pop args");
    ASSERT(isInteger(rslt), "- result not integer");
    ASSERT(integerVal(rslt) == 10, "- result wrong");

    // =
    //
    PUSH( new_integer(20) );
    PUSH( new_integer(10) );
    {
	extern OBJ builtin_eqNr(int);
	rslt = builtin_eqNr(SP-2);
    }
    ASSERT(SP == spBefore, "= did not pop args");
    ASSERT( rslt == SCM_FALSE, "= result wrong");

    PUSH( new_integer(20) );
    PUSH( new_integer(20) );
    {
	extern OBJ builtin_eqNr(int);
	rslt = builtin_eqNr(SP-2);
    }
    ASSERT(SP == spBefore, "= did not pop args");
    ASSERT( rslt == SCM_TRUE, "= result wrong");

    // <
    //
    PUSH( new_integer(10) );
    PUSH( new_integer(10) );
    {
	extern OBJ builtin_lessNr(int);
	rslt = builtin_lessNr(SP-2);
    }
    ASSERT(SP == spBefore, "< did not pop args");
    ASSERT( rslt == SCM_FALSE, "< result wrong");

    PUSH( new_integer(10) );
    PUSH( new_integer(20) );
    {
	extern OBJ builtin_lessNr(int);
	rslt = builtin_lessNr(SP-2);
    }
    ASSERT(SP == spBefore, "< did not pop args");
    ASSERT( rslt == SCM_TRUE, "< result wrong");

    // <
    //
    PUSH( new_integer(10) );
    PUSH( new_integer(10) );
    {
	extern OBJ builtin_lessNr(int);
	rslt = builtin_lessNr(SP-2);
    }
    ASSERT(SP == spBefore, "< did not pop args");
    ASSERT( rslt == SCM_FALSE, "< result wrong");

    PUSH( new_integer(10) );
    PUSH( new_integer(20) );
    {
	extern OBJ builtin_lessNr(int);
	rslt = builtin_lessNr(SP-2);
    }
    ASSERT(SP == spBefore, "< did not pop args");
    ASSERT( rslt == SCM_TRUE, "< result wrong");

# ifndef RECURSIVE
    trampolineTest();
# endif

    //
    // more tests, using reader and eval (see scm_evalCString)
    //
    rslt = scm_evalCString("(+ 1 2 3)");
    ASSERT( isInteger(rslt), "+ result not integer");
    ASSERT( (integerVal(rslt) == 6), "+ result wrong");

    rslt = scm_evalCString("(= (+ 1 2 3) 6)");
    ASSERT_true(rslt, "= result wrong");

    rslt = scm_evalCString("(= (car (cons 1 2)) 1)");
    ASSERT_true(rslt, "car/cons result wrong");

    rslt = scm_evalCString("(= (cdr (cons 1 2)) 2)");
    ASSERT_true(rslt, "car/cons result wrong");

    rslt = scm_evalCString("(< 10 11)");
    ASSERT_true(rslt, "< result wrong");

    rslt = scm_evalCString("(< 10 10)");
    ASSERT_false(rslt, "< result wrong");

    rslt = scm_evalCString("(< 11 10)");
    ASSERT_false(rslt, "< result wrong");


    rslt = scm_evalCString("(= (* 10 10) 100)");
    ASSERT_true(rslt, "* result wrong");

    rslt = scm_evalCString("(= (* -10 10) -100)");
    ASSERT_true(rslt, "* result wrong");

    rslt = scm_evalCString("(= (* 10 -10) -100)");
    ASSERT_true(rslt, "* result wrong");

    rslt = scm_evalCString("(= (* -10 -10) 100)");
    ASSERT_true(rslt, "* result wrong");
}

# ifndef RECURSIVE

static VOIDFUNCPTRFUNC f1();
static VOIDFUNCPTRFUNC g();
static VOIDFUNCPTRFUNC f2();
static VOIDFUNCPTRFUNC h();
static VOIDFUNCPTRFUNC i();
static VOIDFUNCPTRFUNC f();

static void
trampolineTest() {
    if (! silent) printf("testing trampoline...\n");
    (void)trampoline((VOIDFUNCPTRFUNC)f);
    if (! silent) printf("after test of trampoline...\n");
}

static VOIDFUNCPTRFUNC
f() {
    if (! silent) printf("here f\n");
    CALL(g, f1);
    // never reached
}

static VOIDFUNCPTRFUNC
f1() {
    if (! silent) printf("back in f(1)\n");
    CALL(h, f2);
    // never reached
}

static VOIDFUNCPTRFUNC
f2() {
    if (! silent) printf("back in f(2)\n");
    RETURN(SCM_TRUE);
    // never reached
}

static VOIDFUNCPTRFUNC
g() {
    if (! silent) printf("here in g\n");
    RETURN(SCM_FALSE);
    // never reached
}

static VOIDFUNCPTRFUNC
h() {
    if (! silent) printf("here in h\n");
    TCALL(i);
    // never reached
}

static VOIDFUNCPTRFUNC
i() {
    if (! silent) printf("here in i\n");
    RETURN(SCM_FALSE);
    // never reached
}

# endif // non-RECURSIVE
#endif // SELFTEST
