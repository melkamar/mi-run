#include "scheme.h"
#include <string.h>

int
scm_readOrPeekCharacter(OBJ inStream, bool doRead) {
    switch (tagOf(inStream)) {
	case T_FILESTREAM:
	    {
		FILE *f;
		int retChar;

		if ((retChar = inStream->fileStream.lookaheadChar) != 0) {
		    if (doRead) {
			inStream->fileStream.lookaheadChar = 0;
		    }
		    return retChar;
		}

		f = inStream->fileStream.theFile;
		if (feof(f)) {
		    return STREAM_EOF;
		}
		retChar = getc(f);
		if (!doRead) {
		    inStream->fileStream.lookaheadChar = retChar;
		}
		return retChar;
	    }

	case T_STRINGSTREAM:
	    {
		int pos = inStream->stringStream.position;
		char *buffer = inStream->stringStream.theBuffer;
		int retChar;

		if (buffer[pos] == '\0') {
		    return STREAM_EOF;
		}
		retChar = (int)buffer[pos];
		if (doRead) {
		    inStream->stringStream.position = pos + 1;
		}
		return retChar;
	    }

	default:
	    error("argument is not a stream:", inStream);
    }
}

int
scm_readCharacter(OBJ inStream) {
    return scm_readOrPeekCharacter(inStream, C_TRUE);
}

int
scm_peekCharacter(OBJ inStream) {
    return scm_readOrPeekCharacter(inStream, C_FALSE);
}

void
scm_advanceReadPointer(OBJ inStream) {
    switch (tagOf(inStream)) {
	case T_FILESTREAM:
	    if (inStream->fileStream.lookaheadChar != 0) {
		inStream->fileStream.lookaheadChar = 0;
	    } else {
		FILE *f = inStream->fileStream.theFile;

		if (! feof(f)) {
		    getc(f);
		}
	    }
	    return;

	case T_STRINGSTREAM:
	    {
		int pos = inStream->stringStream.position;
		char *buffer = inStream->stringStream.theBuffer;

		if (buffer[pos] == '\0') {
		    return;
		}
		inStream->stringStream.position = pos + 1;
		return;
	    }

	default:
	    error("argument is not a stream:", inStream);
    }
}

static void
skipWhitespace(OBJ inStream) {
    for (;;) {
	int ch;

	ch = scm_peekCharacter(inStream);
	if ((ch != ' ') && (ch != '\t') && (ch != '\n')) {
	    return;
	}
	scm_readCharacter(inStream);
    }
}

// the initial opening parenthesis has already been read!
static OBJ
scm_readList(OBJ inStream) {
    int ch;
    OBJ firstElement, restList;

    skipWhitespace(inStream);
    ch = scm_peekCharacter(inStream);
    if (ch == ')') {
	scm_advanceReadPointer(inStream);
	return SCM_NIL;
    }
    firstElement = scm_read(inStream);
    restList = scm_readList(inStream);
    return new_cons(firstElement, restList);
}

typedef struct bufferStruct {
    char *memory;
    int bufferSize;
    int fillCount;
    char quickMemory[128];
} buffer;

static inline void
allocBuffer(buffer *b) {
    b->memory = b->quickMemory;
    b->bufferSize = sizeof(b->quickMemory);
    b->fillCount = 0;
    b->memory[0] = '\0';
}

static inline void
freeBuffer(buffer *b) {
    if (b->memory != b->quickMemory) {
	free(b->memory);
	b->memory = b->quickMemory;
    }
    b->bufferSize = b->fillCount = 0;
}

static inline void
growBuffer(buffer *b) {
    int newSize = b->bufferSize * 2;

    if (b->memory == b->quickMemory) {
	b->memory = malloc((size_t)newSize);
	memcpy(b->memory, b->quickMemory, b->bufferSize);
    } else {
	b->memory = realloc(b->memory, (size_t)newSize);
    }
    b->bufferSize = newSize;
}

static inline void
putBuffer(buffer *b, int ch) {
    if ((b->fillCount+1) == b->bufferSize) {
	growBuffer(b);
    }
    b->memory[b->fillCount++] = (char)ch;
    b->memory[b->fillCount] = '\0';
}

static bool
allDigits(char *someString) {
    char ch;

    while ((ch = *someString++) != '\0') {
	if (! ((ch >= '0') && (ch <= '9'))) {
	    return C_FALSE;
	}
    }
    return C_TRUE;
}

static OBJ
convert_to_integer(char *someString) {
    // also possible: sscanf
    long intVal = atol(someString);

    return new_integer(intVal);
}

static OBJ
scm_readAtom(OBJ inStream) {
    buffer b;

    allocBuffer(&b);
    for (;;) {
	int ch = scm_peekCharacter(inStream);

	switch (ch) {
	    case '(':
	    case ')':
	    case ' ':
	    case '\t':
	    case '\n':
	    case '\r':
	    case STREAM_EOF:
		{
		    char *theCollectedString = b.memory;

		    freeBuffer(&b);
		    if ((allDigits(theCollectedString))
		     || ((theCollectedString[0] == '-') && allDigits(theCollectedString+1))) {
			return convert_to_integer(theCollectedString);
		    }
		    return new_symbol(theCollectedString);
		}

	    default:
		putBuffer(&b, ch);
		scm_advanceReadPointer(inStream);
		break;
	}
    }
}

// the initial double quote has already been read!
static OBJ
scm_readString(OBJ inStream) {
    buffer b;

    allocBuffer(&b);
    for (;;) {
	int ch = scm_readCharacter(inStream);

	if (ch == '\\') {
	    ch = scm_readCharacter(inStream);
	    switch (ch) {
		case 'n':
		    ch = '\n';
		    break;
		case 't':
		    ch = '\t';
		    break;
		case 'r':
		    ch = '\r';
		    break;
		case '"':
		    ch = '"';
		    break;
		default:
		    break;
	    }
	} else {
	    if (ch == '"') {
		char *theCollectedString = b.memory;

		freeBuffer(&b);
		return new_string(theCollectedString);
	    }
	}
	putBuffer(&b, ch);
    }
}

OBJ
scm_read(OBJ inStream) {
    int ch;

    for (;;) {
	ch = scm_peekCharacter(inStream);
	switch (ch) {
	    case ' ':
	    case '\t':
	    case '\n':
	    case '\r':
		scm_advanceReadPointer(inStream);
		continue;

	    case STREAM_EOF:
		return SCM_EOF;

	    case '(':
		scm_advanceReadPointer(inStream);
		return scm_readList(inStream);

	    case '"':
		scm_advanceReadPointer(inStream);
		return scm_readString(inStream);

	    default:
		return scm_readAtom(inStream);
	}
    }
}
