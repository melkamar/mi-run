OBJ scm_read(OBJ inStream);
void scm_print(OBJ expr);
OBJ scm_eval(OBJ expr);

OBJ new_stringStream(char* buffer);
OBJ new_fileStream(FILE*);
OBJ new_integer(long iVal);
OBJ new_symbol(char*);
OBJ really_new_symbol(char*);
OBJ new_string(char*);
OBJ new_singleton(schemeTag);
OBJ new_cons(OBJ, OBJ);
OBJ new_builtinFunction(OBJFUNC, char*);

OBJ scm_plusFunction();
OBJ scm_minusFunction();
OBJ scm_timesFunction();
OBJ scm_quotientFunction();
OBJ scm_consFunction();
OBJ scm_carFunction();
OBJ scm_cdrFunction();

OBJ getOldSymbolOrNil(char*);
void rememberSymbol(OBJ);

int scm_readCharacter(OBJ inStream);
int scm_peekCharacter(OBJ inStream);

void fatal(char*);

void initializeSymbolTable();
void initializeGlobalEnvironment();
void initializeStack();

OBJ getGlobalValue(OBJ symbol);
void defineGlobalValue(OBJ symbol, OBJ newValue);
void defineBuiltinFunction(char* name, OBJFUNC code);
