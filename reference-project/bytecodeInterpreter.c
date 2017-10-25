//////////////////////////////////////////////////////////////////
//
// Copyright 2017 Claus Gittinger
//
// You may use this, but not claim to have written or own it!
// Use at your own risk.
//
//////////////////////////////////////////////////////////////////

#include "scheme.h"
#include <string.h>
#include "bytecodes.h"
#include "builtinSupport.h"

OBJ
evalBytecode(OBJ theFunction, int indexOfFirstArg) {
    unsigned char* bytecode = theFunction->userDefinedFunction.bytecode;
    OBJ* constants = theFunction->userDefinedFunction.constantTable;
    int pc = 0;

    for (;;) {
	int insn = bytecode[pc++];

	switch(insn) {
	    case DROP:
		POP();
		break;

	    case PUSH_CONST:
		{
		    int constIndex = bytecode[pc++];
		    OBJ val = constants[constIndex];

		    PUSH( val );
		    break;
		}

	    case PUSH_ARG:
		{
		    int argIndex = bytecode[pc++];
		    OBJ val = stack[indexOfFirstArg+argIndex];

		    PUSH( val );
		    break;
		}
	    case ADD:
		{
		    OBJ sum = builtin_plus(SP-2);

		    PUSH(sum);
		    break;
		}
	    case SUB:
		{
		    OBJ sum = builtin_minus(SP-2);

		    PUSH(sum);
		    break;
		}
	    case MUL:
		{
		    OBJ sum = builtin_times(SP-2);

		    PUSH(sum);
		    break;
		}
	    case RET_TOP:
		{
		    OBJ retVal = POP();

		    POPARGS();
		    return retVal;
		}
	}
    }
}
