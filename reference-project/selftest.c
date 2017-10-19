#include "scheme.h"
#include "functionPrototypes.h"

void
selftest() {
    OBJ o;
    long v;
    int ch;

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

    o = new_integer(1234);
    assert( (tagOf(o) == T_INTEGER), "invalid tag");
    assert( ((v = integerVal(o)) == 1234), "invalid value");
}
