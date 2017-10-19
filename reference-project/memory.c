#include "scheme.h"
#include "functionPrototypes.h"

OBJ
new_stringStream(char* buffer) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeStringStream));
    newObj->stringStream.tag = T_STRINGSTREAM;
    newObj->stringStream.theBuffer = buffer;
    newObj->stringStream.position = 0;
    return newObj;
}

OBJ
new_fileStream(FILE* f) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeFileStream));
    newObj->fileStream.tag = T_FILESTREAM;
    newObj->fileStream.theFile = f;
    newObj->fileStream.lookaheadChar = 0;
    return newObj;
}

OBJ
new_integer(long iVal) {
    OBJ newObj;

    newObj = (OBJ)malloc(sizeof(struct schemeInteger));
    newObj->integer.tag = T_INTEGER;
    newObj->integer.intVal = iVal;
    return newObj;
}
