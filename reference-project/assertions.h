#define _assert(cond, msg, filename, lineNr) \
    { \
	if (! (cond)) { \
	    fprintf(stderr, "%s[%d]: assertion failed: %s\n", filename, lineNr, msg); \
	    abort(); \
	} \
    }

#define assert(cond, message) _assert(cond, message, __FILE__, __LINE__)

#define assert_eq(expr, expected, message) _assert(expr==expected, message, __FILE__, __LINE__)

#define assert_true(expr, message) assert_eq(expr, SCM_TRUE, message)

#define assert_false(expr, message) assert_eq(expr, SCM_FALSE, message)
