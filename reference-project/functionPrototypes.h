OBJ scm_read(OBJ inStream);
void scm_print(OBJ expr, FILE* outStream);
void scm_display(OBJ expr, FILE* outStream);
void scm_printOrDisplay(OBJ expr, FILE* outStream, bool);

OBJ scm_eval(OBJ expr, OBJ env);
OBJ scm_evalCString(char *expressionString);

OBJ new_stringStream(char* buffer);
OBJ new_fileStream(FILE*);
OBJ new_integer(long iVal);
OBJ new_symbol(char*);
OBJ really_new_symbol(char*);
OBJ new_string(char*);
OBJ new_singleton(schemeTag);
OBJ new_cons(OBJ, OBJ);
OBJ new_builtinFunction(OBJFUNC code, char*);
OBJ new_builtinSyntax(OBJFUNC code, char*);
OBJ new_userDefinedFunction(OBJ argList, OBJ bodyList, OBJ homeEnv);
OBJ new_globalEnvironment(int size);

void growStack();

OBJ getOldSymbolOrNil(char* symbolsChars);
void rememberSymbol(OBJ theSymbol);

int scm_readCharacter(OBJ inStream);
int scm_peekCharacter(OBJ inStream);

void fatal(char* message);
void error(char* message, OBJ optionalObjectOrNULL);
void argumentCountError(char* functionName, int numExpected, int numGiven, bool variableNumArgs);

int nextPrimeAfter(int nr);

void initializeSymbolTable();
void initializeGlobalEnvironment();
void initializeStack();
void initializeBuiltinFunctions();
void initializeBuiltinSyntax();

OBJ getGlobalValue(OBJ env, OBJ symbol);
void defineGlobalValue(OBJ env, OBJ symbol, OBJ newValue);

void defineBuiltinFunction(char* name, OBJFUNC code);
void defineBuiltinSyntax(char* name, OBJFUNC code);
