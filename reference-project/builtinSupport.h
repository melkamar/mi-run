//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

//
// builtin functions / syntax
//
// these are called by eval with evaluated arguments on the
// stack. The single passed argument is the stack index of
// the first argument (AP, for argument pointer).
// AP == SP if no arguments were pushed.
// Stack grows upward, so the stack looks like:
// The nth argument can be accessed via the ARG(n) macro.
// The number of arguments is given by the NUMARGS() macro.
//
// ^                      ^
// +----------------------+
// +                      +  <--- SP (next free slot)
// +----------------------+
// +        argN          +
// +----------------------+
// /        ...           /
// +----------------------+
// +        arg1          +
// +----------------------+
// +        arg0          +  <--- AP (indexOfFirstArg)
// +----------------------+
//

#define ARG(index0Based) \
    (stack[indexOfFirstArg+index0Based])

#define NUMARGS() \
    (SP-indexOfFirstArg)

#define POPARGS() \
    SP = indexOfFirstArg

// convenient macro to check for the expected number of arguments
// and raise an error if the number given is different

#define checkNumberOfArguments(name, numExpected)               \
    {                                                           \
	int numGiven = NUMARGS();                               \
	if (numGiven != numExpected) {                          \
	    argumentCountError(name, numExpected, numGiven, C_FALSE);    \
	}                                                       \
    }
