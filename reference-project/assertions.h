//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

#define _ASSERT(cond, msg, filename, lineNr) \
    { \
	if (! (cond)) { \
	    fprintf(stderr, "%s[%d]: assertion failed: %s\n", filename, lineNr, msg); \
	    abort(); \
	} \
    }

#define ASSERT(cond, message) _ASSERT(cond, message, __FILE__, __LINE__)

#define ASSERT_eq(expr, expected, message) _ASSERT(expr==expected, message, __FILE__, __LINE__)

#define ASSERT_true(expr, message) ASSERT_eq(expr, SCM_TRUE, message)

#define ASSERT_false(expr, message) ASSERT_eq(expr, SCM_FALSE, message)

#define ASSERT_tag(expr, tag) ASSERT_eq(tagOf(expr), tag, "bad type")
