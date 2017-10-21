# initial makefile, automatically generated.
#
OBJS=main.o reader.o eval.o print.o utils.o selftest.o \
     memory.o symbols.o environment.o builtinFunctions.o \
     builtinSyntax.o

HEADERS=scheme.h functionPrototypes.h assertions.h parameters.h
CFLAGS=-g

all: scheme

$(OBJS): $(HEADERS)

scheme: $(OBJS)
	$(CC) -o scheme $(OBJS)

clean:
	-rm -f $(OBJS)

clobber: clean
	-rm -f scheme *.bak
