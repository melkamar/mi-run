#include "scheme.h"
#include "functionPrototypes.h"

int
scm_readCharacter(OBJ inStream) {
    switch (tagOf(inStream)) {
	case T_FILESTREAM:
	    {
		FILE *f = inStream->fileStream.theFile;
		int ch;

		if (inStream->fileStream.lookaheadChar != 0) {
		    ch = inStream->fileStream.lookaheadChar;
		    inStream->fileStream.lookaheadChar = 0;
		    return ch;
		}

		if (feof(f)) {
		    return STREAM_EOF;
		}
		ch = getc(inStream->fileStream.theFile);
		return ch;
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
		inStream->stringStream.position = pos + 1;
		return retChar;
	    }

	default:
	    error("argument is not a stream:", inStream);
    }
}

int
scm_peekCharacter(OBJ inStream) {
    switch (tagOf(inStream)) {
	case T_FILESTREAM:
	    {
		FILE *f = inStream->fileStream.theFile;
		int ch;

		if (inStream->fileStream.lookaheadChar != 0) {
		    return inStream->fileStream.lookaheadChar;
		}
		if (feof(f)) {
		    return STREAM_EOF;
		}
		ch = getc(inStream->fileStream.theFile);
		inStream->fileStream.lookaheadChar = ch;
		return ch;
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
		return retChar;
	    }

	default:
	    error("argument is not a stream:", inStream);
    }
}

void
scm_advanceReadPointer(OBJ inStream) {
    switch (tagOf(inStream)) {
	case T_FILESTREAM:
	    fatal("as yet unimplemented");
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

OBJ
scm_read(OBJ inStream) {
    int ch;

    skipWhitespace(inStream);

    ch = scm_readCharacter(inStream);
    fatal("as yet unimplemented: read");
}
