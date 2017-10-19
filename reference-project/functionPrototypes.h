OBJ scm_read(OBJ inStream);
void scm_print(OBJ expr);
OBJ scm_eval(OBJ expr);

OBJ new_stringStream(char* buffer);
OBJ new_fileStream(FILE*);
OBJ new_integer(long iVal);

int scm_readCharacter(OBJ inStream);
int scm_peekCharacter(OBJ inStream);

void fatal(char*);
