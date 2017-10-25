//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

OBJ evalBytecode(OBJ theFunction, int indexOfFirstArg);

OBJ scm_read(OBJ inStream);
void scm_printOrDisplay(OBJ expr, FILE* outStream, bool);
#define scm_print(expr, outStream)   scm_printOrDisplay(expr, outStream, C_FALSE)
#define scm_display(expr, outStream) scm_printOrDisplay(expr, outStream, C_TRUE)

OBJ scm_evalCString(char *expressionString);

#ifdef RECURSIVE
OBJ scm_eval(OBJ expr, OBJ env);
#else
OBJ trampoline(VOIDFUNCPTRFUNC fn);
VOIDFUNCPTRFUNC CP_readEvalPrintLoop();
VOIDFUNCPTRFUNC CP_eval();
#endif

OBJ new_stringStream(char* buffer);
OBJ new_fileStream(FILE* file);
OBJ new_integer(long iVal);
OBJ new_symbol(char* symbolChars);
OBJ really_new_symbol(char* symbolChars);
OBJ new_string(char* stringChars);
OBJ new_singleton(schemeTag tag);
OBJ new_cons(OBJ car, OBJ cdr);
OBJ new_builtinFunction(OBJFUNC code, char*);
#ifdef RECURSIVE
OBJ new_builtinSyntax(OBJFUNC func, char* name);
#else
OBJ new_builtinSyntax(VOIDFUNCPTRFUNC func, char* name);
#endif
OBJ new_userDefinedFunction(OBJ argList, OBJ bodyList, OBJ homeEnv);
OBJ new_environment(int size);

void growStack();
void growReturnStack();

OBJ getOldSymbolOrNil(char* symbolsChars);
void rememberSymbol(OBJ theSymbol);

int scm_readCharacter(OBJ inStream);
int scm_peekCharacter(OBJ inStream);

void fatal(char* message);
void error(char* message, OBJ optionalObjectOrNULL);
void argumentCountError(char* functionName, int numExpected, int numGiven, bool variableNumArgs);

int nextPrimeAfter(int nr);
int length(OBJ list);

void initializeSymbolTable();
void initializeGlobalEnvironment();
void initializeStack();
void initializeReturnStack();
void initializeBuiltinFunctions();
void initializeBuiltinSyntax();

OBJ getValue(OBJ env, OBJ symbol);
void defineOrSetValue(OBJ env, OBJ symbol, OBJ newValue, bool);

void defineBuiltinFunction(char* name, OBJFUNC code);
#ifdef RECURSIVE
void defineBuiltinSyntax(char* name, OBJFUNC code);
#else
void defineBuiltinSyntax(char* name, VOIDFUNCPTRFUNC code);
#endif

void selftest();

OBJ builtin_plus(int);
OBJ builtin_minus(int);
OBJ builtin_times(int);
OBJ builtin_cons(int);
