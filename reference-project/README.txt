Documentation on the 2017 Prague Scheme interpreter

2 versions:
    a) recursive
	classic implementation; no tailcall elimination,
	leads to stack overflow for recursive loops)
	to compile: #define RECURSIVE
	in parameters.h

    b) continuation passing
	uses own return-stack and a trampline
	to compile: undefine RECURSIVE
	in parameters.h

Implemented features:
    reader supports strings, and integers only
    reader supports quote-character macro
    tailcall elimination

    builtin functions:
	cons, car, cdr,
	set-car!, set-cdr!,
	+, -, *,
	<


TODO (by order of usability):

    Notice: your should read a scheme standard definition
    eg. r5rs as described in http://www.schemers.org/Documents/Standards/R5RS/
    and try to be compatible in the naming of your new functions
    (thus, you may later be able to use existing standard library code)

    - performance of AST interpreter:
	a) rewrite macros to use tagged integers (this really rocks!)

	b) eliminate CP_eval roundtrip via trampoline for non-cons (non-calls)
	   (think what happens in CP_evalList for each argument, which is either
	    a symbol or an integer. Can you inline the handling for those?)

	c) avoid hashing for small environments
	   (special code and possibly a separate object layout and tag for those?)

    - add support for Float numbers (float primitive type)
      (attention: care for mixed mode arithmetic in the buitins)

    - handle integer overflow
	a) by raising an error
	b) (better)
	   by adding a bignum package (gnu)
	   and adding a LargeInteger primitive type
	   and convert results to bignums when overflowing

    - add string support functions

    - add file- and socket I/O functions (open, close, socket-open, accept, listen, bind etc.)

    - add vector support (a primitive type and reader support)

    - add byte-vector support (a primitive type and reader support)

    - add support for Fraction numbers (fraction primitive type)
      (attention: care for mixed mode arithmetic in the buitins)
